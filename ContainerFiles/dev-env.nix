with import <nixpkgs> { };
# For testing local nixpkgs clone, swap with above
# with import ((builtins.getEnv "HOME") + "/workspace/nixpkgs") { }; # or:
# with import "../nixpkgs" { };
# Note taht the above are not accessible during docker build
{ openmpiDevEnv = buildEnv {
  name = "openmpi-dev-env";
  paths = [
    git
    
    #
    # Always include nix, or environment will break
    # Include bash for base OSes without bash
    #
    nix
    bash
    
    #
    # MPI-related packages
    #
    binutils
    gfortran
    openmpi
    openssh
  ];

};}

#######################################
#
# Refs:
# https://stackoverflow.com/questions/46165918/how-to-get-the-name-from-a-nixpkgs-derivation-in-a-nix-expression-to-be-used-by/46173041#46173041
##