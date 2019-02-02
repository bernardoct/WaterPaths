#!/bin/bash

# shellcheck source=/dev/null
source "Utils/singularity-build-common.sh"

BASEDIR=$(dirname "${BASH_SOURCE[0]}")
export BASEDIR

#
# **** Pick a Distro to build below by uncommenting a section ****
#
source "ContainerFiles/alpine_envs.sh"

source "Utils/image_tag.sh"
NIXUSER="nixuser"
REPO="nix_${BASEOS}_openmpi"
TAG=$(git_image_tag)
export ENVSDIR="/nixenv/$NIXUSER"
export NIX_OMPI_IMAGE="${REPO}_${TAG}"
echo "NIX_OMPI_IMAGE is $NIX_OMPI_IMAGE"

# echo "SINGULARITY_DOCKER_USERNAME is set to ${SINGULARITY_DOCKER_USERNAME}"
# echo "SINGULARITY_DOCKER_PASSWORD is set to ${SINGULARITY_DOCKER_PASSWORD}"
cat "SingTemplate" | envsubst '${BASEOS} ${ENVSDIR}' > "Singularity.${NIX_OMPI_IMAGE}"
sudo singularity --debug build "${NIX_OMPI_IMAGE}.img" "Singularity.${NIX_OMPI_IMAGE}"
sudo chown "$SING_USER:$SING_GROUP" "${NIX_OMPI_IMAGE}.img"
