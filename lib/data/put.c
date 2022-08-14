#include "data.h"
#include "data.private.h"

#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

Node *insert(Node *node, const struct iovec key, const struct iovec value)
{
    if (node == NULL)
    {
        return new_node(key, value);
    }
    const int cmp = iovcmp(key, node->key);
    if (cmp == 0)
    {
        Array array = node->array;
        array.count++;
        unsigned long *accs = array.accs.iov_base;
        while (array.count > array.accs.iov_len / sizeof(unsigned long))
        {
            resize_iov(array.accs);
        }
        const unsigned long previous_acc = array.count > 1 ? accs[array.count - 2] : 0;
        const unsigned long acc = previous_acc + value.iov_len;
        while (acc > array.data.iov_len)
        {
            resize_iov(array.data);
        }
        accs[array.count - 1] = acc;
        memcpy(array.data.iov_base + previous_acc, value.iov_base, value.iov_len);
    }
    else if (cmp < 0)
    {
        node->left = insert(node->left, key, value);
    }
    else
    {
        node->right = insert(node->right, key, value);
    }

    node->height = 1 + MAX(height(node->left), height(node->right));

    const int balance = get_balance(node);
    if (balance > 1 && cmp < 0)
    {
        return right_rotate(node);
    }
    if (balance < -1 && cmp > 0)
    {
        return left_rotate(node);
    }
    if (balance > 1 && cmp > 0)
    {
        node->left = left_rotate(node->left);
        return right_rotate(node);
    }
    if (balance < -1 && cmp < 0)
    {
        node->right = right_rotate(node->right);
        return left_rotate(node);
    }

    return node;
}

void data_put(const struct iovec key, const struct iovec value)
{
    root = insert(root, key, value);
}