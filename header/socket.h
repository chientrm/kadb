#ifndef SOCKET_H
#define SOCKET_H

#include <netinet/in.h>
#include <unistd.h>
#include "check.h"

int socket_create(const int port);
void socket_close(const int socket);

#endif