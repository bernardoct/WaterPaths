######
#
# Author: Brandon Barker
# Authoremail: brandonDOTbarkerATcornellDOTedu
# Start time: December 12, 2018
#
######

{ stdenv
}:

stdenv.mkDerivation rec {
  name = "Borg";

  buildInputs = [ ];

  src = builtins.fetchGit {
    url = "git@bitbucket.org:dmh309/serial-borg-moea.git";
    rev = "2c7702638d42349824e305036fc6eb4a04a8a539";
  };

  buildPhase = ''
    make
  '';

  installPhase = ''
    mkdir $out
    cp -R * $out
  '';

}
