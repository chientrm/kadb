#include "data.private.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

Node *new_node(const char *key, const char *value)
{
    Node *node = (Node *)malloc(sizeof(Node));
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