#include "ring.h"
#include "ring.private.h"

#include <stdlib.h>
#include <netinet/in.h>
#include <liburing.h>

struct sockaddr_in client;
socklen_t socklen = sizeof(client);

int submit_accept(int socket)
{
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    io_uring_prep_accept(sqe, socket, (struct sockaddr *)&client, &socklen, 0);
    EventAccept *event = (EventAccept *)malloc(sizeof(EventAccept));
    event->type = EVENT_ACCEPT;
    io_uring_sqe_set_data(sqe, event);
    return io_uring_submit(&ring);
}