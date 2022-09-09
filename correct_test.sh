#!/bin/bash

killall kadb || true
./kadb &

while ! nc -z localhost 8080; do   
  sleep 0.1
done

STATUS=$(curl -s -o /dev/null -I -w "%{http_code}" http://localhost:8080/)
if [ "$STATUS" != "400" ]; then
  exit 1;
fi
