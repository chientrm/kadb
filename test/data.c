#include "data.h"

int main()
{
    u_int8_t *x[6][2] = {
        {"a", "1"},
        {"b", "12"},
        {"cd", "345"},
        {"efg", "dummy"},
        {"hijk", "dummy"},
        {"cd", "6789"},
    };
    for (int i = 0; i < 6; i++)
    {
        data_put(data_vec(x[i][0]), data_vec(x[i][1]));
    }

    data_serialize();
}