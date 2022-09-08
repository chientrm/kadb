#pragma once

#include "types.h"

const DataGetResult data_get(
    const struct iovec key,
    const size_t offset,
    const size_t n_items);

void data_put(
    const struct iovec key,
    const struct iovec value);
