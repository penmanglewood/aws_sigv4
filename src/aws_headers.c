#include <stdlib.h>
#include <string.h>
#include "aws_headers.h"
#include "aws_status.h"
#include "bstrlib.h"
#include "stringy.h"

#define INITIAL_HEADER_COUNT 8
#define GROW_BY 8

typedef struct keyvalue {
    bstring key;
    bstring value;
} KeyValue;

struct aws_headers {
    KeyValue *headers;
    size_t alloc; /* Number of bytes allocated to *headers */
    int size; /* Number of headers */
};

static int expand_array(aws_headers_t context);
static int sort_header(const void *a, const void *b);

aws_headers_t aws_headers_init()
{
    aws_headers_t ctx = malloc(sizeof(struct aws_headers));
    if (!ctx)
        return NULL;

    ctx->size = 0;
    ctx->alloc = sizeof(KeyValue) * INITIAL_HEADER_COUNT;
    ctx->headers = malloc(ctx->alloc);
    if (!ctx->headers)
        return NULL;

    return ctx;
}

int aws_headers_destroy(aws_headers_t context)
{
    int i;
    for (i = 0; i < context->size; i++)  {
        bdestroy(context->headers[i].key);
        bdestroy(context->headers[i].value);
    }
    free(context->headers);
    free(context);

    return AWS_OK;
}

int aws_headers_add(aws_headers_t context, const char *key, const char *value)
{
    int status;
    bstring bkey, bval;

    if (key == NULL ||
            value == NULL ||
            strcmp(key, "") == 0 ||
            strcmp(value, "") == 0)
        return AWS_ERR;

    status = expand_array(context);
    if (status == AWS_ERR)
        return AWS_ERR;

    bkey = bfromcstr(key);
    if (!bkey)
        return AWS_ERR;
    btolower(bkey);

    bval = trimall(value);
    bval = bfromcstr(value);
    if (!bval)
        return AWS_ERR;

    KeyValue header = {bkey, bval};

    context->headers[context->size] = header;
    context->size++;

    qsort(context->headers, context->size, sizeof(KeyValue), sort_header);

    return AWS_OK;
}

bstring aws_headers_canonicalize(aws_headers_t context)
{
    int i, cmp;
    bstring last_key = bfromcstr("");
    bstring c = bfromcstr("");

    for (i = 0; i < context->size; i++) {
        cmp = biseq(context->headers[i].key, last_key);
        if (cmp == 1) { /* Same header */
            bconchar(c, ',');
            bconcat(c, context->headers[i].value);
        } else if (cmp == 0) {
            if (i > 0)
                bcatcstr(c, "\n");

            bconcat(c, context->headers[i].key);
            bconchar(c, ':');
            bconcat(c, context->headers[i].value);
        } else if (cmp == -1) {
            break;
        }

        bdestroy(last_key);
        last_key = bstrcpy(context->headers[i].key);
    }

    if (cmp != -1)
        bcatcstr(c, "\n");

    bdestroy(last_key);

    return c;
}

bstring aws_headers_sign(aws_headers_t context)
{
    int i, cmp;
    bstring last_key = bfromcstr("");
    bstring c = bfromcstr("");

    for (i = 0; i < context->size; i++) {
        cmp = biseq(context->headers[i].key, last_key);
        if (cmp == 0) {
            bconcat(c, context->headers[i].key);
            if (i < context->size - 1)
                bconchar(c, ';');
        } else if (cmp == -1) {
            break;
        }

        bdestroy(last_key);
        last_key = bstrcpy(context->headers[i].key);
    }

    bdestroy(last_key);

    return c;
}

static int expand_array(aws_headers_t context)
{
    KeyValue *headers;
    if (context->alloc >= (sizeof(KeyValue) * context->size)) {
        context->alloc = sizeof(KeyValue) * (context->size + GROW_BY);
        headers = realloc(context->headers, context->alloc);
        if (!headers)
            return AWS_ERR;

        context->headers = headers;
    }

    return AWS_OK;
}

static int sort_header(const void *a, const void *b)
{
    bstring ka = ((KeyValue *)a)->key;
    bstring kb = ((KeyValue *)b)->key;

    return bstrcmp(ka, kb);
}
