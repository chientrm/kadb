#pragma once

#include <sys/uio.h>
#include <string.h>
#include <alloca.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

static const uint8_t DATA_DIR[] = "data/";

static inline const int iovcmp(
    const struct iovec a,
    const struct iovec b)
{
    if (a.iov_len == b.iov_len)
    {
        return memcmp(a.iov_base, b.iov_base, a.iov_len);
    }
    return a.iov_len - b.iov_len;
}

static inline const int iovncmp(
    const struct iovec a,
    const struct iovec b,
    const size_t len)
{
    return memcmp(a.iov_base, b.iov_base, len);
}

static inline const struct iovec data_vec(u_int8_t *s)
{
    return (const struct iovec){.iov_len = strlen(s), .iov_base = s};
}

static inline FILE *open_key_file(
    const struct iovec key)
{
    size_t len = strlen(DATA_DIR);
    char *filename = alloca(len + key.iov_len + 1);
    memcpy(filename, DATA_DIR, len);
    memcpy(filename + len, key.iov_base, key.iov_len);
    filename[len + key.iov_len] = 0;
    FILE *f = fopen(filename, "a");
    return f;
}

static inline const struct iovec read_key(
    const char *key)
{
    char *filename = alloca(strlen(DATA_DIR) + strlen(key) + 1);
    sprintf(filename, "%s%s", DATA_DIR, key);
    FILE *f = fopen(filename, "rb");
    fseek(f, 0, SEEK_END);
    size_t fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    uint8_t *iov_base = malloc(fsize);
    fread(iov_base, fsize, 1, f);
    fclose(f);
    return (const struct iovec){
        .iov_len = fsize,
        .iov_base = iov_base};
}
