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
    url = "git@bitbucket.org:dmh309/serial-borg-moea.git";
    rev = "2c7702638d42349824e305036fc6eb4a04a8a539";
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
    mv *.exe $out/bin/
    cp -R * $out
  '';

}
