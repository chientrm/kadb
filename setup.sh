#!/bin/bash

apt-get update && apt-get upgrade -y

curl -fsSL https://deb.nodesource.com/setup_18.x | bash -

apt-get install -y \
  liburing-dev\
  build-essential\
  nodejs

apt-get autoremove -f -y
