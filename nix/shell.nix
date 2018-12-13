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
  borg = (import ./default.nix).borg;
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
    borg
    
  ];
  src = null;
}



