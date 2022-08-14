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
#define NUM_LEN 4

typedef struct
{
    char k_len[NUM_LEN];
    char v_len[NUM_LEN];
} PutMeta;
unsigned long put_meta = sizeof(PutMeta);

typedef struct
{
    char k_len[NUM_LEN];
    char from[NUM_LEN];
    char count[NUM_LEN];
} GetMeta;
unsigned long get_meta = sizeof(GetMeta);

unsigned long strntoul(const char s[NUM_LEN])
{
    unsigned long result = 0;
    for (unsigned long i = 0; i < NUM_LEN; i++)
    {
        result = result * 10 + (s[i] - '0');
    }
    return result;
}

int is_put(const void *a)
{
    return memcmp(a, "PUT /", ACTION_LEN) == 0;
}

int is_get(const void *a)
{
    return memcmp(a, "GET /", ACTION_LEN) == 0;
}

int handle_request(int socket, struct iovec req)
{
    const unsigned long len = req.iov_len;
    void *base = req.iov_base;
    if (len == 0)
    {
        return ring_read(socket, ACTION_LEN, req);
    }
    // PUT /<key_len><value_len><key><value>
    if (len == ACTION_LEN && is_put(base))
    {
        return ring_read(socket, put_meta, req);
    }
    if (len >= ACTION_LEN + put_meta && is_put(base))
    {
        const PutMeta *meta = (PutMeta *)(base + ACTION_LEN);
        const unsigned long
            k_len = strntoul(meta->k_len),
            v_len = strntoul(meta->v_len),
            data_len = k_len + v_len;
        if (len == ACTION_LEN + put_meta)
        {
            return ring_read(socket, data_len, req);
        }
        if (len == ACTION_LEN + put_meta + data_len)
        {
            const struct iovec
                key = {.iov_len = k_len, .iov_base = base + put_meta},
                value = {.iov_len = v_len, .iov_base = base + put_meta + k_len};
            data_put(key, value);
            free(base);
            return ring_write_empty(socket);
        }
    }
    // GET /<key_len><from><count><key>
    if (len == ACTION_LEN && is_get(base))
    {
        return ring_read(socket, get_meta, req);
    }
    if (len >= ACTION_LEN + get_meta && is_get(base))
    {
        const GetMeta *meta = (GetMeta *)(base + ACTION_LEN);
        const unsigned long
            key_len = strntoul(meta->k_len),
            from = strntoul(meta->from),
            count = strntoul(meta->count);
        if (len == ACTION_LEN + get_meta)
        {
            return ring_read(socket, key_len, req);
        }
        if (len == ACTION_LEN + get_meta + key_len)
        {
            const struct iovec key = {.iov_len = key_len, .iov_base = base + get_meta};
            const DataGetResult r = data_get(key, from, count);
            free(base);
            return ring_write_data(socket, r.count, r.found, r.accs, r.data);
        }
    }
    return -1;
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