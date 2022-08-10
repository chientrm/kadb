#pragma once

#include <sys/uio.h>

typedef struct
{
    unsigned long count;
    struct iovec acc;
    struct iovec data;
} DataGetResult;

void data_post(const char *key, const char *value);
DataGetResult data_get(const char *key, unsigned long from, unsigned long count);
