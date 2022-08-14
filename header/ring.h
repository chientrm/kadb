#pragma once

#include <sys/uio.h>

void ring_listen(int socket, int max_conns, int (*handle_request)(int, struct iovec));
void ring_stop();
int ring_read(int socket, unsigned long len, struct iovec data);
int ring_write_empty(int socket);
int ring_write_data(
    int socket,
    unsigned long count,
    unsigned long found,
    struct iovec accs,
    struct iovec data);
