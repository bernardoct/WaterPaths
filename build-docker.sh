#!/bin/bash

source "ContainerFiles/generate-keys.sh"
source "ContainerFiles/alpine_envs.sh"

source "Utils/image_tag.sh"
REPO="nix_${BASEOS}_waterpaths"
TAG=$(git_image_tag)
export NIX_OMPI_IMAGE="${REPO}:${TAG}"
echo "NIX_OMPI_IMAGE is $NIX_OMPI_IMAGE"
docker build \
       --build-arg ADDUSER="$ADDUSER" \
       --build-arg BASEOS="$BASEOS" \
       -t "$NIX_OMPI_IMAGE" -f Dockerfile .

TEST_IMG="${REPO}_${TAG}_TEST"
docker create --name "$TEST_IMG" "$NIX_OMPI_IMAGE"
docker cp "$TEST_IMG:/tmp/.nix_versions" ContainerFiles/
docker cp "$TEST_IMG:/tmp/env_backup.drv" ContainerFiles/
docker rm -f "$TEST_IMG"
