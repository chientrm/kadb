#pragma once

#include <liburing.h>

#define EVENT_ACCEPT 0
#define EVENT_READ 1
#define EVENT_WRITE_NO_DATA 2
#define EVENT_WRITE_DATA 3

#define BUFFER_SIZE 1024

typedef struct
{
    char type;
} Event;

typedef struct
{
    char type;
} EventAccept;

typedef struct
{
    char type;
    int socket;
    struct iovec data;
} EventRead;

typedef struct
{
    char type;
    int socket;
} EventWriteNoData;

typedef struct
{
    char type;
    int socket;
    struct iovec header;
    struct iovec acc;
    struct iovec data;
} EventWriteData;

extern struct io_uring ring;

int submit_accept(int socket);
int submit_read(int socket);
int write_no_data(int socket, struct iovec *iov);