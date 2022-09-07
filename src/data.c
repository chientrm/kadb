#include "data.h"

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
    size_t len;
    size_t *acc_lens;
    uint8_t *raw;
} Array;

typedef struct node
{
    struct iovec key;
    struct node *left;
    struct node *right;
    size_t height;
    Array array;
} Node;

Node *root;

const size_t height(Node *node)
{
    if (node == NULL)
    {
        return 0;
    }
    return MAX(height(node->left), height(node->right)) + 1;
}

const int get_balance(Node *node)
{
    if (node == NULL)
    {
        return 0;
    }
    return height(node->left) - height(node->right);
}

Node *left_rotate(Node *x)
{
    Node *y = x->right;
    Node *T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = height(x);
    y->height = height(y);

    return y;
}

Node *right_rotate(Node *y)
{
    Node *x = y->left;
    Node *T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = height(y);
    x->height = height(x);

    return x;
}

Node *new_node(
    const struct iovec key,
    const struct iovec value)
{
    Node *node = malloc(sizeof(Node));
    *node = (Node){
        .key = {
            .iov_len = key.iov_len,
            .iov_base = malloc(key.iov_len),
        },
        .left = NULL,
        .right = NULL,
        .height = 0,
        .array = {
            .max_items = 1,
            .n_items = 1,
            .len = value.iov_len,
            .acc_lens = malloc(sizeof(size_t)),
            .raw = malloc(value.iov_len),
        }};
    memcpy(node->key.iov_base, key.iov_base, key.iov_len);
    memcpy(node->array.raw, value.iov_base, value.iov_len);
    node->array.acc_lens[0] = value.iov_len;
    return node;
}

const DataGetResult empty_result = {
    .n_items = 0,
    .acc_lens = {.iov_base = NULL, .iov_len = 0},
    .raw = {.iov_base = NULL, .iov_len = 0}};

const int iovcmp(
    const struct iovec a,
    const struct iovec b)
{
    if (a.iov_len == b.iov_len)
    {
        return memcmp(a.iov_base, b.iov_base, a.iov_len);
    }
    return a.iov_len - b.iov_len;
}

const DataGetResult data_get(
    const struct iovec key,
    const size_t offset,
    const size_t n_items)
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
        const size_t
            result_offset = MIN(offset, node->array.n_items - 1),
            to = MIN(offset + n_items, node->array.n_items),
            new_n_items = to - result_offset,
            data_offset =
                result_offset == 0
                    ? 0
                    : node->array.acc_lens[result_offset - 1],
            data_len = node->array.acc_lens[to - 1] - data_offset;
        return (DataGetResult){
            .n_items = new_n_items,
            .acc_lens = {
                .iov_base = node->array.acc_lens + offset,
                .iov_len = n_items * sizeof(size_t)},
            .raw = {.iov_base = node->array.raw + data_offset, .iov_len = data_len},
        };
    }
    return empty_result;
}

Node *put(
    Node *node,
    const struct iovec key,
    const struct iovec value)
{
    if (node == NULL)
    {
        return new_node(key, value);
    }
    const int cmp = iovcmp(key, node->key);
    if (cmp == 0)
    {
        Array *array = &node->array;
        array->n_items++;
        while (array->n_items > array->max_items)
        {
            array->max_items *= 2;
            array->acc_lens = realloc(array->acc_lens, array->max_items * sizeof(size_t));
        }
        const size_t
            previous_acc_len =
                array->n_items > 1
                    ? array->acc_lens[array->n_items - 2]
                    : 0,
            acc_len = previous_acc_len + value.iov_len;
        while (acc_len > array->len)
        {
            array->len *= 2;
            array->raw = realloc(array->raw, array->len * sizeof(uint8_t));
        }
        array->acc_lens[array->n_items - 1] = acc_len;
        memcpy(array->raw + previous_acc_len, value.iov_base, value.iov_len);
    }
    else if (cmp < 0)
    {
        node->left = put(node->left, key, value);
    }
    else
    {
        node->right = put(node->right, key, value);
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

void data_put(
    const struct iovec key,
    const struct iovec value)
{
    root = put(root, key, value);
}

void write_arrow(
    int fd,
    const struct iovec a,
    const struct iovec b)
{
    const struct iovec
        arrow = {.iov_len = 4, .iov_base = " -> "},
        new_line = {.iov_len = 1, .iov_base = "\n"},
        data[4] = {a, arrow, b, new_line};
    writev(fd, data, 4);
}

void serialize(
    int fd,
    const Node *node)
{
    if (node->left)
    {
        write_arrow(fd, node->key, node->left->key);
        serialize(fd, node->left);
    }
    if (node->right)
    {
        write_arrow(fd, node->key, node->right->key);
        serialize(fd, node->right);
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