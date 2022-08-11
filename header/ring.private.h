#pragma once

#include "ring.h"
#include <liburing.h>

#define EVENT_ACCEPT 0
#define EVENT_READ 1
#define EVENT_WRITE 2

typedef struct
{
    char type;
    EventData data;
} Event;

typedef struct
{
    Event event;
    int socket;
} EventAccept;

typedef struct
{
    Event event;
    EventData data;
} EventRead;

typedef struct
{
    Event event;
} EventWrite;

extern struct io_uring ring;

int submit_accept(int socket);
int submit_read(int socket);