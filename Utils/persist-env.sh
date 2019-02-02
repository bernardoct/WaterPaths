#!/bin/bash

if [ -z "$1" ] || [ "${1: -4}" != ".nix" ] || [ ! -f "$1" ]
  then
      echo "No .nix file supplied"
      exit -1
fi

ENV_DRV=$(nix-instantiate "$1")
cp "$ENV_DRV" ./env_backup.drv
chmod u+rw ./env_backup.drv

#
# Use the following to restore an environment
#
nix-env --set "$ENV_DRV"

#
# Since nix-env --set isn't compatible with mutations like `nix-env -i foo`
# we will use this in practice instead:
#
nix-env -if "$1"


NIXPKGS_VERSION=$(nix-instantiate --eval '<nixpkgs/lib>' -A version)
NIXOS_VERSION=$(nix-instantiate --eval '<nixos/lib>' -A version)

printf "nixpkgs: %s\\nnixos: %s" "$NIXPKGS_VERSION" "${NIXOS_VERSION}" > .nix_versions
