#pragma once

#include <sys/uio.h>

typedef struct
{
    size_t n_items;
    struct iovec acc_lens;
    struct iovec raw;
} DataGetResult;

const int iovcmp(
    const struct iovec a,
    const struct iovec b);

const DataGetResult data_get(
    const struct iovec key,
    const size_t offset,
    const size_t n_items);

void data_put(
    const struct iovec key,
    const struct iovec value);

void data_serialize();