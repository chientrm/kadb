#include "data.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

struct Node *root;

unsigned long height(struct Node *node)
{
    if (node == NULL)
        return 0;
    return MAX(height(node->left), height(node->right)) + 1;
}

int get_balance(struct Node *node)
{
    if (node == NULL)
        return 0;
    return height(node->left) - height(node->right);
}

struct Node *new_node(const char *key, const char *value)
{
    struct Node *node = (struct Node *)malloc(sizeof(struct Node));
    strcpy(node->key, key);
    node->left = NULL;
    node->right = NULL;
    node->height = 0;
    node->value.maxItems = 1;
    node->value.nItems = 1;
    const size_t length = strlen(value);
    node->value.maxLength = length;
    node->value.accLengths = (unsigned long *)malloc(sizeof(unsigned long));
    node->value.accLengths[0] = length;
    node->value.data = (char *)malloc(length);
    strcpy(node->value.data, value);
    return node;
}

struct Node *right_rotate(struct Node *y)
{
    struct Node *x = y->left;
    struct Node *T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = height(y);
    x->height = height(x);

    return x;
}

struct Node *left_rotate(struct Node *x)
{
    struct Node *y = x->right;
    struct Node *T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = height(x);
    y->height = height(y);

    return y;
}

struct Node *insert(struct Node *node, const char *key, const char *value)
{
    if (node == NULL)
        return new_node(key, value);
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
            accLength += node->value.accLengths[node->value.nItems - 2];
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
        node->left = insert(node->left, key, value);
    else
        node->right = insert(node->right, key, value);
    node->height = 1 + MAX(node->left->height, node->right->height);
    const int balance = get_balance(node);
    if (balance > 1 && cmp < 0)
        return right_rotate(node);
    if (balance < -1 && cmp > 0)
        return left_rotate(node);
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

char *data_get(const char *key, unsigned long from, unsigned long count)
{
    struct Node *node = root;
    while (node != NULL)
    {
        const int cmp = strcmp(key, node->key);
        if (cmp == 0)
            break;
        if (cmp < 0)
            node = node->left;
        else
            node = node->right;
    }
    if (node == NULL)
        return NULL;
    const unsigned long index = node->value.accLengths[node->value.nItems - 1] - node->value.accLengths[node->value.nItems - 2];
    if (from >= index)
        return NULL;
    if (from + count > index)
        count = index - from;
    return node->value.data + from;
}