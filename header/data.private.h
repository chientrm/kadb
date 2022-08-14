#pragma once

#include <sys/uio.h>

typedef struct
{
    unsigned long count;
    struct iovec accs;
    struct iovec data;
} Array;

typedef struct node
{
    struct iovec key;
    struct node *left;
    struct node *right;
    unsigned long height;
    Array array;
} Node;

extern Node *root;

unsigned long height(Node *node);
void resize_iov(struct iovec data);
int get_balance(Node *node);
Node *new_node(struct iovec key, struct iovec value);
Node *right_rotate(Node *y);
Node *left_rotate(Node *x);
int iovcmp(struct iovec a, struct iovec b);
