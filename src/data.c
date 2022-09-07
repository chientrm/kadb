#include "data.h"

#include <sys/uio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef struct
{
    uint64_t max_items;
    uint64_t n_items;
    uint64_t array_size;
    uint64_t *accumulated_lengths;
    uint8_t *raw;
} Array;

typedef struct node
{
    struct iovec key;
    struct node *left;
    struct node *right;
    uint64_t height;
    Array array;
} Node;

Node *root;

const uint64_t height(Node *node)
{
    const uint64_t tmp = height(node->left);
    if (node == NULL)
    {
        return 0;
    }
    return MAX(height(node->left), height(node->right)) + 1;
}

const int64_t get_balance(Node *node)
{
    if (node == NULL)
    {
        return 0;
    }
    return height(node->left) - height(node->right);
}

void increase_size_lu(uint64_t *size, uint64_t **ptr)
{
    *size *= 2;
    *ptr = realloc(*ptr, *size * sizeof(unsigned long));
}

void increase_size_char(uint64_t *size, uint8_t **ptr)
{
    *size *= 2;
    *ptr = realloc(*ptr, *size * sizeof(char));
}

const Node *left_rotate(Node *x)
{
    Node *y = x->right;
    Node *T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = height(x);
    y->height = height(y);

    return y;
}

const Node *right_rotate(Node *y)
{
    Node *x = y->left;
    Node *T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = height(y);
    x->height = height(x);

    return x;
}

const Node *new_node(const uint64_t key_len,
                     const uint64_t value_len,
                     const uint8_t *key,
                     const uint8_t *value)
{
    Node *node = malloc(sizeof(Node));
    *node = (Node){
        .key = {
            .iov_len = key_len,
            .iov_base = malloc(key_len),
        },
        .left = NULL,
        .right = NULL,
        .height = 0,
        .array = {
            .max_items = 1,
            .n_items = 1,
            .array_size = value_len,
            .accumulated_lengths = malloc(sizeof(uint64_t)),
            .raw = malloc(value_len),
        }};
    memcpy(node->key.iov_base, key, key_len);
    memcpy(node->array.raw, value, value_len);
    node->array.accumulated_lengths[0] = value_len;
    return node;
}

const DataGetResult empty_result = {
    .n_items = 0,
    .accumulated_lengths = {.iov_base = NULL, .iov_len = 0},
    .raw = {.iov_base = NULL, .iov_len = 0}};

DataGetResult data_get(const struct iovec *key, uint64_t offset, uint64_t n_items)
{
    Node *node = root;
    while (node != NULL)
    {
        const int cmp = memcmp(key,
                               node->key.iov_base,
                               MIN(key->iov_len, node->key.iov_len));
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
        offset = MIN(offset, array.n_items - 1);
        const uint64_t to = MIN(offset + n_items, array.n_items);
        n_items = to - offset;
        const uint64_t data_offset =
            offset == 0
                ? 0
                : array.accumulated_lengths[offset - 1];
        const uint64_t data_len = array.accumulated_lengths[to - 1] - data_offset;
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