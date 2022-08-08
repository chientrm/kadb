#include "utils.h"

int starts_with(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? 1 : memcmp(pre, str, lenpre);
}