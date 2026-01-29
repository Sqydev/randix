{
	description = "Randix flake";

	# 24.11 and not 25.05 becouse of compatybility
	inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.11";

	outputs = { self, nixpkgs }:
	let
		system = "x86_64-linux";
		pkgs = import nixpkgs { inherit system; };
	in
	{
		devShells.${system}.default = pkgs.buildFHSUserEnv {
				targetPkgs = pkgs: with pkgs; [
					gcc
					glibc
					fish
				];
				runScript = "fish";
			};
	};
}

