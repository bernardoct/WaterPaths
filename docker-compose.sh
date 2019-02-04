#!/bin/bash

#
# Usage: source docker-compose-openmpi.sh <docker-compose argument list>
#
# This is a wrapper for docker-compose that performs
# some variable substitution - just sue as you normally would
# docker-compose, except for -f <file.yml>, which is already included
#
#
# See build-openmpi.sh for a simpler way to build the docker image, without compose
#

# shellcheck source=Base/ContainerFiles/build.sh
source build-docker.sh
docker-compose -f docker-compose.yml "$@"


