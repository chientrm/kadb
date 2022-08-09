#include "data.h"
#include "data.private.h"
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

Node *insert(Node *node, const char *key, const char *value)
{
    if (node == NULL)
    {
        return new_node(key, value);
    }
    const int cmp = strcmp(key, node->key);
    if (cmp == 0)
    {
        while (node->value.nItems == node->value.maxItems)
        {
            const int newMaxCount = node->value.maxItems * 2 + 1;
            node->value.maxItems = newMaxCount;
            node->value.accLengths = (unsigned long *)realloc(node->value.accLengths, newMaxCount * sizeof(unsigned long));
        }
        node->value.nItems++;
        unsigned long accLength = strlen(value);
        if (node->value.nItems > 1)
        {
            accLength += node->value.accLengths[node->value.nItems - 2];
        }
        node->value.accLengths[node->value.nItems - 1] = accLength;
        while (accLength > node->value.maxLength)
        {
            const int newAccLength = accLength * 2 + 1;
            node->value.data = (char *)realloc(node->value.data, newAccLength);
            node->value.maxLength = newAccLength;
        }
        memcpy(node->value.data + accLength, value, strlen(value));
        return node;
    }
    if (cmp < 0)
    {
        node->left = insert(node->left, key, value);
    }
    else
    {
        node->right = insert(node->right, key, value);
    }
    node->height = 1 + MAX(node->left->height, node->right->height);
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

void data_insert(const char *key, const char *value)
{
    root = insert(root, key, value);
}