######
#
# Author: Brandon Barker
# Authoremail: brandonDOTbarkerATcornellDOTedu
# Start time: December 12, 2018
#
######

{ pkgs, stdenv
  , compileParallel ? true
  , mpi ? pkgs.openmpi
}:

stdenv.mkDerivation rec {
  name = "Borg";

  parallelPkgs = if compileParallel then [ mpi ] else [];

  buildInputs = [ ] ++ parallelPkgs;

  src = builtins.fetchGit {
    #url = "git@bitbucket.org:dmh309/serial-borg-moea.git";
    rev = "7c2bd969e69de11373784070c10e6e734439b6c8";
    # For testing; normally don't need to specify 'ref':
    url = "git@bitbucket.org:bebarker/serial-borg-moea.git";
    ref = "nix_build";
  };

  parallelMake = "make compile-parallel";
  serialMake = "make compile";
  
  buildPhase =
  if compileParallel
  then ''
    $serialMake
    $parallelMake
  ''
  else serialMake;
  
  installPhase = ''
    mkdir -p $out/bin
    mkdir -p $out/include
    mkdir -p $out/lib
    mv *.exe $out/bin/
    mv *.a *.so $out/lib/
    mv *.h $out/include/
    cp -R * $out/ # for debugging
  '';

}
