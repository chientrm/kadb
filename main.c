#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "data.h"
#include "check.h"
#include "utils.h"
#include "constants.h"

const char *result = "Hello World!";
int resultlen;

typedef struct
{
    int client;
    pthread_t thread_id;
} ThreadArgs;

void *handle_client(void *vargp)
{
    ThreadArgs *args = (ThreadArgs *)vargp;
    const int client = args->client;
    char buffer[BUFSIZE],
        method[UL],
        uri[UL],
        version[UL],
        key[BUFSIZE],
        from[UL],
        count[UL];
    FILE *stream = fdopen(client, "r+");
    if (stream == NULL)
    {
        close(client);
        return NULL;
    }

    fgets(buffer, BUFSIZE, stream);
    sscanf(buffer, "%s %s %s\n", method, uri, version);

    fgets(buffer, BUFSIZE, stream);
    unsigned long fromUl = 0,
                  countUl = 0;
    while (strcmp(buffer, "\r\n"))
    {
        fgets(buffer, BUFSIZE, stream);
        if (starts_with("Data-Key: ", buffer) == 0)
            sscanf(buffer, "Data-Key: %s\n", key);
        else if (starts_with("Data-From: ", buffer) == 0)
        {
            sscanf(buffer, "Data-From: %s\n", from);
            fromUl = strtoul(from, NULL, 10);
        }
        else if (starts_with("Data-Count: ", buffer) == 0)
        {
            sscanf(buffer, "Data-Count: %s\n", count);
            countUl = strtoul(count, NULL, 10);
        }
    }
    fprintf(stream, "HTTP/1.0 200 OK\n");
    fprintf(stream, "Server: Svelty Data Server\n");
    fprintf(stream, "Content-length: %d\n", resultlen);
    fprintf(stream, "Content-type: text/plain\n");
    fprintf(stream, "\r\n");
    fflush(stream);
    fprintf(stream, "%s", result);
    fclose(stream);
    close(client);
}

int main(int argc, char **agrv)
{
    signal(SIGPIPE, SIG_IGN);
    resultlen = strlen(result);
    const int server = socket(AF_INET, SOCK_STREAM, 0);
    check_failed(server, "server failed");
    const int optval = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
    struct sockaddr_in serveraddr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr = {.s_addr = htonl(INADDR_ANY)},
    };
    check_failed(
        bind(server, (struct sockaddr *)&serveraddr, sizeof(serveraddr)),
        "bind failed");
    check_failed(listen(server, 5), "listen failed");
    struct sockaddr_in clientaddr;
    int clientlen = sizeof(clientaddr);
    char buffer[BUFSIZE],
        method[BUFSIZE],
        uri[BUFSIZE],
        version[BUFSIZE],
        key[BUFSIZE],
        from[BUFSIZE],
        count[BUFSIZE];

    while (1)
    {
        const int client = accept(server, (struct sockaddr *)&clientaddr, &clientlen);
        if (client < 0)
            continue;
        pthread_t thread_id;
        ThreadArgs *args = (ThreadArgs *)malloc(sizeof(ThreadArgs));
        args->client = client;
        pthread_create(&args->thread_id, NULL, handle_client, (void *)args);
    }
    close(server);
    return 0;
}