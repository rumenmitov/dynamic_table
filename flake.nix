{
  description = "Dynamic Table for Managing Virtual Memory";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs   = nixpkgs.legacyPackages.${system};
    in
    {

      packages.${system}.default = import ./build.nix { inherit pkgs; };
      packages.${system}.debug   = import ./build.nix { inherit pkgs; flag = "debug"; };
      packages.${system}.graph   = import ./build.nix { inherit pkgs; flag = "graph"; };

  };
}
