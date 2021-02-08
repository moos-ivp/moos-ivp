#!/bin/bash
set -ex

if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
  source /etc/lsb-release
  if [[ "$DISTRIB_CODENAME" == "precise" ]]; then
    # Precise ships with CMake 2.8.7, which is not new enough.
    # We install 2.8.12, which is the minimum required by moos.
    sudo sh -c "echo \"foreign-architecture i386\" > /etc/dpkg/dpkg.cfg.d/multiarch"
    sudo apt-get update
    sudo apt-get install libc6:i386 libncurses5:i386 libstdc++6:i386 -y
    sudo apt-get remove cmake -y
    wget --no-check-certificate http://cmake.org/files/v2.8/cmake-2.8.12.2-Linux-i386.tar.gz
    sudo tar xzf cmake-2.8.12.2-Linux-i386.tar.gz --strip 1 -C /usr/local
  fi
fi
