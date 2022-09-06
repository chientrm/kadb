#!/bin/bash

apt-get update && apt-get upgrade -y

apt-get install -y \
  nodejs\
  npm\
  liburing-dev\
  build-essential\
  python3\

apt-get autoremove -f -y