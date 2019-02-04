#!/bin/sh

echo "nameserver 8.8.8.8" | tee /etc/resolv.conf > /dev/null && \
  apk update && apk add --no-cache wget bzip2 ca-certificates sudo && \
  wget https://raw.githubusercontent.com/git/git/master/contrib/completion/git-completion.bash -P /etc/bash_completion.d/
