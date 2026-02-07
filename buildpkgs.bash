#!/usr/bin/env bash
set -e

if [ ! -f "./package.conf" ]; then 
    echo "NO package.conf! :0"
    exit 1
fi

source ./package.conf

# Sprawdzamy czy zmienne z package.conf istnieją
if [ -z "$BIN_NAME_REMOTE" ] || [ -z "$BIN_LOCAL" ]; then
    echo "package.conf! is bad"
    exit 1
fi

if [ ! -f "$BIN_LOCAL" ]; then 
    echo "NO BIN IN: $BIN_LOCAL"
    exit 1
fi

SHA256_SUM=$(sha256sum "$BIN_LOCAL" | cut -d' ' -f1)
OUT="$(pwd)/packages"
GITHUB_URL="$url/releases/download/v$version"
DEB_ARCH="amd64"
RPM_ARCH="x86_64"

echo "=== Starting build process ==="
echo "Version: $version | Hash: $SHA256_SUM"
echo "Remote file: $BIN_NAME_REMOTE"
mkdir -p "$OUT"

# --- A. ARCH LINUX (AUR) ---
echo "[1/5] Generating PKGBUILD (Arch)..."
mkdir -p "$OUT/aur"
cat > "$OUT/aur/PKGBUILD" <<EOF
pkgname=$name
pkgver=$version
pkgrel=$release
pkgdesc="$summary"
arch=('x86_64')
url="$url"
license=('$license')
depends=($dependencies)
source=("$name::$GITHUB_URL/$BIN_NAME_REMOTE")
sha256sums=('$SHA256_SUM')

package() {
    install -Dm755 "\$srcdir/$name" "\$pkgdir/usr/bin/\$pkgname"
}
EOF

# --- B. NIX (Nixpkgs) ---
echo "[2/5] Generating default.nix (Nix)..."
mkdir -p "$OUT/nix"
cat > "$OUT/nix/default.nix" <<EOF
{ pkgs ? import <nixpkgs> {} }:

pkgs.stdenv.mkDerivation rec {
  pname = "$name";
  version = "$version";
  src = pkgs.fetchurl {
    url = "$GITHUB_URL/$BIN_NAME_REMOTE";
    sha256 = "$SHA256_SUM";
  };
  dontUnpack = true;
  installPhase = ''
    mkdir -p \$out/bin
    cp \$src \$out/bin/$name
    chmod +x \$out/bin/$name
  '';
  meta = {
    description = "$summary";
    homepage = "$url";
    license = pkgs.lib.licenses.gpl3;
    platforms = [ "x86_64-linux" ];
  };
}
EOF

# --- C. VOID LINUX ---
echo "[3/5] Building Void Linux package (.xbps for glibc)..."
VOID_OUT="$OUT/void"
mkdir -p "$VOID_OUT/bin" "$VOID_OUT/binpkgs" "$VOID_OUT/repo"

BIN_DIR="$VOID_OUT/binpkgs/$name"
mkdir -p "$BIN_DIR/usr/bin"

if [ ! -f "$BIN_LOCAL" ]; then
    echo "[INFO] Downloading $BIN_NAME_REMOTE from GitHub..."
    curl -L -o "$BIN_DIR/usr/bin/$name" "$GITHUB_URL/$BIN_NAME_REMOTE"
else
    cp "$BIN_LOCAL" "$BIN_DIR/usr/bin/$name"
fi

chmod +x "$BIN_DIR/usr/bin/$name"
SHA256_SUM=$(sha256sum "$BIN_DIR/usr/bin/$name" | cut -d' ' -f1)

cat > "$VOID_OUT/template" <<EOF
pkgname=$name
version=$version
revision=$release
archs="x86_64"
short_desc="$summary"
maintainer="$maintainer"
license="$license"
homepage="$url"
distfiles="$BIN_DIR/usr/bin/$name"
checksum="$SHA256_SUM"

build_style=none

do_build() { :; }

do_install() {
    vbin $BIN_DIR/usr/bin/$name $name
}
EOF

echo "[INFO] Template generated at $VOID_OUT/template"

pushd "$VOID_OUT" > /dev/null

echo "[INFO] Creating .xbps package..."
xbps-create -A x86_64 \
            -n "${name}-${version}_${release}" \
            -s "$summary" \
            "$BIN_DIR"

mkdir -p repo
xbps-rindex -a *.xbps

mkdir -p bin
cp *.xbps bin/ 2>/dev/null || true

popd > /dev/null
echo "[INFO] Void glibc package built: $VOID_OUT/bin/"

# --- D. DEBIAN (.deb) ---
echo "[4/5] Building .deb..."
DEBROOT="$OUT/deb_tmp"
mkdir -p "$DEBROOT/DEBIAN" "$DEBROOT/usr/bin"
cp "$BIN_LOCAL" "$DEBROOT/usr/bin/$name"
chmod 755 "$DEBROOT/usr/bin/$name"
deb_deps=$(echo "$dependencies" | sed "s/'//g; s/ /, /g")

cat > "$DEBROOT/DEBIAN/control" <<EOF
Package: $name
Version: $version-$release
Section: utils
Priority: optional
Architecture: $DEB_ARCH
Maintainer: $maintainer
Depends: $deb_deps
Description: $summary
EOF
dpkg-deb --build --root-owner-group "$DEBROOT" "$OUT/${name}_${version}_${DEB_ARCH}.deb"
rm -rf "$DEBROOT"

# --- E. FEDORA (RPM) ---
echo "[5/5] Building .rpm..."
if command -v rpmbuild &> /dev/null; then
    RPMROOT="$OUT/rpm_tmp"
    rm -rf "$RPMROOT"
    mkdir -p "$RPMROOT"/{BUILD,RPMS,SOURCES,SPECS,SRPMS,db}
    cp "$BIN_LOCAL" "$RPMROOT/SOURCES/$name"
    cat > "$RPMROOT/SPECS/$name.spec" <<EOF
Name:            $name
Version:         $version
Release:         $release
Summary:         $summary
License:         $license

%description
$summary

%install
mkdir -p %{buildroot}/usr/bin
cp %{_sourcedir}/$name %{buildroot}/usr/bin/$name
chmod 755 %{buildroot}/usr/bin/$name

%files
/usr/bin/$name
EOF
    rpmbuild --define "_topdir $RPMROOT" --dbpath "$RPMROOT/db" -bb "$RPMROOT/SPECS/$name.spec"
    find "$RPMROOT/RPMS" -name "*.rpm" -exec mv {} "$OUT/" \;
    rm -rf "$RPMROOT"
else
    echo "SKIP: No rpmbuild - install 'rpm-build' or 'rpmbuild' to gen RPM."
fi

echo "--------------------------------------------------------"
echo "Done! Packages are in: $OUT"
