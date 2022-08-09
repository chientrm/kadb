#include "check.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

void check_null(const void *ptr, const char *message)
{
    if (ptr == NULL)
    {
        perror(message);
        exit(1);
    }
}