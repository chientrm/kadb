#include <signal.h>
#include <string.h>
#include <liburing.h>

#include "check.h"
#include "constants.h"
#include "socket.h"
#include "ring.h"

const char *result = "HTTP/1.0 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 2\r\n\r\nOK\r\n";
int result_len;

void handle_request(Event *event)
{
    Event *event_write = (Event *)malloc(sizeof(Event));
    event_write->socket = event->socket;
    char *buffer = (char *)malloc(result_len);
    memcpy(buffer, result, result_len);
    event->iov.iov_base = buffer;
    event->iov.iov_len = result_len;

    ring_submit_write(event_write);
}

void loop(const int socket)
{
    RingCompletion *completion;
    struct sockaddr_in client;
    socklen_t socklen = sizeof(client);

    ring_submit_accept(socket, &client, &socklen);

    while (1)
    {
        check_failed(io_uring_wait_cqe(&ring, &completion), "io_uring_wait_cqe");
        check_failed(completion->res, "failed event");
        Event *event = (Event *)completion->user_data;

        switch (event->type)
        {
        case EVENT_ACCEPT:
            ring_submit_accept(socket, &client, &socklen);
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
    io_uring_queue_exit(&ring);
    exit(0);
}

void main()
{
    signal(SIGINT, sigint);
    result_len = strlen(result);
    const int socket = socket_create(PORT);
    check_failed(io_uring_queue_init(MAX_CONNS, &ring, 0), "init_ring failed");
    printf("Listening on http://localhost:%d\n", PORT);
    loop(socket);
}