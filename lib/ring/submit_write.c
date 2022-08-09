#include "ring.h"
#include "ring.private.h"

int ring_submit_write(Event *event)
{
    event->type = EVENT_WRITE;
    RingSubmission *submission = io_uring_get_sqe(&ring);
    io_uring_prep_writev(submission, event->socket, &event->iov, 1, 0);
    io_uring_sqe_set_data(submission, event);
    return io_uring_submit(&ring);
}