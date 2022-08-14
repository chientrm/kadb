#include "ring.h"
#include "ring.private.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void ring_listen(int socket, int max_conns, int (*handle_request)(int, struct iovec))
{
    io_uring_queue_init(max_conns, &ring, 0);
    struct io_uring_cqe *cqe;
    ring_accept(socket);
    while (1)
    {
        io_uring_wait_cqe(&ring, &cqe);
        Event *event = (Event *)cqe->user_data;
        switch (event->type)
        {
        case EVENT_ACCEPT:
        {
            EventAccept *accept = (EventAccept *)event;
            ring_accept(socket);
            if (cqe->res >= 0)
            {
                ring_read(cqe->res, 0, accept->data);
            }
            break;
        }
        case EVENT_READ:
        {
            EventRead *read = (EventRead *)event;
            if (cqe->res == read->len && handle_request(read->socket, read->data) == 1)
            {
                break;
            }
            free(read->data.iov_base);
            shutdown(read->socket, SHUT_RDWR);
            break;
        }
        case EVENT_WRITE_EMPTY:
        {
            EventWriteEmpty *write = (EventWriteEmpty *)event;
            shutdown(write->socket, SHUT_RDWR);
            break;
        }
        case EVENT_WRITE_DATA:
        {
            EventWriteData *write_data = (EventWriteData *)event;
            free(write_data->header.iov_base);
            shutdown(write_data->socket, SHUT_RDWR);
            break;
        }
        }
        free(event);
        io_uring_cqe_seen(&ring, cqe);
    }
}