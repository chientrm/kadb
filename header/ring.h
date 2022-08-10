#pragma once

#include <sys/uio.h>

typedef struct
{
    int socket;
    char iov_count;
    struct iovec iov[3];
} EventData;

void ring_listen(int socket, int max_conns, int (*request_handler)(EventData));
void ring_stop();
int ring_response(EventData data);
