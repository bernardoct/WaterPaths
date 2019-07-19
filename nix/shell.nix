######
#
# Author: Brandon Barker
# Authoremail: brandonDOTbarkerATcornellDOTedu
# Start time: December 12, 2018
#
######

with import <nixpkgs> {};
let
  pkgs    = import <nixpkgs> {};
  Borg = (import ./default.nix).borg;
in
stdenv.mkDerivation {
  name = "WaterPathsEnv";
  buildInputs = [
    nix
    bash
    
    # MPI-related packages
    binutils
    gcc
    openmpi
    openssh
    #
    # Project code dependencies
    #
    # MOEAFramework
    # Pareto
    # boost
    Borg
    
  ];
  src = null;
  shellHook = ''
    mkdir -p ./Borg
    ln -sfn ${Borg}/ ./Borg/
  '';
}



