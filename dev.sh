#!/bin/bash

sudo rm -rf .ccls-cache

docker build -t dev .

docker run --rm -it\
	-v ${PWD}:/workspace\
       	-w /workspace dev /bin/bash -c "nvim src/main.c"
