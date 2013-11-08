#include <string.h>
#include "minunit.h"
#include "sha256.h"


char *test_empty_string()
{
    char out[65];
    char correct[65] = "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";

    sha256_hex(out, "");

    mu_assert(strcmp(out, correct) == 0, "Empty string hash failed");

    return NULL;
}

char *all_tests() {
    mu_suite_start();

    mu_run_test(test_empty_string);

    return NULL;
}

RUN_TESTS(all_tests);
