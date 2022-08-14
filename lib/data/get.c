#include "data.h"
#include "data.private.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

const DataGetResult empty_result = {
    .count = 0,
    .found = 0,
    .accs = {.iov_base = NULL, .iov_len = 0},
    .data = {.iov_base = NULL, .iov_len = 0}};

DataGetResult data_get(
    const struct iovec key,
    unsigned long from,
    unsigned long count)
{
    Node *node = root;
    while (node != NULL)
    {
        const int cmp = iovcmp(key, node->key);
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
        const Array array = node->array;
        unsigned long *accs = array.accs.iov_base;
        const unsigned long to = MIN(from + count, array.count);
        from = MIN(from, array.count - 1);
        const unsigned long found = to - from;
        const unsigned long data_offset = from == 0 ? 0 : accs[from - 1];
        const unsigned long data_len = accs[to - 1] - data_offset;
        DataGetResult result = {
            .count = array.count,
            .found = found,
            .accs = {
                .iov_base = &accs[from],
                .iov_len = found * sizeof(unsigned long)},
            .data = {.iov_base = array.data.iov_base + data_offset, .iov_len = data_len}};
        return result;
    }
    return empty_result;
}