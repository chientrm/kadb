#include <liburing.h>
#include <stdio.h>
#include <stdlib.h>

void sigint(int signo) {
    exit(0);
}

int main() {
  printf("lel");
  return 0;
}
