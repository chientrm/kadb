#pragma once

#include <netinet/in.h>
#include <liburing.h>

extern struct io_uring ring;

#define EVENT_ACCEPT 0
#define EVENT_READ 1
#define EVENT_WRITE 2

typedef struct io_uring_cqe RingCompletion;
typedef struct io_uring_sqe RingSubmission;

typedef struct
{
    char type;
    int socket;
    struct iovec iov;
} Event;

int ring_init(unsigned int entries, unsigned int flags);
void ring_exit();
int ring_submit_accept(int socket);
int ring_submit_read(int socket);
int ring_submit_write(Event *event);
