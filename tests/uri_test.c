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

char *test_relative_path()
{
    char *test_str = "http://google.com/a/fds/../b/erw/../c/";
    bstring expected = bfromcstr("http://google.com/a/b/c/");
    bstring res;

    res = uri_normalize(test_str);

    mu_assert(biseq(expected, res) == 1, "Relative paths should be removed");

    return NULL;
}

char *all_tests() {
    mu_suite_start();

    mu_run_test(test_empty_string);
    mu_run_test(test_url);
    mu_run_test(test_relative_path);

    return NULL;
}

RUN_TESTS(all_tests);
