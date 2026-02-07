#!/usr/bin/env bash
if [ ! -f "./package.conf" ]; then echo "Brak package.conf"; exit 1; fi
source ./package.conf

OUT_DIR="$(pwd)/packages"
DEB_FILE="${name}_${version}_amd64.deb"
RPM_FILE=$(ls $OUT_DIR/*.rpm 2>/dev/null | xargs basename 2>/dev/null)

echo "=== Doing LOCAL package test (with net access for deps) ==="
echo "Katalog pakietów: $OUT_DIR"
echo "-----------------------------------------------------------"

# 1. ARCH LINUX
#echo "[TEST 1/5] Arch Linux (PKGBUILD)"
#docker run --rm -v "$OUT_DIR/aur:/build" archlinux:latest sh -c "
#  pacman-key --init && \
#  pacman -Syu --noconfirm base-devel sudo && \
#  useradd -m builder && echo 'builder ALL=(ALL) NOPASSWD: ALL' >> /etc/sudoers && \
#  chown -R builder:builder /build && \
#  su builder -c 'cd /build && makepkg -si --noconfirm' && \
#  randix --version
#"

# 2. DEBIAN
#echo -e "\n[TEST 2/5] Debian (Local .deb)"
#docker run --rm -v "$OUT_DIR:/pkgs" debian:stable-slim sh -c "
#  apt-get update && \
#  apt-get install -y /pkgs/$DEB_FILE && \
#  randix --version
#"

# 3. FEDORA
#echo -e "\n[TEST 3/5] Fedora (Local .rpm)"
#docker run --rm -v "$OUT_DIR:/pkgs" fedora:latest sh -c "
#	dnf install -y /pkgs/$RPM_FILE && \
#	randix --version
#"
#
# 4. NIX 
#echo -e "\n[TEST 4/5] Nix (Local nix + patchelf)"
#docker run --rm -v "$OUT_DIR/nix:/build" nixos/nix sh -c "
#  nix-channel --update && \
#  nix-env -iA nixpkgs.patchelf nixpkgs.glibc && \
#  RESULT=\$(nix-build /build/default.nix) && \
#  
#  LD_PATH=\$(nix-build '<nixpkgs>' -A glibc --no-out-link)/lib/ld-linux-x86-64.so.2 && #\
#  
#  patchelf --set-interpreter \"\$LD_PATH\" \$RESULT/bin/randix && \
#  
#  \$RESULT/bin/randix --version
#"

# 2. Void
# TODO
