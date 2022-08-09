#include "socket.h"
#include <unistd.h>

void socket_close(const int socket)
{
    close(socket);
}