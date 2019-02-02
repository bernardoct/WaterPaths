#!/bin/sh

echo "nameserver 8.8.8.8" | tee /etc/resolv.conf > /dev/null && \
  apt update -y && apt install -y --no-install-recommends wget && \
  wget -O spc.deb http://launchpadlibrarian.net/249551255/software-properties-common_0.96.20_all.deb && \
  dpkg -i spc.deb; rm -f spc.deb && apt install -y -f && \
  apt install -y --no-install-recommends bzip2 ca-certificates && \
  apt clean && \
  wget https://raw.githubusercontent.com/git/git/master/contrib/completion/git-completion.bash -P /etc/bash_completion.d/
