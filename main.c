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
#define ACTION_LEN 4

typedef struct
{
    unsigned long key_len;
    unsigned long value_len;
} PutRequestHeader;

typedef struct
{
    unsigned long key_len;
    unsigned long from;
    unsigned long count;
} GetRequestHeader;

int handle_request(int socket, struct iovec req)
{
    if (req.iov_len == 0)
    {
        return ring_read(socket, ACTION_LEN, req);
    }
    if (req.iov_len == ACTION_LEN && memcmp(req.iov_base, "/put", ACTION_LEN) == 0)
    {
        PutRequestHeader *header = (PutRequestHeader *)req.iov_base + ACTION_LEN;
        unsigned long key_len = header->key_len;
        unsigned long value_len = header->value_len;
        unsigned long header_len = sizeof(PutRequestHeader);
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
    if (req.iov_len == 4 && memcmp(req.iov_base, "/get", ACTION_LEN) == 0)
    {
        GetRequestHeader *header = (GetRequestHeader *)req.iov_base + ACTION_LEN;
        unsigned long key_len = header->key_len;
        unsigned long header_len = sizeof(GetRequestHeader);
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
            DataGetResult result = data_get(key, header->from, header->count);
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