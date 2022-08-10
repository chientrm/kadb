#pragma once

void check_null(const void *ptr, const char *message);
void check_negative(const int result, const char *message);
void check_int_equal(const int result, const int expected, const char *message);