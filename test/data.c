#include "data.h"

int main()
{
#define COUNT 15
    u_int8_t *x[COUNT][2] = {
        {"a", "1"},
        {"b", "12"},
        {"cd", "345"},
        {"efg", "dummy"},
        {"hijk", "dummy"},
        {"cd", "6789"},
        {"cd", "dummy"},
        {"m", "dummy"},
        {"n", "dummy"},
        {"o", "dummy"},
        {"p", "dummy"},
        {"q", "dummy"},
        {"r", "dummy"},
        {"s", "dummy"},
        {"t", "dummy"},
    };
    for (int i = 0; i < COUNT; i++)
    {
        data_put(data_vec(x[i][0]), data_vec(x[i][1]));
    }

    data_serialize();
}