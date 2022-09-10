#include "ring.h"

#include "types.h"

#include <liburing.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define EVENT_ACCEPT 0
#define EVENT_READ 1
#define EVENT_WRITE_EMPTY 2
#define EVENT_WRITE_DATA 3

typedef struct
{
    uint8_t type;
} Event;

typedef struct
{
    Event parent;
    struct iovec data;
} EventAccept;

typedef struct
{
    EventAccept parent;
    int socket;
} EventReadWrite;

typedef struct
{
    EventReadWrite parent;
    size_t len;
} EventRead;

typedef struct
{
    EventReadWrite parent;
    struct iovec header;
    struct iovec acc_lens;
    struct iovec raw;
} EventWriteData;

struct io_uring ring;

struct sockaddr_in client;
socklen_t client_len = sizeof(client);

int ring_accept(int socket)
{
    EventAccept *event = malloc(sizeof(EventAccept));
    *event = (EventAccept){.parent.type = EVENT_ACCEPT};
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    io_uring_prep_accept(
        sqe,
        socket,
        (struct sockaddr *)&client,
        &client_len,
        0);
    io_uring_sqe_set_data(sqe, event);
    return io_uring_submit(&ring);
}

int ring_read(
    int socket,
    size_t len,
    struct iovec data)
{
    EventRead *event = malloc(sizeof(EventRead));
    *event = (EventRead){
        .parent.parent.parent.type = EVENT_READ,
        .parent.parent.data = {
            .iov_len = data.iov_len + len,
            .iov_base = malloc(data.iov_len + len)},
        .parent.socket = socket,
        .len = len};
    memcpy(
        event->parent.parent.data.iov_base,
        data.iov_base,
        data.iov_len);

    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    io_uring_prep_read(
        sqe,
        socket,
        event->parent.parent.data.iov_base + data.iov_len,
        len,
        0);
    io_uring_sqe_set_data(sqe, event);
    return io_uring_submit(&ring);
}

void ring_listen(
    int socket,
    const unsigned int max_conns,
    const int (*request_handler)(int, struct iovec))
{
    struct io_uring_cqe *cqe;
    io_uring_queue_init(max_conns, &ring, 0);
    ring_accept(socket);
    while (1)
    {
        int ret = io_uring_wait_cqe(&ring, &cqe);
        if (ret < 0)
        {
            continue;
        }
        Event *event = (Event *)cqe->user_data;

        switch (event->type)
        {
        case EVENT_ACCEPT:
        {
            EventAccept *accept = (EventAccept *)event;
            ring_accept(socket);
            ring_read(cqe->res, 0, accept->data);
            break;
        }
        case EVENT_READ:
        {
            EventRead *read = (EventRead *)event;
            request_handler(
                read->parent.socket,
                read->parent.parent.data);
            free(read->parent.parent.data.iov_base);
            break;
        }
        case EVENT_WRITE_EMPTY:
        {
            EventReadWrite *write = (EventReadWrite *)event;
            close(write->socket);
            break;
        }
        case EVENT_WRITE_DATA:
        {
            EventWriteData *write = (EventWriteData *)event;
            free(write->header.iov_base);
            close(write->parent.socket);
            break;
        }
        }
        free(event);
        io_uring_cqe_seen(&ring, cqe);
    }
}

void ring_stop()
{
    io_uring_queue_exit(&ring);
}

uint8_t BAD_REQUEST_MESSAGE_S[] = "HTTP/1.0 400 Bad Request\r\n\r\n";
const struct iovec BAD_REQUEST_MESSAGE = {
    .iov_len = sizeof(BAD_REQUEST_MESSAGE_S) - 1,
    .iov_base = BAD_REQUEST_MESSAGE_S};

int ring_write_bad_request(int socket)
{
    EventReadWrite *write = malloc(sizeof(EventReadWrite));
    *write = (EventReadWrite){
        .parent.parent.type = EVENT_WRITE_EMPTY,
        .socket = socket};
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    io_uring_prep_writev(
        sqe,
        socket,
        &BAD_REQUEST_MESSAGE,
        1,
        0);
    io_uring_sqe_set_data(sqe, write);
    return io_uring_submit(&ring);
}

uint8_t EMPTY_MESSAGE_S[] = "HTTP/1.0 204 OK\r\n\r\n";
const struct iovec EMPTY_MESSAGE = {
    .iov_len = sizeof(EMPTY_MESSAGE_S) - 1,
    .iov_base = EMPTY_MESSAGE_S};

int ring_write_no_content(int socket)
{
    EventReadWrite *write = malloc(sizeof(EventReadWrite));
    *write = (EventReadWrite){
        .parent.parent.type = EVENT_WRITE_EMPTY,
        .socket = socket};
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    io_uring_prep_writev(
        sqe,
        socket,
        &EMPTY_MESSAGE,
        1,
        0);
    io_uring_sqe_set_data(sqe, write);
    return io_uring_submit(&ring);
}

#define PARAMS_LENGTH 64

uint8_t DATA_FMT[] =
    "HTTP/1.0 200 OK\r\n"
    "Content-Type: application/octet-stream\r\n"
    "Content-Length: %zu\r\n"
    "Kadb-total_n_items: %zu\r\n"
    "Kadb-n_items: %zu\r\n\r\n";

int ring_write_data(
    int socket,
    DataGetResult result)
{
    uint8_t *header = malloc(sizeof(DATA_FMT) + PARAMS_LENGTH);
    sprintf(
        header,
        DATA_FMT,
        result.acc_lens.iov_len + result.raw.iov_len,
        result.total_n_items,
        result.n_items);
    EventWriteData *write = malloc(sizeof(EventWriteData));
    *write = (EventWriteData){
        .parent.parent.parent.type = EVENT_WRITE_DATA,
        .parent.socket = socket,
        .header = {
            .iov_len = strlen(header),
            .iov_base = header},
        .acc_lens = result.acc_lens,
        .raw = result.raw};
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    io_uring_prep_writev(
        sqe,
        write->parent.socket,
        &write->header,
        3,
        0);
    io_uring_sqe_set_data(sqe, write);
    return io_uring_submit(&ring);
}