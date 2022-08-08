#ifndef DATA_H
#define DATA_H

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "constants.h"
#include "utils.h"
#include "check.h"

struct Value
{
    unsigned long maxItems;
    unsigned long nItems;
    unsigned long *accLengths;
    unsigned long maxLength;
    char *data;
};

struct Node
{
    char key[KEY_LENGTH];
    struct Node *left;
    struct Node *right;
    unsigned long height;
    struct Value value;
};

void data_insert(const char *key, const char *value);
char *data_get(const char *key, unsigned long from, unsigned long count);

#endif