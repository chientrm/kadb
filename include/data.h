#pragma once

typedef struct
{
    unsigned long n_items;
    struct iovec accumulated_lengths;
    struct iovec raw;
} DataGetResult;