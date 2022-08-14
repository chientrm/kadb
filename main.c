#include "socket.h"
#include "ring.h"
#include "data.h"

#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/uio.h>
#include <sys/socket.h>

#define PORT 8080
#define MAX_CONNS 1024
#define MAX_QUEUE MAX_CONNS * 5
#define ACTION_LEN 5

typedef struct
{
    unsigned long key_len;
    unsigned long value_len;
} PutMeta;

typedef struct
{
    unsigned long key_len;
    unsigned long from;
    unsigned long count;
} GetMeta;

int handle_request(int socket, struct iovec req)
{
    if (req.iov_len == 0)
    {
        return ring_read(socket, ACTION_LEN, req);
    }
    if (req.iov_len == ACTION_LEN && memcmp(req.iov_base, "PUT /", ACTION_LEN) == 0)
    {
        PutMeta *meta = (PutMeta *)req.iov_base + ACTION_LEN;
        unsigned long key_len = meta->key_len;
        unsigned long value_len = meta->value_len;
        unsigned long header_len = sizeof(PutMeta);
        if (req.iov_len == ACTION_LEN)
        {
            return ring_read(socket, header_len, req);
        }
        if (req.iov_len == ACTION_LEN + header_len)
        {
            return ring_read(socket, key_len + value_len, req);
        }
        if (req.iov_len == ACTION_LEN + header_len + key_len + value_len)
        {
            struct iovec key = {
                .iov_len = key_len,
                .iov_base = req.iov_base + ACTION_LEN + header_len};
            struct iovec value = {
                .iov_len = value_len,
                .iov_base = req.iov_base + ACTION_LEN + header_len + key_len};
            data_put(key, value);
            free(req.iov_base);
            return ring_write_empty(socket);
        }
    }
    if (req.iov_len == ACTION_LEN && memcmp(req.iov_base, "GET /", ACTION_LEN) == 0)
    {
        GetMeta *meta = (GetMeta *)req.iov_base + ACTION_LEN;
        unsigned long key_len = meta->key_len;
        unsigned long header_len = sizeof(GetMeta);
        if (req.iov_len == ACTION_LEN)
        {
            return ring_read(socket, header_len, req);
        }
        if (req.iov_len == ACTION_LEN + header_len)
        {
            return ring_read(socket, key_len, req);
        }
        if (req.iov_len == ACTION_LEN + header_len + key_len)
        {
            struct iovec key = {
                .iov_len = key_len,
                .iov_base = req.iov_base + ACTION_LEN + header_len};
            DataGetResult result = data_get(key, meta->from, meta->count);
            free(req.iov_base);
            return ring_write_data(socket, result.count, result.found, result.accs, result.data);
        }
    }
    free(req.iov_base);
    shutdown(socket, SHUT_RDWR);
    return 0;
}

void sigint(int signo)
{
    ring_stop();
    exit(0);
}

void main()
{
    signal(SIGINT, sigint);
    const int socket = socket_create(PORT, MAX_CONNS);
    printf("Listening on http://localhost:%d\n", PORT);
    ring_listen(socket, MAX_QUEUE, handle_request);
}