#include "data.private.h"

#include <stddef.h>
#include <stdlib.h>

void resize_iov(struct iovec data)
{
    data.iov_len *= 2;
    data.iov_base = realloc(data.iov_base, data.iov_len);
}
