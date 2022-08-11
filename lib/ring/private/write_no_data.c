#include "ring.h"
#include "ring.private.h"

#include <stdlib.h>

int write_no_data(int socket, struct iovec *iov)
{
    EventWriteNoData *write = (EventWriteNoData *)malloc(sizeof(EventWriteNoData));
    write->type = EVENT_WRITE_NO_DATA;
    write->socket = socket;
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    io_uring_prep_writev(sqe, socket, iov, 1, 0);
    io_uring_sqe_set_data(sqe, write);
    return io_uring_submit(&ring);
}