#include "socket.h"
#include "socket.private.h"

#include <unistd.h>
#include <netinet/in.h>

int socket_create(const int port, const int max_conns)
{
    const int sock = socket(AF_INET, SOCK_STREAM, 0);
    int optval = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = {.s_addr = htonl(INADDR_ANY)},
    };
    bind(sock, (const struct sockaddr *)&addr, sizeof(addr));
    listen(sock, max_conns);
    return sock;
}