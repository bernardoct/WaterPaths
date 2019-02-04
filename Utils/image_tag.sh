#!/bin/bash

git_image_tag()
{
    local commit
    commit=$(git rev-parse --verify HEAD)
    local tag="$commit"
    if [ ! -z "$(git status --porcelain)" ]; then
	tag="${commit}_testing"
    fi
    
    echo "$tag"
}

#
# Uncomment to test from command line:
#
# git_image_tag
