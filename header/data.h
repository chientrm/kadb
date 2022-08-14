#pragma once

#include <sys/uio.h>

typedef struct
{
    unsigned long count;
    unsigned long found;
    struct iovec accs;
    struct iovec data;
} DataGetResult;

void data_put(const struct iovec key, const struct iovec value);
DataGetResult data_get(struct iovec key, unsigned long from, unsigned long count);
