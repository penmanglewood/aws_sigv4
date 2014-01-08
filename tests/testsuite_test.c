#include "minunit.h"
#include "aws_status.h"
#include "aws_sigv4.h"
#include <dirent.h>

char *all_tests() {
    DIR *dp;
    struct dirent *ep;
    dp = opendir("tests/aws4_testsuite");
    if (dp != NULL)
    {
        while (ep = readdir(dp))
            puts (ep->d_name);

        (void)closedir(dp);
    }
    else
        perror("couldn't open dir");

    mu_suite_start();

    return NULL;
}

RUN_TESTS(all_tests);
