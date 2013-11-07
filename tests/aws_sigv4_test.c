#include "minunit.h"
#include "aws_status.h"
#include "aws_sigv4.h"

static aws_t context = NULL;

char *test_create()
{
    context = aws_init("us-east-1", "glacier", "glacier.aws.amazon.com", "/", "GET");
    mu_assert(context != NULL, "aws_init failed");

    return NULL;
}

char *test_add_bad_headers()
{
    int status;
    status = aws_add_header(context, "", "hello");
    mu_assert(status == AWS_ERR, "Adding header with empty key should not be accepted");

    status = aws_add_header(context, "hello", "");
    mu_assert(status == AWS_ERR, "Adding header with empty value should not be accepted");

    status = aws_add_header(context, "", "");
    mu_assert(status == AWS_ERR, "Adding header with empty key and value should not be accepted");

    return NULL;
}

char *test_add_bad_params()
{
    int status;
    status = aws_add_param(context, "", "hello");
    mu_assert(status == AWS_ERR, "Adding param with empty key should not be accepted");

    status = aws_add_param(context, "hello", "");
    mu_assert(status == AWS_ERR, "Adding param with empty value should not be accepted");

    status = aws_add_param(context, "", "");
    mu_assert(status == AWS_ERR, "Adding param with empty key and value should not be accepted");

    return NULL;
}

char *test_add_params()
{
    aws_add_param(context, "key4", "val4");
    aws_add_param(context, "key3", "val3");
    aws_add_param(context, "Key2", "val2");
    aws_add_param(context, "key1", "val1");

    /* TODO */

    return NULL;
}

char *test_add_headers()
{
    aws_add_header(context, "key4", "val4");
    aws_add_header(context, "key3", "val3");
    aws_add_header(context, "Key2", "val2");
    aws_add_header(context, "key1", "val1");

    /* TODO */

    return NULL;
}

char *test_sign()
{
    char date[17] = "20131106T090900Z";

    aws_sign(context, date);

    return NULL;
}

char *test_cleanup()
{
    aws_cleanup(context);

    return NULL;
}

char *all_tests() {
    mu_suite_start();

    mu_run_test(test_create);
    mu_run_test(test_add_bad_headers);
    mu_run_test(test_add_headers);
    mu_run_test(test_add_bad_params);
    mu_run_test(test_add_params);
    mu_run_test(test_sign);
    mu_run_test(test_cleanup);

    return NULL;
}

RUN_TESTS(all_tests);
