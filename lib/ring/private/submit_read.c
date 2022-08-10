#include "ring.h"
#include "ring.private.h"
#include "constants.h"
#include <stdlib.h>
#include <string.h>

int submit_read(int socket)
{
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    Event *event = (Event *)malloc(sizeof(Event));
    event->type = EVENT_READ;
    event->data.socket = socket;
    event->data.iov[0].iov_len = BUFFER_SIZE;
    event->data.iov[0].iov_base = calloc(1, BUFFER_SIZE);
    io_uring_prep_readv(sqe, socket, event->data.iov, 1, 0);
    io_uring_sqe_set_data(sqe, event);
    return io_uring_submit(&ring);
}