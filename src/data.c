#include "data.h"
#include "data.private.h"

#include "utils.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef struct
{
    size_t max_items;
    size_t n_items;
    size_t *acc_lens;
    struct iovec raw;
} Array;

typedef struct node
{
    struct iovec key;
    struct node *left;
    struct node *right;
    uint8_t height;
    Array array;
} Node;

Node *root = NULL;

const uint8_t _height(Node *node)
{
    if (node)
    {
        return node->height + 1;
    }
    return 0;
}

const uint8_t height(Node *node)
{
    if (node)
    {
        return MAX(_height(node->left), _height(node->right));
    }
    return 0;
}

const int8_t get_balance(Node *node)
{
    if (node)
    {
        return _height(node->left) - _height(node->right);
    }
}

Node *left_rotate(Node *node)
{
    Node *right = node->right;
    node->right = right->left;
    right->left = node;

    node->height = height(node);
    right->height = height(right);
    return right;
}

Node *right_rotate(Node *node)
{
    Node *left = node->left;
    node->left = left->right;
    left->right = node;

    node->height = height(node);
    left->height = height(left);
    return left;
}

Node *new_node(
    const struct iovec key,
    const struct iovec value)
{
    Node *node = malloc(sizeof(Node));
    *node = (Node){
        .key = {
            .iov_len = key.iov_len,
            .iov_base = malloc(key.iov_len)},
        .left = NULL,
        .right = NULL,
        .height = 0,
        .array = {
            .max_items = 1,
            .n_items = 1,
            .acc_lens = malloc(sizeof(size_t)),
            .raw = {
                .iov_len = value.iov_len + 1,
                .iov_base = malloc(value.iov_len + 1),
            },
        }};
    memcpy(node->key.iov_base, key.iov_base, key.iov_len);
    ((uint8_t *)node->array.raw.iov_base)[0] = '+';
    memcpy(node->array.raw.iov_base + 1, value.iov_base, value.iov_len);
    node->array.acc_lens[0] = value.iov_len + 1;
    return node;
}

Node *put(
    Node *node,
    const struct iovec key,
    const struct iovec value)
{
    if (!node)
    {
        return new_node(key, value);
    }
    const int cmp = iovcmp(key, node->key);
    if (cmp == 0)
    {
        Array *array = &node->array;
        array->n_items++;
        while (array->max_items < array->n_items)
        {
            array->max_items *= 2;
            array->acc_lens = realloc(
                array->acc_lens,
                array->max_items * sizeof(size_t));
        }
        const size_t
            previous_acc_len =
                array->n_items > 1
                    ? array->acc_lens[array->n_items - 2]
                    : 0,
            acc_len = previous_acc_len + value.iov_len + 1;
        while (array->raw.iov_len < acc_len)
        {
            array->raw.iov_len *= 2;
            array->raw.iov_base = realloc(
                array->raw.iov_base,
                array->raw.iov_len * sizeof(uint8_t));
        }
        array->acc_lens[array->n_items - 1] = acc_len;
        ((uint8_t *)node->array.raw.iov_base)[previous_acc_len] = '+';
        memcpy(
            array->raw.iov_base + previous_acc_len + 1,
            value.iov_base,
            value.iov_len);
    }
    else if (cmp > 0)
    {
        node->right = put(node->right, key, value);
        if (get_balance(node) == -2)
        {
            if (iovcmp(key, node->right->key) < 0)
            {
                node->right = right_rotate(node->right);
            }
            node = left_rotate(node);
        }
    }
    else
    {
        node->left = put(node->left, key, value);
        if (get_balance(node) == 2)
        {
            if (iovcmp(key, node->left->key) > 0)
            {
                node->left = left_rotate(node->left);
            }
            node = right_rotate(node);
        }
    }
    node->height = height(node);
    return node;
}

void data_put(
    const struct iovec key,
    const struct iovec value)
{
    root = put(root, key, value);
}

const DataGetResult empty_result = {
    .n_items = 0,
    .raw = {.iov_base = NULL, .iov_len = 0}};

const DataGetResult data_get(
    const struct iovec key,
    const size_t offset,
    const size_t n_items)
{
    Node *node = root;
    while (node)
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
        if (offset >= node->array.n_items || n_items <= 0)
        {
            return empty_result;
        }
        const size_t
            to = MIN(offset + n_items, node->array.n_items),
            raw_offset =
                offset == 0
                    ? 0
                    : node->array.acc_lens[offset - 1],
            raw_len = node->array.acc_lens[to - 1] - raw_offset;
        return (DataGetResult){
            .n_items = node->array.n_items,
            .raw = {
                .iov_len = raw_len,
                .iov_base = node->array.raw.iov_base + raw_offset}};
    }
    return empty_result;
}

void write_arrow(
    int fd,
    const struct iovec a,
    const struct iovec b)
{
    const struct iovec data[4] = {
        a,
        data_vec(" -> "),
        b,
        data_vec("\n")};
    writev(fd, data, 4);
}

#define SIZE_T_HEX sizeof(size_t) * 2

const struct iovec serialize_size_t_array(
    uint8_t *s,
    size_t n_items,
    size_t *array)
{
    for (size_t i = 0; i < n_items; i++)
    {
        sprintf(s + i * SIZE_T_HEX, "%0*zx", (int)SIZE_T_HEX, array[i]);
    }
    return (const struct iovec){
        .iov_len = n_items * SIZE_T_HEX,
        .iov_base = s};
}

const struct iovec serialize_size_t(
    uint8_t *s,
    size_t value)
{
    return serialize_size_t_array(s, 1, &value);
}

void serialize(
    int fd,
    Node *node)
{
    uint8_t max_items[SIZE_T_HEX], n_items[SIZE_T_HEX], height[SIZE_T_HEX];
    uint8_t *acc_lens = alloca(SIZE_T_HEX * node->array.n_items);
    const struct iovec data[14] = {
        node->key,
        data_vec("[shape=record,label=\"{key="),
        node->key,
        data_vec("|max_items="),
        serialize_size_t(max_items, node->array.max_items),
        data_vec("|n_items="),
        serialize_size_t(n_items, node->array.n_items),
        data_vec("|height="),
        serialize_size_t(height, node->height),
        data_vec("|acc_lens="),
        serialize_size_t_array(acc_lens, node->array.n_items, node->array.acc_lens),
        data_vec("|raw="),
        {
            .iov_len = node->array.acc_lens[node->array.n_items - 1],
            .iov_base = node->array.raw.iov_base,
        },
        data_vec("}\"]\n")};
    writev(fd, data, 14);
    if (node->left)
    {
        serialize(fd, node->left);
        write_arrow(fd, node->key, node->left->key);
    }
    if (node->right)
    {
        serialize(fd, node->right);
        write_arrow(fd, node->key, node->right->key);
    }
}

void data_serialize()
{
    const uint8_t *filename = "data.dot";
    FILE *f = fopen(filename, "w");
    fclose(f);
    int fd = open(filename, O_WRONLY);
    const struct iovec
        begin = {.iov_len = 10, .iov_base = "digraph {\n"},
        end = {.iov_len = 1, .iov_base = "}"};
    writev(fd, &begin, 1);
    if (root)
    {
        serialize(fd, root);
    }
    writev(fd, &end, 1);
    close(fd);
}