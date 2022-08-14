#pragma once

#include <liburing.h>

#define EVENT_ACCEPT 0
#define EVENT_READ 1
#define EVENT_WRITE_EMPTY 2
#define EVENT_WRITE_DATA 3

typedef struct
{
    char type;
} Event;

typedef struct
{
    char type;
    struct iovec data;
} EventAccept;

typedef struct
{
    char type;
    int socket;
    unsigned long len;
    struct iovec data;
} EventRead;

typedef struct
{
    char type;
    int socket;
} EventWriteEmpty;

typedef struct
{
    char type;
    int socket;
    struct iovec header;
    struct iovec accs;
    struct iovec data;
} EventWriteData;

extern struct io_uring ring;

int ring_accept(int socket);
int read_action(int socket, unsigned long len);
int write_no_data(int socket, struct iovec iov);