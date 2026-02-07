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
echo "[TEST 1/5] Arch Linux (PKGBUILD)"
docker run --rm -v "$OUT_DIR/aur:/build" archlinux:latest sh -c "
  pacman-key --init && \
  pacman -Syu --noconfirm base-devel sudo && \
  useradd -m builder && echo 'builder ALL=(ALL) NOPASSWD: ALL' >> /etc/sudoers && \
  chown -R builder:builder /build && \
  su builder -c 'cd /build && makepkg -si --noconfirm' && \
  randix --version
"

# 2. DEBIAN
echo -e "\n[TEST 2/5] Debian (Local .deb)"
if [ -f "$OUT_DIR/$DEB_FILE" ]; then
    docker run --rm -v "$OUT_DIR:/pkgs" debian:stable-slim sh -c "
      apt-get update && \
      apt-get install -y /pkgs/$DEB_FILE && \
      randix --version
    "
else
    echo "SKIP: Nie znaleziono $DEB_FILE"
fi

# 3. FEDORA
echo -e "\n[TEST 3/5] Fedora (Local .rpm)"
if [ -n "$RPM_FILE" ]; then
    docker run --rm -v "$OUT_DIR:/pkgs" fedora:latest sh -c "
      dnf install -y /pkgs/$RPM_FILE && \
      randix --version
    "
else
    echo "SKIP: Nie znaleziono pliku RPM"
fi

# 4. NIX (Z prawidłową obsługą interpretera)
echo -e "\n[TEST 4/5] Nix (Local nix + patchelf)"
docker run --rm -v "$OUT_DIR/nix:/build" nixos/nix sh -c "
  nix-channel --update && \
  nix-env -iA nixpkgs.patchelf nixpkgs.glibc && \
  RESULT=\$(nix-build /build/default.nix) && \
  
  # Pobieramy ścieżkę do ld-linux bezpośrednio z zainstalowanego glibc
  LD_PATH=\$(nix-build '<nixpkgs>' -A glibc --no-out-link)/lib/ld-linux-x86-64.so.2 && \
  
  # Naprawiamy binarkę
  patchelf --set-interpreter \"\$LD_PATH\" \$RESULT/bin/randix && \
  
  \$RESULT/bin/randix --version
"

# 5. VOID LINUX (Z wymuszeniem mirrora)
echo -e "\n[TEST 5/5] Void Linux (Local template)"
docker run --rm -v "$OUT_DIR/void:/build-input" voidlinux/voidlinux sh -c "
  # Zmiana mirrora na stabilniejszy
  mkdir -p /etc/xbps.d && echo 'repository=https://repo-default.voidlinux.org/current' > /etc/xbps.d/00-repository-main.conf && \
  xbps-install -Syu xbps && \
  xbps-install -yu ca-certificates && \
  update-ca-certificates && \
  xbps-install -y git base-devel && \
  git clone --depth=1 https://github.com/void-linux/void-packages.git /void-packages && \
  cd /void-packages && ./xbps-src binary-bootstrap && \
  mkdir -p srcpkgs/randix && cp /build-input/template srcpkgs/randix/template && \
  ./xbps-src pkg randix && \
  xbps-install -y --repository=hostdir/binpkgs randix && \
  randix --version
"
