#include "ring.h"
#include "ring.private.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void ring_listen(int socket, int max_conns, int (*request_handler)(int, struct iovec))
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
        {
            submit_accept(socket);
            submit_read(cqe->res);
            break;
        }
        case EVENT_READ:
        {
            EventRead *read = (EventRead *)event;
            if (cqe->res > 0)
            {
                request_handler(read->socket, read->data);
            }
            free(read->data.iov_base);
            break;
        }
        case EVENT_WRITE_NO_DATA:
        {
            EventWriteNoData *write_no_data = (EventWriteNoData *)event;
            close(write_no_data->socket);
            break;
        }
        case EVENT_WRITE_DATA:
        {
            EventWriteData *write_data = (EventWriteData *)event;
            free(write_data->header.iov_base);
            close(write_data->socket);
            break;
        }
        }
        free(event);
        io_uring_cqe_seen(&ring, cqe);
    }
}