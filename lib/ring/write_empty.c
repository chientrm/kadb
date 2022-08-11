#include "ring.h"
#include "ring.private.h"

char EMPTY_MESSAGE[] = "HTTP/1.0 204 OK\r\n\r\n";

struct iovec EMPTY = {
    .iov_base = EMPTY_MESSAGE,
    .iov_len = sizeof(EMPTY_MESSAGE) - 1};

int ring_write_empty(int socket)
{
    return write_no_data(socket, &EMPTY);
}