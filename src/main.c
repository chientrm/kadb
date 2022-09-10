#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#include "socket.h"
#include "data.h"
#include "ring.h"
#include "utils.h"

#define PORT 8080
#define MAX_CONNS 1024
#define MAX_QUEUE MAX_CONNS * 5
#define NUM_LEN 4

#define ACTION_LEN 3

typedef struct
{
    uint8_t space;
    uint8_t slash_1;
    uint8_t key_len[NUM_LEN];
    uint8_t slash_2;
    uint8_t value_len[NUM_LEN];
    uint8_t slash_3;
} PutMeta;

size_t PUT_META_SIZE = sizeof(PutMeta);

typedef struct
{
    uint8_t space;
    uint8_t slash_1;
    uint8_t key_len[NUM_LEN];
    uint8_t slash_2;
    uint8_t offset[NUM_LEN];
    uint8_t slash_3;
    uint8_t n_items[NUM_LEN];
    uint8_t slash_4;
} GetMeta;

size_t GET_META_SIZE = sizeof(GetMeta);

size_t strntoul(const char s[NUM_LEN])
{
    size_t result = 0;
    for (uint8_t i = 0; i < NUM_LEN; i++)
    {
        result = result * 10 + (s[i] - '0');
    }
    return result;
}

int handle_request(
    int socket,
    struct iovec req)
{ 
    const struct iovec
        EMPTY = data_vec(""),
        PUT = data_vec("PUT"),
        GET = data_vec("GET");
    // EMPTY
    if (iovcmp(req, EMPTY) == 0)
    {
        return ring_read(socket, ACTION_LEN, req);
    }
    // PUT
    if (iovcmp(req, PUT) == 0)
    {
        return ring_read(socket, PUT_META_SIZE, req);
    }
    if (iovncmp(req, PUT, ACTION_LEN) == 0 &&
        req.iov_len == ACTION_LEN + PUT_META_SIZE)
    {
        const PutMeta *meta = req.iov_base + ACTION_LEN;
        return ring_read(
            socket,
            strntoul(meta->key_len) + strntoul(meta->value_len),
            req);
    }
    if (iovncmp(req, PUT, ACTION_LEN) == 0 &&
        req.iov_len > ACTION_LEN + PUT_META_SIZE)
    {
        const PutMeta *meta = req.iov_base + ACTION_LEN;
        data_put(
            (const struct iovec){
                .iov_len = strntoul(meta->key_len),
                .iov_base =
                    req.iov_base +
                    ACTION_LEN +
                    PUT_META_SIZE},
            (const struct iovec){
                .iov_len = strntoul(meta->value_len),
                .iov_base =
                    req.iov_base +
                    ACTION_LEN +
                    PUT_META_SIZE +
                    strntoul(meta->key_len)});
        return ring_write_no_content(socket);
    }
    // GET
    if (iovcmp(req, GET) == 0)
    {
        return ring_read(socket, GET_META_SIZE, req);
    }
    if (iovncmp(req, GET, ACTION_LEN) == 0 &&
        req.iov_len == ACTION_LEN + GET_META_SIZE)
    {
        const GetMeta *meta = req.iov_base + ACTION_LEN;
        return ring_read(socket, strntoul(meta->key_len), req);
    }
    if (iovncmp(req, GET, ACTION_LEN) == 0 &&
        req.iov_len > ACTION_LEN + GET_META_SIZE)
    {
        const GetMeta *meta = req.iov_base + ACTION_LEN;
        const DataGetResult result = data_get(
            (const struct iovec){
                .iov_len = strntoul(meta->key_len),
                .iov_base =
                    req.iov_base +
                    ACTION_LEN +
                    GET_META_SIZE},
            strntoul(meta->offset),
            strntoul(meta->n_items));
        return ring_write_data(socket, result);
    }
    return ring_write_bad_request(socket);
}

void sigint(int signo)
{
    ring_stop();
    exit(0);
}

int main()
{
    signal(SIGINT, sigint);
    int socket = socket_create(PORT, MAX_CONNS);
    printf("Listening on http://localhost:%d\n", PORT);
    ring_listen(socket, MAX_QUEUE, handle_request);
}
