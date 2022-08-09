#ifndef SUBMISSION_H
#define SUBMISSION_H

#include <netinet/in.h>
#include <liburing.h>

#include "constants.h"

struct io_uring ring;

#define EVENT_ACCEPT 0
#define EVENT_READ 1
#define EVENT_WRITE 2

typedef struct io_uring_sqe RingSubmission;
typedef struct io_uring_cqe RingCompletion;

typedef struct
{
    char type;
    int socket;
    struct iovec iov;
} Event;

int ring_submit_accept(int socket, struct sockaddr_in *client, socklen_t *client_len);
int ring_submit_read(int socket);
int ring_submit_write(Event *event);

#endif