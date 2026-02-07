{ stdenv }:

stdenv.mkDerivation {
  pname = "randix";
  version = "1.0.0";
  src = ../../compiled/normal/randix-normal-glibc;
  dontUnpack = true;
  installPhase = ''
    mkdir -p $out/bin
    cp $src $out/bin/randix
    chmod +x $out/bin/randix
  '';
  meta = {
    description = "A simple terminal program that displays random characters.";
    homepage = "https://github.com/Sqydev/randix";
    license = stdenv.lib.licenses.gpl3;
    platforms = [ "x86_64-linux" ];
  };
}
