#include "ring.h"
#include "ring.private.h"

#include <stdlib.h>

char EMPTY_MESSAGE[] = "HTTP/1.0 204 OK\r\n\r\n";

struct iovec EMPTY = {
    .iov_base = EMPTY_MESSAGE,
    .iov_len = sizeof(EMPTY_MESSAGE)};

int ring_write_empty(int socket)
{
    EventWriteEmpty *write = (EventWriteEmpty *)malloc(sizeof(EventWriteEmpty));
    write->type = EVENT_WRITE_EMPTY;
    write->socket = socket;

    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    io_uring_prep_write(sqe, socket, EMPTY.iov_base, EMPTY.iov_len, 0);
    io_uring_sqe_set_data(sqe, write);
    return io_uring_submit(&ring);
}