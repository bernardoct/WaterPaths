#!/bin/sh

# BASEIMG="nvidia/cuda:8.0-cudnn7-runtime-ubuntu16.04"
export BASEIMG="ubuntu:18.04"
export BASEOS="ubuntu"
export ADDUSER="adduser --disabled-password --gecos \"\""
export DISTRO_INSTALL_CMDS="ubuntu_install_cmds.sh"
