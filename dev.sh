#!/bin/bash

cycle() {
    clear
    make clean
    make
}

cycle
while inotifywait -e close_write src include test Makefile *_test.sh; do
    cycle
done
