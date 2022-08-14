#include "ring.h"
#include "ring.private.h"

#include <stdlib.h>
#include <string.h>

int ring_read(int socket, size_t len, struct iovec data)
{
    EventRead *event = (EventRead *)malloc(sizeof(EventRead));
    event->type = EVENT_READ;
    event->socket = socket;
    event->len = len;
    event->data = data;
    unsigned long old_len = event->data.iov_len;
    event->data.iov_len += len;
    event->data.iov_base = realloc(event->data.iov_base, event->data.iov_len);

    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    io_uring_prep_read(sqe, socket, event->data.iov_base + old_len, len, 0);
    io_uring_sqe_set_data(sqe, event);
    return io_uring_submit(&ring);
}
