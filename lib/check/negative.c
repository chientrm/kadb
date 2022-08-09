#include "check.h"
#include <stdio.h>
#include <stdlib.h>

void check_negative(int result, const char *message)
{
    if (result < 0)
    {
        perror(message);
        exit(1);
    }
}