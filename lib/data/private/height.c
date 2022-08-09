#include "data.private.h"
#include <stddef.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

unsigned long height(Node *node)
{
    if (node == NULL)
    {
        return 0;
    }
    return MAX(height(node->left), height(node->right)) + 1;
}