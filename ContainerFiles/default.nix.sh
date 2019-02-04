#
# #TODO: explore using pure environment using a shell script to exec nix-shell
#

with import <nixpkgs> {};
# with pkgs.python36Packages;
stdenv.mkDerivation {
  name = "impurePythonEnv";
  buildInputs = [
    python36Full
    python36Packages.mpi4py
    python36Packages.numpy    
    python36Packages.pip
    python36Packages.scipy
    python36Packages.virtualenv    
  ];
  src = null;
  shellHook = ''
    export LANG=en_US.UTF-8
  '';
}
