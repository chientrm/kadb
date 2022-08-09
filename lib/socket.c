#include "socket.h"

int socket_create(const int port)
{
    const int result = socket(AF_INET, SOCK_STREAM, 0);
    check_failed(result, "socket failed");

    int optval = 1;
    check_failed(setsockopt(result, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)), "setsockopt failed");

    struct sockaddr_in serveraddr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = {.s_addr = htonl(INADDR_ANY)},
    };
    check_failed(
        bind(result, (const struct sockaddr *)&serveraddr, sizeof(serveraddr)),
        "bind failed");

    check_failed(listen(result, 10), "listen failed");
    return result;
}

void socket_close(const int socket)
{
    close(socket);
}