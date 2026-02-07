{ pkgs ? import <nixpkgs> {} }:

pkgs.stdenv.mkDerivation rec {
  pname = "randix";
  version = "1.0.0";
  src = pkgs.fetchurl {
    url = "https://github.com/Sqydev/randix/releases/download/v1.0.0/randix-normal-glibc";
    sha256 = "0025ac5df997530defd8cc4e8d370a176a1536cebbf6714f92a1edfc53843a97";
  };
  dontUnpack = true;
  installPhase = ''
    mkdir -p $out/bin
    cp $src $out/bin/randix
    chmod +x $out/bin/randix
  '';
  meta = {
    description = "A simple terminal program that displays random characters.";
    homepage = "https://github.com/Sqydev/randix";
    license = pkgs.lib.licenses.gpl3;
    platforms = [ "x86_64-linux" ];
  };
}
