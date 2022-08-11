#include "ring.h"
#include "ring.private.h"

char INVALID_MESSAGE[] = "HTTP/1.0 400 Bad Request\r\n\r\n";

struct iovec INVALID = {
    .iov_base = INVALID_MESSAGE,
    .iov_len = sizeof(INVALID_MESSAGE) - 1};

int ring_write_invalid(int socket)
{
    return write_no_data(socket, &INVALID);
}