#include "ring.h"
#include "ring.private.h"
#include <stdlib.h>

int ring_response(EventData data)
{
    Event *event = (Event *)malloc(sizeof(Event));
    event->type = EVENT_WRITE;
    event->data = data;

    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    io_uring_prep_writev(sqe, event->data.socket, event->data.iov, event->data.iov_count, 0);
    io_uring_sqe_set_data(sqe, event);
    return io_uring_submit(&ring);
}
