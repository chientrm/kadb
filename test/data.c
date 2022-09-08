#include "data.h"
#include "data.private.h"

#include "utils.h"

#include <assert.h>
#include <stddef.h>

void result_cmp(const DataGetResult a, const DataGetResult b)
{
    assert(a.n_items == b.n_items);
    assert(iovcmp(a.acc_lens, b.acc_lens) == 0);
    assert(iovcmp(a.raw, b.raw) == 0);
}

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

    // invalid key
    result_cmp(
        data_get(data_vec("d"), 1, 3),
        (const DataGetResult){
            .n_items = 0,
            .acc_lens = {.iov_len = 0, .iov_base = NULL},
            .raw = {.iov_len = 0, .iov_base = NULL}});

    // get 2 items
    size_t acc_lens_1[2] = {3, 7};
    result_cmp(
        data_get(data_vec("cd"), 0, 2),
        (const DataGetResult){
            .n_items = 2,
            .acc_lens = {.iov_len = 2 * sizeof(size_t), .iov_base = acc_lens_1},
            .raw = data_vec("3456789")});

    // get 2 items with invalid len
    size_t acc_lens_2[2] = {7, 12};
    result_cmp(
        data_get(data_vec("cd"), 1, 3),
        (const DataGetResult){
            .n_items = 2,
            .acc_lens = {.iov_len = 2 * sizeof(size_t), .iov_base = acc_lens_2},
            .raw = data_vec("6789dummy")});
}