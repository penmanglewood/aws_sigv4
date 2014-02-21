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
    char *date = "Mon, 09 Sep 2011 23:36:00 GMT";
    char signature[65];
    int status;

    status = aws_sign(context, "wJalrXUtnFEMI/K7MDENG+bPxRfiCYEXAMPLEKEY", date, signature);
    mu_assert(status == AWS_OK, "aws_sign failed");

    return NULL;
}

char *test_example()
{
    char *date = "Mon, 09 Sep 2011 23:36:00 GMT";
    char *secret = "wJalrXUtnFEMI/K7MDENG+bPxRfiCYEXAMPLEKEY";
    char signature[65];

    aws_cleanup(context);
    context = aws_init("us-east-1", "host", "host.foo.com", "/", "GET");

    aws_sign(context, secret, date, signature);

    mu_assert(strcmp(signature, "b27ccfbfa7df52a200ff74193ca6e32d4b48b8856fab7ebf1c595d0670a7e470") == 0, "Signatures don't match");

    return NULL;
}

char *test_add_bad_http_method()
{
    aws_cleanup(context);
    context = aws_init("us-east-1", "host", "host.foo.com", "/", "GAT");

    mu_assert(context == NULL, "Invalid http method should return a null context");

    aws_cleanup(context);
    context = aws_init("us-east-1", "host", "host.foo.com", "/", "");

    mu_assert(context == NULL, "Empty http method should return a null context");

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
    mu_run_test(test_example);
    mu_run_test(test_add_bad_http_method);
    mu_run_test(test_cleanup);

    return NULL;
}

RUN_TESTS(all_tests);
