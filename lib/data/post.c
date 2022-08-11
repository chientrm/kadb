#include "data.h"
#include "data.private.h"
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

Node *insert(Node *node, const char *key, const char *item)
{
    if (node == NULL)
    {
        return new_node(key, item);
    }
    const int cmp = strcmp(key, node->key);
    if (cmp == 0)
    {
        Value *value = &node->value;
        value->count++;
        while (value->count > value->maxCount)
        {
            increase_size_lu(&value->maxCount, &value->accs);
        }
        const unsigned long len = strlen(item);
        const unsigned long previous_acc = value->count > 1 ? value->accs[value->count - 2] : 0;
        const unsigned long acc = previous_acc + len;
        while (acc > value->data_size)
        {
            increase_size_char(&value->data_size, &value->data);
        }
        value->accs[value->count - 1] = acc;
        memcpy(value->data + previous_acc, item, len);
    }
    else if (cmp < 0)
    {
        node->left = insert(node->left, key, item);
    }
    else
    {
        node->right = insert(node->right, key, item);
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

void data_post(const char *key, const char *value)
{
    root = insert(root, key, value);
}