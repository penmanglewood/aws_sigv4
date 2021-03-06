/*
 * =====================================================================================
 *
 *       Filename:  aws_sigv4.c
 *
 *    Description:  Library to generate AWS signature version 4
 *
 *        Version:  1.0
 *        Created:  2013/11/01
 *       Compiler:  gcc
 *
 *         Author:  Eric Rubio
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include "aws_sigv4.h"
#include "aws_headers.h"
#include "aws_params.h"
#include "aws_status.h"
#include "bstrlib.h"
#include "sha256.h"
#include "hmac.h"
#include "uri.h"
#include "utils.h"

#define HTTP_METHOD_MAX_LENGTH 6 /* Safety checking the http method user input */
#define HTTP_METHOD_COUNT 4

static char *http_request_methods[4] = {"GET", "POST", "PUT", "DELETE"};

struct request_date {
    bstring original_str;
    bstring str; /* YYYY-MM-DDTHH:MM:SSZ */
    bstring str_date_only; /* YYYY-MM-DD */
    struct tm tm;
};

struct aws_context {
    bstring request_method;
    bstring account_id;
    bstring region;
    bstring service;
    bstring host;
    bstring path;
    struct request_date date;
    bstring canonical_request;
    bstring credential_scope;
    bstring string_to_sign;
    bstring signature;
    aws_headers_t headers;
    aws_params_t params;
    bstring form_data;
};

static bool initialized = false;

/* Private functions */

static int generate_canonical_request(aws_t context);
static int generate_credential_scope(aws_t context);
static int generate_string_to_sign(aws_t context);
static int parse_date(aws_t context, const char *str);
static void parse_path(aws_t context, const char *the_path, int len);
static void parse_query_parameters(aws_t context, const char *the_path, int len);
static bool is_valid_http_method(const char *method);
static bool is_post_request(aws_t context);

aws_t aws_init(const char *region, const char *service, const char *host, const char *path, const char *http_method)
{
    aws_t context = malloc(sizeof(struct aws_context));
    if (context == NULL)
        return NULL;

    context->headers = aws_headers_init();
    if (!context->headers)
        return NULL;

    context->params = aws_params_init();
    if (!context->params)
        return NULL;
    parse_query_parameters(context, path, strlen(path));

    context->region = bfromcstr(region);
    if (!context->region)
        return NULL;

    context->service = bfromcstr(service);
    if (!context->service)
        return NULL;

    context->host = bfromcstr(host);
    if (!context->host)
        return NULL;

    aws_headers_add(context->headers, "host", host);

    parse_path(context, path, strlen(path));
    if (!context->path)
        return NULL;

    if (!is_valid_http_method(http_method))
        return NULL;

    context->request_method = bfromcstr(http_method);
    if (!context->request_method)
        return NULL;

    context->date.original_str = NULL;
    context->date.str = NULL;
    context->date.str_date_only = NULL;

    context->account_id = NULL;
    context->canonical_request = NULL;
    context->credential_scope = NULL;
    context->string_to_sign = NULL;
    context->signature = NULL;

    context->form_data = bfromcstr("");

    initialized = true;

    return context;
}

int aws_cleanup(aws_t context)
{
#ifdef DEBUG
    assert(initialized);
#endif

    if (context == NULL)
        return AWS_OK;

    bdestroy(context->account_id);
    bdestroy(context->region);
    bdestroy(context->service);
    bdestroy(context->host);
    bdestroy(context->path);
    bdestroy(context->request_method);
    bdestroy(context->canonical_request);
    bdestroy(context->credential_scope);
    bdestroy(context->string_to_sign);
    bdestroy(context->signature);
    bdestroy(context->date.original_str);
    bdestroy(context->date.str);
    bdestroy(context->date.str_date_only);
    bdestroy(context->form_data);

    aws_headers_destroy(context->headers);
    aws_params_destroy(context->params);

    context->headers = NULL;
    context->params = NULL;

    free(context);

    return AWS_OK;
}

int aws_add_header(aws_t context, const char *key, const char *value)
{
#ifdef DEBUG
    assert(initialized);
#endif

    return aws_headers_add(context->headers, key, value);
}

int aws_add_param(aws_t context, const char *key, const char *value)
{
#ifdef DEBUG
    assert(initialized);
#endif

    return aws_params_add(context->params, key, value);
}

int aws_add_form_data(aws_t context, const char *form_data)
{
    if (is_post_request(context)) {
        bcatcstr(context->form_data, form_data);
    }

    return AWS_OK;
}

static int generate_canonical_request(aws_t context)
{
    bstring cr = bfromcstr("");
    bstring canonical_headers = aws_headers_canonicalize(context->headers);
    bstring signed_headers = aws_headers_sign(context->headers);
    bstring canonical_query_string = aws_params_canonicalize(context->params);

    char hashed_form_data[65];
    sha256_hex(hashed_form_data, (const char *)context->form_data->data);

    bconcat(cr, context->request_method);
    bcatcstr(cr, "\n");
    bconcat(cr, context->path);
    bcatcstr(cr, "\n");
    bconcat(cr, canonical_query_string);
    bcatcstr(cr, "\n");
    bconcat(cr, canonical_headers);
    bcatcstr(cr, "\n");
    bconcat(cr, signed_headers);
    bcatcstr(cr, "\n");
    bcatcstr(cr, (const char *)hashed_form_data);

    bdestroy(signed_headers);
    bdestroy(canonical_headers);
    bdestroy(canonical_query_string);

    context->canonical_request = cr;

    return AWS_OK;
}

static int generate_credential_scope(aws_t context)
{
    bstring cs = bfromcstr("");

    bconcat(cs, context->date.str_date_only);
    bconchar(cs, '/');
    bconcat(cs, context->region);
    bconchar(cs, '/');
    bconcat(cs, context->service);
    bconchar(cs, '/');
    bcatcstr(cs, AWS_REQUEST_STRING);

    context->credential_scope = cs;

    return AWS_OK;
}

static int generate_string_to_sign(aws_t context)
{
    bstring sts = bfromcstr(AWS_SIGNING_ALGORITHM);
    char hashed_canonical_request[65];

    bcatcstr(sts, "\n");
    bconcat(sts, context->date.str);
    bcatcstr(sts, "\n");

    generate_credential_scope(context);

    bconcat(sts, context->credential_scope);
    bcatcstr(sts, "\n");

    sha256_hex(hashed_canonical_request, (const char *)context->canonical_request->data);
    bcatcstr(sts, (const char *)hashed_canonical_request);

    context->string_to_sign = sts;

    return AWS_OK;
}

int aws_sign(aws_t context, const char *secret, const char *date, char *out)
{
    bstring kSecret;
    unsigned char kDate[32], kRegion[32], kService[32], signing_key[32];
    char signature[65];
    int i;

    if (parse_date(context, date) != AWS_OK)
        return AWS_ERR;

    generate_canonical_request(context);
    generate_string_to_sign(context);

    kSecret = bfromcstr(AWS_AWS4_STRING);
    bcatcstr(kSecret, secret);

    hmac(kDate, kSecret->data, strlen((char *)kSecret->data), context->date.str_date_only->data, strlen((char *)context->date.str_date_only->data));
    hmac(kRegion, kDate, HMAC_DIGEST_LENGTH, context->region->data, strlen((char *)context->region->data));
    hmac(kService, kRegion, HMAC_DIGEST_LENGTH, context->service->data, strlen((char *)context->service->data));
    hmac(signing_key, kService, HMAC_DIGEST_LENGTH, (unsigned char *)AWS_REQUEST_STRING, strlen(AWS_REQUEST_STRING));

    hmac_hex(signature,  signing_key, HMAC_DIGEST_LENGTH, context->string_to_sign->data, strlen((char *)context->string_to_sign->data));

    bdestroy(kSecret);

    for (i = 0; i < 64; i++)
        out[i] = signature[i];
    out[64] = '\0';

    return AWS_OK;
}

void aws_debug(aws_t context, char *out)
{
    bstring b = bfromcstr("");

    if (context->request_method) {
        bconcat(b, context->request_method);
        bcatcstr(b, "\n");
    }

    if (context->path) {
        bconcat(b, context->path);
        bcatcstr(b, "\n");
    }

    if (context->canonical_request) {
        bconcat(b, context->canonical_request);
        /* bcatcstr(b, "\n"); */
    }

    /* if (context->string_to_sign) { */
    /*     bconcat(b, context->string_to_sign); */
    /*     bcatcstr(b, "\n"); */
    /* } */

    /* if (context->signature) { */
    /*     bconcat(b, context->signature); */
    /*     bcatcstr(b, "\n"); */
    /* } */

    strcpy(out, bdata(b));
}

static int parse_date(aws_t context, const char *str)
{
    char yyyy_mm_dd_hh_mm_ss[17],
         yyyy_mm_dd[9];

    if (strptime(str, AWS_DATE_FORMAT, &context->date.tm) == NULL)
        return AWS_ERR;

    if (strftime(yyyy_mm_dd_hh_mm_ss, 17, "%Y%m%dT%H%M%SZ", &context->date.tm) == 0)
        return AWS_ERR;

    if (strftime(yyyy_mm_dd, 9, "%Y%m%d", &context->date.tm) == 0)
        return AWS_ERR;

    context->date.original_str = bfromcstr(str);
    context->date.str = bfromcstr(yyyy_mm_dd_hh_mm_ss);
    context->date.str_date_only = bfromcstr(yyyy_mm_dd);

    if (aws_add_header(context, "date", str) != AWS_OK)
        return AWS_ERR;

    return AWS_OK;
}

/**
 * Parse only the path portion of a string
 * It includes everything up until the "?" start of the query string
 */
static void parse_path(aws_t context, const char *the_path, int len)
{
    char *strpos, *path;

    path = malloc(sizeof(char) * len + 1);
    if (!path)
        return;
    strncpy(path, the_path, len + 1);

    strpos = strstr(the_path, "?");
    if (strpos != NULL)
        path[strpos-the_path] = '\0';
    else
        path[len] = '\0';

    context->path = uri_normalize(path);
    free(path);
}

/**
 * Parse the query parameters from a given string and save them in aws_params_t
 */
static void parse_query_parameters(aws_t context, const char *the_path, int len)
{
    char *path, *paramscope, *kvscope, *param;
    Pair kv;

    path = malloc(sizeof(char) * len + 1);
    if (!path)
        return;
    strncpy(path, the_path, len + 1);

    strtok_r(path, "?", &paramscope);
    while ((param = strtok_r(NULL, "&", &paramscope))) {
        kv.key = strtok_r(param, "=", &kvscope);
        kv.value = strtok_r(NULL, "=", &kvscope);

        aws_params_add(context->params, kv.key, kv.value);
    }

    free(path);
}

static bool is_valid_http_method(const char *method)
{
    int i;
    for (i = 0; i < HTTP_METHOD_COUNT; i++)
        if (strncmp(method, http_request_methods[i], HTTP_METHOD_MAX_LENGTH) == 0)
            return true;

    return false;
}

static bool is_post_request(aws_t context)
{
    return (bool)(strncmp((const char *)context->request_method->data, http_request_methods[1], HTTP_METHOD_MAX_LENGTH) == 0);
}
