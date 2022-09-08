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
    size_t *acc_lens;
    struct iovec raw;
} Array;

typedef struct node
{
    struct iovec key;
    struct node *left;
    struct node *right;
    size_t height;
    Array array;
} Node;

Node *root = NULL;

const size_t height(Node *node)
{
    if (node == NULL)
    {
        return 0;
    }
    size_t
        left_height = node->left ? node->left->height + 1 : 0,
        right_height = node->right ? node->right->height + 1 : 0;
    return MAX(left_height, right_height);
}

const int get_balance(Node *node)
{
    if (node == NULL)
    {
        return 0;
    }
    size_t
        left_height = node->left ? node->left->height + 1 : 0,
        right_height = node->right ? node->right->height + 1 : 0;
    return left_height - right_height;
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
            .raw = {.iov_len = value.iov_len, .iov_base = malloc(value.iov_len)},
        }};
    memcpy(node->key.iov_base, key.iov_base, key.iov_len);
    memcpy(node->array.raw.iov_base, value.iov_base, value.iov_len);
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
            .raw = {.iov_base = node->array.raw.iov_base + data_offset, .iov_len = data_len},
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
        while (acc_len > array->raw.iov_len)
        {
            array->raw.iov_len *= 2;
            array->raw.iov_base = realloc(array->raw.iov_base, array->raw.iov_len * sizeof(uint8_t));
        }
        array->acc_lens[array->n_items - 1] = acc_len;
        memcpy(array->raw.iov_base + previous_acc_len, value.iov_base, value.iov_len);
        return node;
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

const struct iovec data_vec(u_int8_t *s)
{
    return (const struct iovec){.iov_len = strlen(s), .iov_base = s};
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