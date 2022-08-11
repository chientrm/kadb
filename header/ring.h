#pragma once

#include <sys/uio.h>

void ring_listen(int socket, int max_conns, int (*request_handler)(int, struct iovec));
void ring_stop();
int ring_write_invalid(int socket);
int ring_write_empty(int socket);
int ring_write_data(int socket, unsigned long count, struct iovec acc, struct iovec data);
