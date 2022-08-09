#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "check.h"
#include "constants.h"
#include "socket.h"
#include "ring.h"

const char *result = "HTTP/1.0 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 2\r\n\r\nOK\r\n";
int result_len;

int handle_request(Event *event)
{
    char *buffer = (char *)malloc(result_len);
    memcpy(buffer, result, result_len);

    Event *write_event = (Event *)malloc(sizeof(Event));
    write_event->socket = event->socket;
    write_event->iov.iov_base = buffer;
    write_event->iov.iov_len = result_len;
    return ring_submit_write(write_event);
}

void loop(const int socket)
{
    RingCompletion *completion;

    ring_submit_accept(socket);

    while (1)
    {
        check_negative(io_uring_wait_cqe(&ring, &completion), "ring_wait");
        check_negative(completion->res, "event failed");
        Event *event = (Event *)completion->user_data;

        switch (event->type)
        {
        case EVENT_ACCEPT:
            ring_submit_accept(socket);
            ring_submit_read(completion->res);
            break;
        case EVENT_READ:
            handle_request(event);
            break;
        case EVENT_WRITE:
            free(event->iov.iov_base);
            socket_close(event->socket);
            break;
        }
        free(event);
        io_uring_cqe_seen(&ring, completion);
    }
}

void sigint(int signo)
{
    printf(" pressed. Shutting down.\n");
    ring_exit();
    exit(0);
}

void main()
{
    signal(SIGINT, sigint);
    result_len = strlen(result);
    const int socket = socket_create(PORT);
    check_negative(ring_init(MAX_CONNS, 0), "init_ring failed");
    printf("Listening on http://localhost:%d\n", PORT);
    loop(socket);
}