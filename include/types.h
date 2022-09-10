#pragma once

#include <stddef.h>
#include <sys/uio.h>

typedef struct
{
    size_t n_items;
    struct iovec raw;
} DataGetResult;