with import <nixpkgs> {};
# with pkgs.python36Packages;
let
  MOEAFramework = stdenv.mkDerivation {
    name = "MOEAFramework";
    
    src = fetchurl {
      url = https://github.com/MOEAFramework/MOEAFramework/releases/download/v2.12/MOEAFramework-2.12-Demo.jar;
      sha256 = "0kmfdmpzpfl7f1hnfck7nyfimsd92d2fndsypfnfar6gqw5cl3w4";
    };
    phases = "installPhase";
    
    installPhase = ''
      mkdir -p $out/
      cp $src $out/MOEAFramework-2.12-Demo.jar
    '';
  };
  Pareto = stdenv.mkDerivation {
    name = "Pareto";
    
    src = fetchurl {
      url = https://github.com/matthewjwoodruff/pareto.py/archive/1.1.1-3.tar.gz;
      sha256 = "1k057g9rgm4a9k8nfkibrqxh56kqkbs635f5xmgbzlbcchbxzp4p";
    };
    phases = "installPhase";
    
    installPhase = ''
      mkdir -p $out/
      tar -C $out/ -xzf $src
    '';
  };
  Borg = stdenv.mkDerivation {    
    name = "Borg";
    src = builtins.fetchGit {
      url = "git@bitbucket.org:dmh309/serial-borg-moea.git";
      rev = "2c7702638d42349824e305036fc6eb4a04a8a539";
    };
    buildInputs = [
      # gnumake
    ];
    builder = ./borg-standard-builder.sh;
  };
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
}
