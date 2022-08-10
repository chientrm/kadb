#include "data.h"
#include "data.private.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

DataGetResult data_get(const char *key, unsigned long from, unsigned long count)
{
    DataGetResult result = {
        .count = 0,
        .acc = {.iov_base = NULL, .iov_len = 0},
        .data = {.iov_base = NULL, .iov_len = 0}};
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
    if (node)
    {
        const unsigned long nItems = node->value.nItems;
        from = MIN(from, nItems - 1);
        const unsigned long to = MIN(from + count, nItems);
        result.count = to - from;
        result.acc.iov_len = result.count * sizeof(unsigned long);
        result.acc.iov_base = (char *)malloc(result.acc.iov_len);
        memcpy(result.acc.iov_base, node->value.accLengths + from, result.acc.iov_len);
        result.data.iov_len = node->value.accLengths[to - 1] - node->value.accLengths[from];
        const unsigned long offset = from > 0 ? node->value.accLengths[from - 1] : 0;
        result.data.iov_base = (char *)malloc(result.data.iov_len);
        memcpy(result.data.iov_base, node->value.data + offset, result.data.iov_len);
    }

    return result;
}