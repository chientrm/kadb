#!/bin/bash

build_and_run() {
    clear
    make clean
    make
}

build_and_run
while inotifywait -e close_write src include Makefile; do
    build_and_run
done
