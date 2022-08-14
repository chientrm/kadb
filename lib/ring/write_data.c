#include "ring.h"
#include "ring.private.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define PARAMS_LENGTH 64

char DATA_FMT[] =
    "HTTP/1.0 200 OK\r\n"
    "Content-Type: application/octet-stream\r\n"
    "Content-Length: %lu\r\n"
    "Kadb-Count: %lu\r\n"
    "Kadb-found: %lu\r\n\r\n";

int ring_write_data(
    int socket,
    unsigned long count,
    unsigned long found,
    struct iovec accs,
    struct iovec data)
{
    EventWriteData *write = (EventWriteData *)malloc(sizeof(EventWriteData));
    write->type = EVENT_WRITE_DATA;
    write->socket = socket;

    char *header = (char *)malloc(sizeof(DATA_FMT) * PARAMS_LENGTH);
    sprintf(header, DATA_FMT, accs.iov_len + data.iov_len, count, found);
    write->header.iov_len = strlen(write->header.iov_base);
    write->header.iov_base = header;

    write->accs = accs;
    write->data = data;

    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    io_uring_prep_writev(sqe, write->socket, &write->header, 3, 0);
    io_uring_sqe_set_data(sqe, write);
    return io_uring_submit(&ring);
}