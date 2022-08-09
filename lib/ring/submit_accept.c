#include "ring.h"
#include "ring.private.h"
#include <stdlib.h>

struct sockaddr_in client;
socklen_t socklen = sizeof(client);

int ring_submit_accept(int socket)
{
    RingSubmission *submission = io_uring_get_sqe(&ring);
    io_uring_prep_accept(submission, socket, (struct sockaddr *)&client, &socklen, 0);
    Event *event = (Event *)malloc(sizeof(Event));
    event->type = EVENT_ACCEPT;
    io_uring_sqe_set_data(submission, event);
    return io_uring_submit(&ring);
}