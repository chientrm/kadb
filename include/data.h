#pragma once

#include <sys/uio.h>

typedef struct
{
    size_t n_items;
    struct iovec acc_lens;
    struct iovec raw;
} DataGetResult;

const DataGetResult data_get(
    const struct iovec key,
    const size_t offset,
    const size_t n_items);

void data_put(
    const struct iovec key,
    const struct iovec value);

struct iovec data_vec(
    u_int8_t *s);

void data_serialize();