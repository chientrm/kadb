#include "ring.h"
#include "ring.private.h"

#include <stdlib.h>

char EMPTY_MESSAGE[] = "HTTP/1.0 204 OK\r\n\r\n";
unsigned long len = sizeof(EMPTY_MESSAGE) - 1;

int ring_write_empty(int socket)
{
    EventWriteEmpty *write = malloc(sizeof(EventWriteEmpty));
    *write = (EventWriteEmpty){.type = EVENT_WRITE_EMPTY, .socket = socket};

    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    io_uring_prep_write(sqe, socket, EMPTY_MESSAGE, len, 0);
    io_uring_sqe_set_data(sqe, write);
    return io_uring_submit(&ring);
}