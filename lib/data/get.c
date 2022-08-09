#include "data.h"
#include "data.private.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

char *data_get(const char *key, unsigned long from, unsigned long count)
{
    Node *node = root;
    while (node != NULL)
    {
        const int cmp = strcmp(key, node->key);
        if (cmp == 0)
        {
            break;
        }
        if (cmp < 0)
        {
            node = node->left;
        }
        else
        {
            node = node->right;
        }
    }
    if (node == NULL)
    {
        return NULL;
    }
    const unsigned long index = node->value.accLengths[node->value.nItems - 1] - node->value.accLengths[node->value.nItems - 2];
    if (from >= index)
    {
        return NULL;
    }
    if (from + count > index)
    {
        count = index - from;
    }
    return node->value.data + from;
}