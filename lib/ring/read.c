#include "ring.h"
#include "ring.private.h"

#include <stdlib.h>
#include <string.h>

int ring_read(int socket, unsigned long len, struct iovec data)
{
    const unsigned long old_len = data.iov_len, new_len = old_len + len;
    const EventRead read = {
        .type = EVENT_READ,
        .socket = socket,
        .len = len,
        .data = {
            .iov_len = new_len,
            .iov_base = realloc(data.iov_base, new_len)}};
    EventRead *event = (EventRead *)malloc(sizeof(EventRead));
    memcpy(event, &read, sizeof(EventRead));

    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    io_uring_prep_read(sqe, socket, event->data.iov_base + old_len, len, 0);
    io_uring_sqe_set_data(sqe, event);
    return io_uring_submit(&ring);
}
