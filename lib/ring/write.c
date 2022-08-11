#include "ring.h"
#include "ring.private.h"
#include <stdlib.h>

int ring_write(EventData data)
{
    Event *event = (Event *)malloc(sizeof(Event));
    event->type = EVENT_WRITE;
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    io_uring_prep_writev(sqe, data.socket, data.iov, data.iov_count, 0);
    io_uring_sqe_set_data(sqe, event);
    return io_uring_submit(&ring);
}
