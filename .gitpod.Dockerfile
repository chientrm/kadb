FROM debian:sid

RUN apt-get update && apt-get install -y\
  liburing-dev
