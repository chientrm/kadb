#include "data.h"
#include "data.private.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

const DataGetResult empty_result = {
    .count = 0,
    .acc = {.iov_base = NULL, .iov_len = 0},
    .data = {.iov_base = NULL, .iov_len = 0}};

DataGetResult data_get(const char *key, unsigned long from, unsigned long count)
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
    if (node)
    {
        const Value value = node->value;
        from = MIN(from, value.count - 1);
        const unsigned long to = MIN(from + count, value.count);
        count = to - from;
        const unsigned long data_offset = from == 0 ? 0 : value.accs[from - 1];
        const unsigned long data_len = value.accs[to - 1] - data_offset;
        DataGetResult result = {
            .count = count,
            .acc = {
                .iov_base = value.accs + from,
                .iov_len = count * sizeof(unsigned long)},
            .data = {.iov_base = value.data + data_offset, .iov_len = data_len}};
        return result;
    }
    return empty_result;
}