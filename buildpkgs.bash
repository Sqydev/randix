#!/usr/bin/env bash
set -e

if [ ! -f "./package.conf" ]; then 
    echo "NO package.conf! :0"
    exit 1
fi

source ./package.conf

OUT="$(pwd)/packages"
VOID_OUT="$OUT/void"
AUR_OUT="$OUT/aur"

SRC_SHA256_SUM=$(curl -Ls "$SRC_URL" | sha256sum | cut -d' ' -f1)
TEMP_FILE="./v$version.tar.gz"
if curl -LfsS "$SRC_URL" -o "$TEMP_FILE"; then
    SRC_SHA256_SUM=$(sha256sum "$TEMP_FILE" | cut -d' ' -f1)
    rm "$TEMP_FILE"
else
    echo "Can't download from $SRC_URL" >&2
    exit 1
fi

echo "=== Starting build process ==="
echo "Version: $version | Src hash: $SRC_SHA256_SUM"
mkdir -p "$OUT"

# --- VOID LINUX ---
echo "[1/5] Generating Void Template and .xbps package"
mkdir -p "$VOID_OUT"

cat > "$VOID_OUT/template" <<EOF
pkgname=$name
version=$version
revision=$release
archs="$arch"
build_style=$build_style
make_build_target="$make_build_target"
make_install_args="$make_install_args"
short_desc="$summary"
maintainer="$maintainer"
license="$license"
homepage="$url"
distfiles="$SRC_URL"
checksum=$SRC_SHA256_SUM
wrksrc="$name-$version"

EOF

echo "[INFO] Template generated at $VOID_OUT/template"

echo "[INFO] Creating .xbps package...(Btw. you only give the official repo template not .xbps)"

cdw=$(pwd)
cd "$VOID_OUT"

xbps-create -A x86_64 \
            -n "${name}-${version}_${release}" \
            -s "$summary" \
			"$VOID_OUT"

cd "$cdw"

echo "[INFO] Void glibc package built: $VOID_OUT/bin/"

echo "[2/5] Generating AUR package"
mkdir -p "$AUR_OUT"

cat > "$AUR_OUT/template" <<EOF
pkgname=$name
pkgver=$version
pkgrel=$release
pkgdesc="$summary."
arch=($arch)
url="$url"
license=('$license')
depends=($dependencies)

EOF

echo "[INFO] AUR package generated at $AUR_OUT/PKGBUILD"
