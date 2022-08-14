#include "ring.private.h"

#include <stdlib.h>
#include <netinet/in.h>
#include <liburing.h>

struct sockaddr_in client;
socklen_t socklen = sizeof(client);

int ring_accept(int socket)
{
    EventAccept *event = (EventAccept *)malloc(sizeof(EventAccept));
    event->type = EVENT_ACCEPT;

    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    io_uring_prep_accept(sqe, socket, (struct sockaddr *)&client, &socklen, 0);
    io_uring_sqe_set_data(sqe, event);
    return io_uring_submit(&ring);
}