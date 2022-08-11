#include "socket.h"
#include "ring.h"
#include "data.h"

#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/uio.h>

#define PORT 8080
#define MAX_CONNS 1024
#define MAX_QUEUE MAX_CONNS * 5

int handle_request(int socket, struct iovec data)
{
    const char *method = strtok(data.iov_base, " ");
    char *uri = strtok(NULL, " ");
    char *key, *from, *count, *value;
    if (strcmp(method, "GET") == 0 &&
        (key = strtok(uri, "/")) &&
        (from = strtok(NULL, "/")) &&
        (count = strtok(NULL, "/")))
    {
        const unsigned long from_ul = strtoul(from, NULL, 10);
        const unsigned long count_ul = strtoul(count, NULL, 10);
        DataGetResult result = data_get(key, from_ul, count_ul);
        return ring_write_data(socket, result.count, result.acc, result.data);
    }
    if (strcmp(method, "POST") == 0 &&
        (key = strtok(uri, "/")) &&
        (value = strtok(NULL, "/")))
    {
        data_post(key, value);
        return ring_write_empty(socket);
    }
    return ring_write_invalid(socket);
}

void sigint(int signo)
{
    ring_stop();
    exit(0);
}

void main()
{
    signal(SIGINT, sigint);
    const int socket = socket_create(PORT, MAX_CONNS);
    printf("Listening on http://localhost:%d\n", PORT);
    ring_listen(socket, MAX_QUEUE, handle_request);
}