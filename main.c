#include <liburing.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PORT 8080
#define MAX_CONNS 1024
#define MAX_QUEUE MAX_CONNS * 5

void sigint(int signo)
{
  exit(0);
}

int main()
{
  signal(SIGINT, sigint);
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  return 0;
}
