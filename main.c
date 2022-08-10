#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/uio.h>

#include "socket.h"
#include "constants.h"
#include "ring.h"
#include "data.h"

int handle_invalid(int socket)
{
    const char *INVALID = "HTTP/1.0 400 Bad Request\r\n\r\n";
    const unsigned long INVALID_LEN = sizeof(INVALID);
    EventData data = {
        .iov[0] = {
            .iov_len = INVALID_LEN,
            .iov_base = (char *)malloc(INVALID_LEN)},
        .iov_count = 1,
        .socket = socket};
    memcpy(data.iov[0].iov_base, INVALID, INVALID_LEN);
    return ring_response(data);
}

int handle_empty(int socket)
{
    const char *EMPTY = "HTTP/1.0 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 0\r\n\r\n";
    const unsigned long EMPTY_LEN = sizeof(EMPTY);
    EventData data = {
        .iov[0] = {
            .iov_len = EMPTY_LEN,
            .iov_base = (char *)malloc(EMPTY_LEN)},
        .iov_count = 1,
        .socket = socket};
    memcpy(data.iov[0].iov_base, EMPTY, EMPTY_LEN);
    return ring_response(data);
}

int handle_result(int socket, DataGetResult result)
{
    const char *SUCCESS_FORMAT = "HTTP/1.0 200 OK\r\nContent-Type: text/plain\r\nContent-Length: %lu\r\nKadb-Count: %lu\r\n";
    EventData data = {
        .socket = socket,
        .iov_count = 3,
        .iov[0] = {
            .iov_len = BUFFER_SIZE,
            .iov_base = (char *)malloc(BUFFER_SIZE)},
        .iov[1] = result.acc,
        .iov[2] = result.data};
    sprintf(data.iov[0].iov_base,
            SUCCESS_FORMAT,
            result.acc.iov_len + result.data.iov_len,
            result.count);
    return ring_response(data);
}

int handle_request(EventData data)
{
    char method[BUFFER_SIZE], uri[BUFFER_SIZE], version[BUFFER_SIZE];
    sscanf(data.iov[0].iov_base, "%s %s %s", method, uri, version);
    char *key, *from, *count, *value;
    if (strcmp(method, "GET") == 0 &&
        (key = strtok(uri, "/")) &&
        (from = strtok(NULL, "/")) &&
        (count = strtok(NULL, "/")))
    {
        const unsigned long from_ul = strtoul(from, NULL, 10);
        const unsigned long count_ul = strtoul(count, NULL, 10);
        DataGetResult result = data_get(key, from_ul, count_ul);
        return handle_result(data.socket, result);
    }
    if (strcmp(method, "POST") == 0 &&
        (key = strtok(uri, "/")) &&
        (value = strtok(NULL, "/")))
    {
        data_post(key, value);
        return handle_empty(data.socket);
    }
    return handle_invalid(data.socket);
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
    ring_listen(socket, MAX_CONNS, handle_request);
}