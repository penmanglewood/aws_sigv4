#include "minunit.h"
#include "aws_headers.h"
#include "bstrlib.h"

static aws_headers_t headers = NULL;

char *test_create()
{
    headers = aws_headers_init();
    mu_assert(headers != NULL, "aws_headers_init failed");

    return NULL;
}

char *test_add()
{
    aws_headers_add(headers, "key1", "val1");
    aws_headers_add(headers, "key2", "val2");
    aws_headers_add(headers, "key3", "val3");

    mu_assert(headers != NULL, "aws_headers_add failed");

    return NULL;
}

char *test_canonicalize()
{
    bstring c, expected;

    c = aws_headers_canonicalize(headers);
    expected = bfromcstr("key1:val1\nkey2:val2\nkey3:val3");

    mu_assert(biseq(c, expected) == 1, "Wrong canonical string");

    bdestroy(c);
    bdestroy(expected);

    return NULL;
}

char *test_adding_out_of_order()
{
    bstring c, expected;

    aws_headers_destroy(headers);
    headers = aws_headers_init();
    mu_assert(headers != NULL, "aws_headers_init failed");

    aws_headers_add(headers, "key4", "val4");
    aws_headers_add(headers, "key3", "val3");
    aws_headers_add(headers, "key2", "val2");
    aws_headers_add(headers, "key1", "val1");

    c = aws_headers_canonicalize(headers);
    expected = bfromcstr("key1:val1\nkey2:val2\nkey3:val3\nkey4:val4");

    mu_assert(biseq(c, expected) == 1, "Canonical string out of order");

    bdestroy(c);
    bdestroy(expected);

    return NULL;
}

char *test_destroy()
{
    aws_headers_destroy(headers);

    return NULL;
}

char *all_tests() {
    mu_suite_start();

    mu_run_test(test_create);
    mu_run_test(test_add);
    mu_run_test(test_canonicalize);
    mu_run_test(test_adding_out_of_order);
    mu_run_test(test_destroy);

    return NULL;
}

RUN_TESTS(all_tests);
