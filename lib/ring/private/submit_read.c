#include "ring.h"
#include "ring.private.h"

#include <stdlib.h>
#include <string.h>

int submit_read(int socket)
{
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    EventRead *event = (EventRead *)malloc(sizeof(EventRead));
    event->type = EVENT_READ;
    event->socket = socket;
    event->data.iov_len = BUFFER_SIZE;
    event->data.iov_base = calloc(1, BUFFER_SIZE + 1);
    io_uring_prep_readv(sqe, socket, &event->data, 1, 0);
    io_uring_sqe_set_data(sqe, event);
    return io_uring_submit(&ring);
}