#pragma once

#include <sys/uio.h>
#include <string.h>

static inline const int iovcmp(
    const struct iovec a,
    const struct iovec b)
{
    if (a.iov_len == b.iov_len)
    {
        return memcmp(a.iov_base, b.iov_base, a.iov_len);
    }
    return a.iov_len - b.iov_len;
}

static inline const int iovncmp(
    const struct iovec a,
    const struct iovec b,
    const size_t len)
{
    return memcmp(a.iov_base, b.iov_base, len);
}

static inline const struct iovec data_vec(u_int8_t *s)
{
    return (const struct iovec){.iov_len = strlen(s), .iov_base = s};
}