#include "ring.h"
#include "ring.private.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const char DATA_MESSAGE_FORMAT[] =
    "HTTP/1.0 200 OK\r\nContent-Type: application/octet-stream\r\nContent-Length: %lu\r\nKadb-Count: %lu\r\n\r\n";

int ring_write_data(int socket, unsigned long count, struct iovec acc, struct iovec data)
{
    EventWriteData *write = (EventWriteData *)malloc(sizeof(EventWriteData));
    write->type = EVENT_WRITE_DATA;
    write->socket = socket;
    write->header.iov_base = (char *)malloc(BUFFER_SIZE);
    sprintf(write->header.iov_base, DATA_MESSAGE_FORMAT, acc.iov_len + data.iov_len, count);
    write->header.iov_len = strlen(write->header.iov_base);
    write->acc = acc;
    write->data = data;
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    io_uring_prep_writev(sqe, write->socket, &write->header, 3, 0);
    io_uring_sqe_set_data(sqe, write);
    return io_uring_submit(&ring);
}