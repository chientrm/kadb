#include "check.h"

void error(const char *message)
{
    perror(message);
    exit(1);
}

void check_null(const void *ptr, const char *message)
{
    if (ptr == NULL)
        error(message);
}

void check_failed(int result, const char *message)
{
    if (result < 0)
        error(message);
}