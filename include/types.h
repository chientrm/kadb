#pragma once

#include <stddef.h>
#include <sys/uio.h>

typedef struct
{
    size_t total_n_items;
    size_t n_items;
    struct iovec acc_lens;
    struct iovec raw;
} DataGetResult;