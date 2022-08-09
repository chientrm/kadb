#pragma once

#include "constants.h"

typedef struct node
{
    char key[KEY_SIZE];
    struct node *left;
    struct node *right;
    unsigned long height;
    struct
    {
        unsigned long maxItems;
        unsigned long nItems;
        unsigned long *accLengths;
        unsigned long maxLength;
        char *data;
    } value;
} Node;

extern Node *root;

unsigned long height(Node *node);
int get_balance(Node *node);
Node *new_node(const char *key, const char *value);
Node *right_rotate(Node *y);
Node *left_rotate(Node *x);
