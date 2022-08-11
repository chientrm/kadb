#include "ring.h"
#include "ring.private.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void ring_listen(int socket, int max_conns, int (*request_handler)(EventData))
{
    io_uring_queue_init(max_conns, &ring, 0);
    struct io_uring_cqe *cqe;
    submit_accept(socket);
    while (1)
    {
        io_uring_wait_cqe(&ring, &cqe);
        Event *event = (Event *)cqe->user_data;
        switch (event->type)
        {
        case EVENT_ACCEPT:
            submit_accept(socket);
            submit_read(cqe->res);
            break;
        case EVENT_READ:
            if (cqe->res > 0)
            {
                request_handler(event->data);
            }
            for (int i = 0; i < event->data.iov_count; i++)
            {
                free(event->data.iov[i].iov_base);
            }
            break;
        case EVENT_WRITE:
            close(event->data.socket);
            break;
        }
        free(event);
        io_uring_cqe_seen(&ring, cqe);
    }
}