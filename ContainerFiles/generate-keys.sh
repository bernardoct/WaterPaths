#!/bin/bash

cd ContainerFiles

if [ -d "ssh" ]; then
  chmod u+rw -R ssh
  rm -rf ssh
fi

mkdir -p ssh
cd ssh && ssh-keygen -t rsa -f id_rsa.mpi -N '' && cd ..

cat > ssh/config <<EOF
StrictHostKeyChecking no
PasswordAuthentication no
EOF

chmod 700 ssh && chmod 600 ssh/*

cd ..

