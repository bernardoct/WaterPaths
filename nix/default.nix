######
#
# Author: Brandon Barker
# Authoremail: brandonDOTbarkerATcornellDOTedu
# Start time: December 12, 2018
#
######

let
  pkgs    = import <nixpkgs> {};
in rec {
  borg = pkgs.callPackage ./borg.nix { };
}
