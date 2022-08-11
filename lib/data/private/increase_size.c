#include "data.private.h"
#include <stddef.h>
#include <stdlib.h>

void increase_size_lu(unsigned long *size, unsigned long **ptr)
{
    *size *= 2;
    *ptr = realloc(*ptr, *size * sizeof(unsigned long));
}

void increase_size_char(unsigned long *size, char **ptr)
{
    *size *= 2;
    *ptr = realloc(*ptr, *size * sizeof(char));
}