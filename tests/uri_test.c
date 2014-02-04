#include <string.h>
#include "minunit.h"
#include "uri.h"


char *test_empty_string()
{
    char *test_str = "";
    bstring expected = bfromcstr(test_str);
    bstring res;

    res = uri_normalize(test_str);

    mu_assert(biseq(expected, res) == 1, "Empty string should be normalized to empty string");

    return NULL;
}

char *test_url()
{
    char *test_str = "http://google.com";
    bstring expected = bfromcstr(test_str);
    bstring res;

    res = uri_normalize(test_str);

    mu_assert(biseq(expected, res) == 1, "URL should be normalized to URL");

    return NULL;
}

char *test_nonpath()
{
    char *test_str = "nonpath";
    bstring expected = bfromcstr(test_str);
    bstring res;

    res = uri_normalize(test_str);

    mu_assert(biseq(expected, res) == 1, "nonpath should be normalized to nonpath");

    return NULL;
}

char *test_relative_path()
{
    char *test_str = "http://google.com/a/fds/../b/erw/../c/";
    bstring expected = bfromcstr("http://google.com/a/b/c/");
    bstring res;

    res = uri_normalize(test_str);

    mu_assert(biseq(expected, res) == 1, "Relative paths should be removed");

    return NULL;
}

char *test_path()
{
    char *test_str = "/";
    bstring expected = bfromcstr("/");
    bstring res;

    res = uri_normalize(test_str);

    mu_assert(biseq(expected, res) == 1, "Slash should stay the same");

    return NULL;
}

char *test_consecutive_slashes_1()
{
    char *test_str = "//";
    bstring expected = bfromcstr("/");
    bstring res;

    res = uri_normalize(test_str);

    mu_assert(biseq(expected, res) == 1, "Consecutive slashes should be reduced to one (1)");

    return NULL;
}

char *test_consecutive_slashes_2()
{
    char *test_str = "/a//";
    bstring expected = bfromcstr("/a/");
    bstring res;

    res = uri_normalize(test_str);

    mu_assert(biseq(expected, res) == 1, "Consecutive slashes should be reduced to one (2)");

    return NULL;
}

char *test_consecutive_slashes_3()
{
    char *test_str = "/a////";
    bstring expected = bfromcstr("/a/");
    bstring res;

    res = uri_normalize(test_str);

    mu_assert(biseq(expected, res) == 1, "Consecutive slashes should be reduced to one (3)");

    return NULL;
}

char *test_consecutive_slashes_4()
{
    char *test_str = "////a";
    bstring expected = bfromcstr("/a");
    bstring res;

    res = uri_normalize(test_str);

    mu_assert(biseq(expected, res) == 1, "Consecutive slashes should be reduced to one (4)");

    return NULL;
}

char *test_one_slash()
{
    char *test_str = "/";
    bstring expected = bfromcstr(test_str);
    bstring res;

    res = uri_normalize(test_str);

    mu_assert(biseq(expected, res) == 1, "One slash should stay the same");

    return NULL;
}

char *all_tests() {
    mu_suite_start();

    mu_run_test(test_empty_string);
    mu_run_test(test_url);
    mu_run_test(test_nonpath);
    mu_run_test(test_relative_path);
    mu_run_test(test_path);
    mu_run_test(test_consecutive_slashes_1);
    mu_run_test(test_consecutive_slashes_2);
    mu_run_test(test_consecutive_slashes_3);
    mu_run_test(test_consecutive_slashes_4);
    mu_run_test(test_one_slash);

    return NULL;
}

RUN_TESTS(all_tests);
