{ pkgs ? import <nixpkgs> {} }:
pkgs.mkShell {
  name = "Dynamic Table - Shell Environment";
  src = ./.;

  buildInputs = with pkgs; [ gnuplot zsh ];

  shellHook = ''
    zsh
		echo "Run with <make> for base program."
    echo "Run with <make debug> for debugging."
    echo "Run with <make graph> for graph."
  '';
}
