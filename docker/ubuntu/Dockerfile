FROM ubuntu:22.04

RUN apt-get update
RUN apt-get install -y --no-install-recommends \
    make \
    binutils \
    gcc \
    nasm \
    grub-pc \
    lib32gcc-11-dev \
    libc6-dev-i386 \
    genisoimage \
    xorriso \
    mtools \
    qemu-system-x86 \
    qemu-system-gui

# Personal stuff
RUN apt-get install -y --no-install-recommends \
    git \
    wget \
    zsh

# zsh
RUN git config --global http.sslverify false
RUN sh -c "$(wget -O- --no-check-certificate https://github.com/deluan/zsh-in-docker/releases/download/v1.1.2/zsh-in-docker.sh)" -- \
    -t simple

WORKDIR /data