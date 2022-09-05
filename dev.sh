#!/bin/bash

docker build -t dev .
docker run\
  -p 52022:22\
  -v /home/podie/.ssh/authorized_keys:/root/.ssh/authorized_keys
  -v ${PWD}:/workspace
  -w /workspace
  dev
