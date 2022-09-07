#!/bin/bash

apt-get update && apt-get upgrade -y

apt-get install -y \
  liburing-dev\
  build-essential

apt-get autoremove -f -y
