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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "aws_sigv4.h"
#include "aws_headers.h"
#include "aws_params.h"
#include "aws_status.h"
#include "bstrlib.h"
#include "sha256.h"

/* TODO does this belong here? probably not */
#define GLACIER_API_VERSION_KEY "x-amz-glacier-version"
#define GLACIER_API_VERSION_VALUE "x-amz-glacier-version"

static char *http_request_methods[4] = {"GET", "POST", "PUT", "DELETE"};

struct aws_context {
    bstring request_method;
    bstring account_id;
    bstring region;
    bstring service;
    bstring host;
    bstring path;
    bstring request_date;
    bstring canonical_request;
    bstring credential_scope;
    bstring string_to_sign;
    bstring signature;
    aws_headers_t headers;
    aws_params_t params;
};

static bool initialized = false;

/* Private functions */

static int generate_canonical_request(aws_t context);
static int generate_credential_scope(aws_t context, char date[17]);
static int generate_string_to_sign(aws_t context, char date[17]);

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

    /* TODO create canonical URI */
    context->path = bfromcstr(path);
    if (!context->path)
        return NULL;

    /* TODO check if http_method is allowed */
    context->request_method = bfromcstr(http_method);
    if (!context->request_method)
        return NULL;

    context->account_id = NULL;
    context->request_date = NULL;
    context->canonical_request = NULL;
    context->credential_scope = NULL;
    context->string_to_sign = NULL;
    context->signature = NULL;

    initialized = true;

    /* TODO glacier api version key and version value need to be added as header.
     * This can be done from the outside */

    return context;
}

int aws_cleanup(aws_t context)
{
    assert(initialized);

    /* TODO need to check if null first? */
    bdestroy(context->account_id);
    bdestroy(context->region);
    bdestroy(context->service);
    bdestroy(context->request_date);
    bdestroy(context->host);
    bdestroy(context->path);
    bdestroy(context->request_method);
    bdestroy(context->canonical_request);
    bdestroy(context->credential_scope);
    bdestroy(context->string_to_sign);
    bdestroy(context->signature);

    aws_headers_destroy(context->headers);
    aws_params_destroy(context->params);

    context->headers = NULL;
    context->params = NULL;

    free(context);

    return AWS_OK;
}

int aws_add_header(aws_t context, const char *key, const char *value)
{
    assert(initialized);
    return aws_headers_add(context->headers, key, value);
}

int aws_add_param(aws_t context, const char *key, const char *value)
{
    assert(initialized);
    return aws_params_add(context->params, key, value);
}

static int generate_canonical_request(aws_t context)
{
    bstring cr = bfromcstr("");
    bstring canonical_headers = aws_headers_canonicalize(context->headers);
    bstring signed_headers = aws_headers_sign(context->headers);
    bstring canonical_query_string = aws_params_canonicalize(context->params);

    char hashed_payload[65];
    sha256(hashed_payload, (const char *)canonical_query_string->data);

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
    bcatcstr(cr, hashed_payload);

    bdestroy(signed_headers);
    bdestroy(canonical_headers);
    bdestroy(canonical_query_string);

    context->canonical_request = cr;

    return AWS_OK;
}

static int generate_credential_scope(aws_t context, char date[17])
{
    bstring cs = bfromcstr("");
    char yyyy_mm_dd[9];

    strncpy(yyyy_mm_dd, date, 8);
    yyyy_mm_dd[8] = '\0';

    bcatcstr(cs, yyyy_mm_dd);
    bconchar(cs, '/');
    bconcat(cs, context->region);
    bconchar(cs, '/');
    bconcat(cs, context->service);
    bconchar(cs, '/');
    bcatcstr(cs, AWS_REQUEST_STRING);

    context->credential_scope = cs;

    return AWS_OK;
}

static int generate_string_to_sign(aws_t context, char date[17])
{
    bstring sts = bfromcstr(AWS_SIGNING_ALGORITHM);
    char hashed_canonical_request[65];

    bcatcstr(sts, "\n");
    bcatcstr(sts, date);
    bcatcstr(sts, "\n");

    generate_credential_scope(context, date);

    bconcat(sts, context->credential_scope);
    bcatcstr(sts, "\n");

    sha256(hashed_canonical_request, (const char *)context->canonical_request->data);
    bcatcstr(sts, hashed_canonical_request);

    context->string_to_sign = sts;

    return AWS_OK;
}

char *aws_sign(aws_t context, char date[17])
{
    bstring signature;

    generate_canonical_request(context);
    generate_string_to_sign(context, date);

    /* DEBUG */
    /* printf("Canonical Request:\n%s\n", context->canonical_request->data); */
    /* printf("String to Sign:\n%s\n", context->string_to_sign->data); */

    /* TODO */
    return "";
}
