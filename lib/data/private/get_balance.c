#include "data.private.h"
#include <stddef.h>

int get_balance(Node *node)
{
    if (node == NULL)
    {
        return 0;
    }
    return height(node->left) - height(node->right);
}