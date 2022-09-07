#!/bin/bash

apt-get update && apt-get upgrade -y

apt-get install -y \
  liburing-dev\
  build-essential\
  python3\
  python3-pip
pip3 install watchdog

apt-get autoremove -f -y
