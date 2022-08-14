#include "data.private.h"

#include <string.h>

int iovcmp(struct iovec a, struct iovec b)
{
    return a.iov_len == b.iov_len
               ? memcmp(a.iov_base, b.iov_base, a.iov_len)
               : a.iov_len - b.iov_len;
}