#!/usr/bin/env bash
set -e

if [ ! -f "./package.conf" ]; then
    echo "NO package.conf :0"
    exit 1
fi
source ./package.conf

ARCH_GENERIC="$arch"
case "$ARCH_GENERIC" in
    x86_64)
        ARCH_DEB="amd64"
        ARCH_RPM="x86_64"
        ARCH_AUR="x86_64"
        ARCH_VOID="x86_64"
        ARCH_NIX="x86_64-linux"
        ;;
    aarch64|arm64)
        ARCH_DEB="arm64"
        ARCH_RPM="aarch64"
        ARCH_AUR="aarch64"
        ARCH_VOID="aarch64"
        ARCH_NIX="aarch64-linux"
        ;;
    *)
        echo "Unsupported architecture: $ARCH_GENERIC"
        exit 1
        ;;
esac

OUT="$(pwd)/packages"
BIN_SRC="compiled/normal/randix-normal-glibc"
INSTALL_NAME="$name"

if [ ! -f "$BIN_SRC" ]; then
    echo "NO BIN IN $BIN_SRC :0"
    exit 1
fi

mkdir -p "$OUT"
BIN_FULL_PATH="$(realpath $BIN_SRC)"
SHA256_SUM=$(sha256sum "$BIN_SRC" | cut -d' ' -f1)

# --- ARCH LINUX (AUR) ---
echo "Doing AUR (PKGBUILD)"
mkdir -p "$OUT/aur"
cat > "$OUT/aur/PKGBUILD" <<EOF
pkgname=$name
pkgver=$version
pkgrel=$release
pkgdesc="$summary"
arch=('$ARCH_AUR')
url="$url"
license=('$license')
depends=($dependencies)

package() {
    install -Dm755 "\$srcdir/../../$BIN_SRC" "\$pkgdir/usr/bin/$INSTALL_NAME"
}
EOF

# --- DEBIAN (.deb) ---
echo "Doing .deb"
DEBROOT="$OUT/deb/${name}_${version}_${ARCH_DEB}"
mkdir -p "$DEBROOT/DEBIAN"
mkdir -p "$DEBROOT/usr/bin"
cp "$BIN_SRC" "$DEBROOT/usr/bin/$INSTALL_NAME"
chmod 755 "$DEBROOT/usr/bin/$INSTALL_NAME"

deb_deps=$(echo "$dependencies" | sed "s/'//g; s/ /, /g")

{
    echo "Package: $name"
    echo "Version: $version-$release"
    echo "Section: utils"
    echo "Priority: optional"
    echo "Architecture: $ARCH_DEB"
    echo "Maintainer: $maintainer"
    [[ -n "$deb_deps" ]] && echo "Depends: $deb_deps"
    echo "Description: $summary"
} > "$DEBROOT/DEBIAN/control"

dpkg-deb --build --root-owner-group "$DEBROOT" "$OUT/deb/${name}_${version}_${ARCH_DEB}.deb"

# --- FEDORA (RPM) ---
echo "Doing rpm"
RPMROOT="$OUT/rpm"
mkdir -p "$RPMROOT"/{BUILD,RPMS,SOURCES,SPECS,SRPMS}
cp "$BIN_SRC" "$RPMROOT/SOURCES/$INSTALL_NAME"

cat > "$RPMROOT/SPECS/$name.spec" <<EOF
Name:           $name
Version:        $version
Release:        $release%{?dist}
Summary:        $summary
License:        $license
BuildArch:      $ARCH_RPM

%description
$summary

%install
mkdir -p %{buildroot}/usr/bin
cp %{_sourcedir}/$INSTALL_NAME %{buildroot}/usr/bin/$INSTALL_NAME
chmod 755 %{buildroot}/usr/bin/$INSTALL_NAME

%files
/usr/bin/$INSTALL_NAME
EOF
# rpmbuild --define "_topdir $RPMROOT" -bb "$RPMROOT/SPECS/$name.spec"

# --- VOID LINUX ---
echo "Doing void things"
mkdir -p "$OUT/void"
cat > "$OUT/void/template" <<EOF
pkgname=$name
version=$version
revision=$release
archs="$ARCH_VOID"
short_desc="$summary"
maintainer="$maintainer"
license="$license"
homepage="$url"
checksum="$SHA256_SUM"

do_install() {
	vbin "\$(pwd)/../../$BIN_SRC" $INSTALL_NAME
}
EOF

# --- NIXOS ---
echo "Doing default.nix"
mkdir -p "$OUT/nix"
cat > "$OUT/nix/default.nix" <<EOF
{ stdenv }:

stdenv.mkDerivation {
  pname = "$name";
  version = "$version";
  src = ../../$BIN_SRC;
  dontUnpack = true;
  installPhase = ''
    mkdir -p \$out/bin
    cp \$src \$out/bin/$INSTALL_NAME
    chmod +x \$out/bin/$INSTALL_NAME
  '';
  meta = {
    description = "$summary";
    homepage = "$url";
    license = stdenv.lib.licenses.gpl3;
    platforms = [ "$ARCH_NIX" ];
  };
}
EOF

echo -e "\nDone!"
