#pragma once

#include "munit/munit.h"
#include <stdbool.h>
#include <stdlib.h>

// Memory tracking wrapper function
void *boot_malloc(size_t size);
void *boot_calloc(size_t num, size_t size);
void *boot_realloc(void *ptr, size_t size);
void boot_free(void *ptr);

// Redirect the standard malloc and free functions to the bootlib wrapper
#define malloc(s) boot_malloc(s)
#define calloc(n, s) boot_calloc(n, s)
#define realloc(p, s) boot_realloc(p, s)
#define free(p) boot_free(p)

// Macro Munit
#define munit_case(type, name, block) \
  static MunitResult name(const MunitParameter params[], void *user_data_or_fixture) { \
    do block while (0); \
    return MUNIT_OK; \
  }

#define munit_test(name, test_func) \
  { (char *)(name), (test_func), NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }

#define munit_null_test \
  { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }

#define munit_suite(name, tests_arr) \
  (MunitSuite){ (char *)(name), (tests_arr), NULL, 1, MUNIT_SUITE_OPTION_NONE }

#define assert_true(expr) munit_assert_true(expr)
#define assert_false(expr) munit_assert_false(expr)
#define assert_int(a, op, b, msg) munit_assert_int(a, op, b)
#define assert_ptr_equal(a, b) munit_assert_ptr_equal(a, b)
#define assert_ptr_not_equal(a, b) munit_assert_ptr_not_equal(a, b)
#define assert(expr) munit_assert_true(expr)

bool boot_is_freed(void *ptr);
bool boot_all_freed(void);
