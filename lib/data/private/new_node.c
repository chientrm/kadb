#include "data.private.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/uio.h>

Node *new_node(struct iovec key, struct iovec value)
{
    Node *node = (Node *)malloc(sizeof(Node));

    node->key.iov_len = key.iov_len;
    node->key.iov_base = malloc(key.iov_len);
    memcpy(node->key.iov_base, key.iov_base, key.iov_len);

    node->left = NULL;
    node->right = NULL;
    node->height = 0;
    node->array.count = 1;

    unsigned long *accs = (unsigned long *)malloc(sizeof(unsigned long));
    accs[0] = value.iov_len;
    node->array.accs.iov_len = sizeof(unsigned long);
    node->array.accs.iov_base = accs;

    void *data = malloc(value.iov_len);
    memcpy(data, value.iov_base, value.iov_len);
    node->array.data.iov_len = value.iov_len;
    node->array.data.iov_base = data;

    return node;
}