{ pkgs ? import <nixpkgs> {}, flags ? "" }:
pkgs.stdenv.mkDerivation {
  name = "Dynamic Table";
  src = ./.;

  buildInputs = with pkgs; [ gnuplot ];

  buildPhase = "make " + flags;

  installPhase = ''
		mkdir -p $out/build
		cp -r build/* $out/build/
    cp -r results.png $out/
    cp -r results.dat $out/
  '';

}
