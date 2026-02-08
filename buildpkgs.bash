#!/usr/bin/env bash
set -e

if [ ! -f "./package.conf" ]; then 
    echo "NO package.conf! :0"
    exit 1
fi

source ./package.conf

OUT="$(pwd)/packages"
ARCH_BIN_SHA256_SUM=$(curl -Ls "$BIN_URL_ARCH" | sha256sum | cut -d' ' -f1)
DEB_BIN_SHA256_SUM=$(curl -Ls "$BIN_URL_DEB" | sha256sum | cut -d' ' -f1)
VOID_BIN_SHA256_SUM=$(curl -Ls "$BIN_URL_VOID" | sha256sum | cut -d' ' -f1)
NIXOS_BIN_SHA256_SUM=$(curl -Ls "$BIN_URL_NIXOS" | sha256sum | cut -d' ' -f1)
RPM_BIN_SHA256_SUM=$(curl -Ls "$BIN_URL_RPM" | sha256sum | cut -d' ' -f1)

echo "=== Starting build process ==="
echo "Version: $version"
echo "Arch hash:  $ARCH_BIN_SHA256_SUM"
echo "Deb hash:   $DEB_BIN_SHA256_SUM"
echo "Void hash:  $VOID_BIN_SHA256_SUM"
echo "Nixos hash: $NIXOS_BIN_SHA256_SUM"
echo "Rpm hash:   $RPM_BIN_SHA256_SUM"
                                                                                                             exit
mkdir -p "$OUT"

# --- VOID LINUX ---
echo "[1/5] Generating Void Template And Building Package"
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
