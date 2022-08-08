#ifndef CHECK_H
#define CHECK_H

#include <stdio.h>
#include <stdlib.h>

void check_null(const void *ptr, const char *message);
void check_failed(const int result, const char *message);

#endif