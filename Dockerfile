FROM debian:sid

RUN apt-get update && apt-get install -y\
  curl\
  wget\
  nodejs\
  npm\
  liburing-dev\
  sudo\
  git\
  build-essential\
  python3
