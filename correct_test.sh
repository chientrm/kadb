#!/bin/bash

killall kadb || true
./kadb &

while ! nc -z localhost 8080; do   
  sleep 0.1
done