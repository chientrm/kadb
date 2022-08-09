#include "ring.h"
#include "ring.private.h"
#include "constants.h"
#include <stdlib.h>
#include <string.h>

int ring_submit_read(int socket)
{
    RingSubmission *submission = io_uring_get_sqe(&ring);
    Event *event = (Event *)malloc(sizeof(Event));
    event->type = EVENT_READ;
    event->socket = socket;
    event->iov.iov_base = malloc(BUFSIZE);
    event->iov.iov_len = BUFSIZE;
    memset(event->iov.iov_base, 0, BUFSIZE);
    io_uring_prep_readv(submission, socket, &event->iov, 1, 0);
    io_uring_sqe_set_data(submission, event);
    return io_uring_submit(&ring);
}