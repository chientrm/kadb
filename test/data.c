#include "data.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>

void result_cmp(const DataGetResult a, const DataGetResult b)
{
    assert(a.n_items == b.n_items);
    assert(iovcmp(a.acc_lens, b.acc_lens) == 0);
    assert(iovcmp(a.raw, b.raw) == 0);
}

struct iovec create_vec(u_int8_t *s)
{
    return (struct iovec){.iov_len = strlen(s), .iov_base = s};
}

int main()
{
    struct iovec
        students = create_vec("students"),
        alice = create_vec("alice"),
        bob = create_vec("bob"),
        alicebob = create_vec("alicebob");
    size_t acc_lens[2] = {5, 8};

    data_put(students, alice);
    result_cmp(
        data_get(students, 0, 1),
        (DataGetResult){
            .n_items = 1,
            .acc_lens = {.iov_len = sizeof(size_t), .iov_base = &acc_lens},
            .raw = alice});

    data_put(students, bob);
    result_cmp(
        data_get(students, 0, 2),
        (DataGetResult){
            .n_items = 2,
            .acc_lens = {.iov_len = sizeof(size_t) * 2, .iov_base = &acc_lens},
            .raw = alicebob});

    result_cmp(
        data_get(students, 1, 1),
        (DataGetResult){
            .n_items = 1,
            .acc_lens = {.iov_len = sizeof(size_t), .iov_base = &acc_lens[1]},
            .raw = bob});

    struct iovec
        teacher = create_vec("teacher"),
        john = create_vec("john");
    size_t teacher_lens[1] = {4};
    data_put(teacher, john);
    result_cmp(
        data_get(teacher, 0, 1),
        (DataGetResult){
            .n_items = 1,
            .acc_lens = {.iov_len = sizeof(size_t), .iov_base = &teacher_lens},
            .raw = john});

    struct iovec dummy = create_vec("dummy");

    data_put(create_vec("subjects"), dummy);
    data_put(create_vec("tables"), dummy);
    data_put(create_vec("chairs"), dummy);
    data_put(create_vec("computers"), dummy);
    data_serialize();
}