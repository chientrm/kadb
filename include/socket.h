#pragma once

#include <stdint.h>

int socket_create(
    const uint16_t port,
    const int max_conns);