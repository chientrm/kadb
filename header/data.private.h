#pragma once

typedef struct
{
    unsigned long maxCount;
    unsigned long count;
    unsigned long data_size;
    unsigned long *accs;
    char *data;
} Value;

typedef struct node
{
    char *key;
    struct node *left;
    struct node *right;
    unsigned long height;
    Value value;
} Node;

extern Node *root;

unsigned long height(Node *node);
void increase_size_lu(unsigned long *size, unsigned long **ptr);
void increase_size_char(unsigned long *size, char **ptr);
int get_balance(Node *node);
Node *new_node(const char *key, const char *item);
Node *right_rotate(Node *y);
Node *left_rotate(Node *x);
