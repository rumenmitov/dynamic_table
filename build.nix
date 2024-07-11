{ pkgs ? import <nixpkgs> {}, flag ? "" }:
pkgs.stdenv.mkDerivation {
  name = "Dynamic Table";
  src = ./.;
    
  buildInputs = with pkgs; [ gnuplot ];

  buildPhase = "make " ++ flag;

  installPhase = ''
		    mkdir -p $out/build
        cp -r build/* $out/build
      '';

}
