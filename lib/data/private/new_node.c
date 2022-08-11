#include "data.private.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

Node *new_node(const char *key, const char *item)
{
    const unsigned long key_len = strlen(key);
    const unsigned long item_len = strlen(item);
    Node *result = (Node *)malloc(sizeof(Node));
    result->key = (char *)calloc(1, key_len + 1);
    memcpy(result->key, key, key_len);
    result->left = NULL;
    result->right = NULL;
    result->height = 0;
    result->value.maxCount = 1;
    result->value.count = 1;
    result->value.data_size = item_len;
    result->value.accs = malloc(sizeof(unsigned long));
    result->value.accs[0] = item_len;
    result->value.data = (char *)malloc(item_len);
    memcpy(result->value.data, item, item_len);
    return result;
}