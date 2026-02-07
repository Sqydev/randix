#!/usr/bin/env bash

echo "--- Testing Arch Linux (PKGBUILD) ---"
docker run --rm -v "$(pwd)/packages/aur:/build" archlinux:latest sh -c "
  pacman -Syu --noconfirm base-devel sudo && \
  useradd -m builder && \
  echo 'builder ALL=(ALL) NOPASSWD: ALL' >> /etc/sudoers && \
  chown -R builder:builder /build && \
  su builder -c 'cd /build && makepkg -fp PKGBUILD'
"

echo "--- Testing Debian (.deb) ---"
docker run --rm -v "$(pwd)/packages/deb:/build" debian:stable-slim sh -c "
  dpkg -i /build/randix_1.0.0_amd64.deb || apt-get install -f -y && \
  /usr/bin/randix --version || randix
"

echo "--- Testing Fedora (RPM) ---"
docker run --rm -v "$(pwd)/packages/rpm:/build" fedora:latest sh -c "
  dnf install -y /build/RPMS/x86_64/*.rpm && \
  /usr/bin/randix
"

echo "--- Testing Nix (default.nix) ---"
docker run --rm -v "$(pwd):/src" nixos/nix sh -c "
  cd /src/packages/nix && nix-build
"

echo "--- Testing Void Linux (xbps-src) ---"
docker run --rm -v "$(pwd)/packages/void:/build-template" voidlinux/voidlinux sh -c "
  xbps-install -Syu && xbps-install -y git base-devel && \
  
  git clone --depth=1 https://github.com/void-linux/void-packages.git /void-packages && \
  cd /void-packages && \
  ./xbps-src binary-bootstrap && \
  
  mkdir -p srcpkgs/randix && \
  cp /build-template/template srcpkgs/randix/template && \
  
  ./xbps-src pkg randix && \
  
  xbps-install -y --repository=hostdir/binpkgs randix && \
  randix
"
