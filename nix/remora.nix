with import <nixpkgs> {};
let
  mpiP = stdenv.mkDerivation {
    name = "mpiP";
    
    src = fetchurl {
      url = https://sourceforge.net/projects/mpip/files/mpiP/mpiP-3.4.1/mpiP-3.4.1.tar.gz;
      sha256 = "1w4n693im81qgbq0m8f0cv8nd8l2h8srriczby8nl7i1fdyz72v8";
    };
    phases = "installPhase";
    
    installPhase = ''
      mkdir -p $out/
      tar -C $out -xzf $src
    '';
  };
  remora = stdenv.mkDerivation {
    name = "remora";
    
    src = fetchurl {
      url = https://github.com/TACC/remora/archive/v1.8.3.tar.gz;
      sha256 = "08n72i6r8y14zr5wlcllpccv4q9xz6lf0j2d5k7kmyz55qid8kb8";
    };
    phases = "installPhase";
    
    buildInputs = with stdenv; [ binutils gcc gfortran openmpi ];
    
    installPhase = ''
      source $stdenv/setup
      
      mkdir -p $out/
      tar -C $out -xzf $src
      
      export REMORA_INSTALL_PREFIX=$out
      cd $out/remora-1.8.3
      $out/remora-1.8.3/install.sh
    '';
  };
in
stdenv.mkDerivation {
  name = "ProfilingEnv";
  buildInputs = [
    nix
    bash
    vim
    
    # MPI-related packages
    binutils
    gcc
    gfortran
    openmpi
    openssh
    
    #
    # Project code dependencies
    #
    mpiP
    remora
  ];
  src = null;
  shellHook = ''
    export PATH=$PATH:${remora}/bin
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${remora}/lib
    export REMORA_BIN=${remora}/bin
  '';
}
