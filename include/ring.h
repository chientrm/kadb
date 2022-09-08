#pragma once

#include <sys/uio.h>

#include "types.h"

void ring_listen(
    int socket,
    const unsigned int max_conns,
    const int (*request_handler)(int, struct iovec));

int ring_read(
    int socket,
    size_t len,
    struct iovec data);

void ring_stop();

int ring_write_empty(int socket);

int ring_write_data(
    int socket,
    DataGetResult result);