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
  python3\
  openssh-server
RUN echo "PubkeyAuthentication yes" >> /etc/ssh/sshd_config
RUN service ssh start
CMD ["/usr/sbin/sshd","-D"]
