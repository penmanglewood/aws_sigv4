#include <stdlib.h>
#include <string.h>
#include "aws_params.h"
#include "aws_status.h"
#include "bstrlib.h"
#include "stringy.h"

#define INITIAL_PARAM_COUNT 8
#define GROW_BY 8

typedef struct keyvalue {
    bstring key;
    bstring value;
} KeyValue;

struct aws_params {
    KeyValue *params;
    size_t alloc; /* Number of bytes allocated to *params */
    int size; /* Number of params */
};

static int expand_array(aws_params_t context);
static void kvsort(aws_params_t context);
static int cmp_keys(const void *a, const void *b);
static int cmp_values(const void *a, const void *b);

aws_params_t aws_params_init()
{
    aws_params_t ctx = malloc(sizeof(struct aws_params));
    if (!ctx)
        return NULL;

    ctx->size = 0;
    ctx->alloc = sizeof(KeyValue) * INITIAL_PARAM_COUNT;
    ctx->params = malloc(ctx->alloc);
    if (!ctx->params)
        return NULL;

    return ctx;
}

int aws_params_destroy(aws_params_t context)
{
    int i;
    for (i = 0; i < context->size; i++)  {
        bdestroy(context->params[i].key);
        bdestroy(context->params[i].value);
    }
    free(context->params);
    free(context);

    return AWS_OK;
}

int aws_params_add(aws_params_t context, const char *key, const char *value)
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

    bkey = uri_encode(key);
    if (!bkey)
        return AWS_ERR;

    bval = uri_encode(value);
    if (!bval)
        return AWS_ERR;

    KeyValue param = {bkey, bval};

    context->params[context->size] = param;
    context->size++;

    return AWS_OK;
}

bstring aws_params_canonicalize(aws_params_t context)
{
    int i;
    bstring c = bfromcstr("");

    kvsort(context);

    for (i = 0; i < context->size; i++) {
        bconcat(c, context->params[i].key);
        bconchar(c, '=');
        bconcat(c, context->params[i].value);

        if (i < context->size - 1)
            bconchar(c, '&');
    }

    return c;
}

static int expand_array(aws_params_t context)
{
    KeyValue *params;
    if (context->alloc >= (sizeof(KeyValue) * context->size)) {
        context->alloc = sizeof(KeyValue) * (context->size + GROW_BY);
        params = realloc(context->params, context->alloc);
        if (!params)
            return AWS_ERR;

        context->params = params;
    }

    return AWS_OK;
}

static int cmp_keys(const void *a, const void *b)
{
    bstring ka = ((KeyValue *)a)->key;
    bstring kb = ((KeyValue *)b)->key;

    return bstrcmp(ka, kb);
}

static int cmp_values(const void *a, const void *b)
{
    bstring ka = ((KeyValue *)a)->value;
    bstring kb = ((KeyValue *)b)->value;

    return bstrcmp(ka, kb);
}

static void kvsort(aws_params_t context)
{
    int i, j, key_changed = 0;
    KeyValue current_kv;
    KeyValue tmp;

    // Sort keys
    qsort(context->params, context->size, sizeof(KeyValue), cmp_keys);

    for (i = 0; i < context->size; i++) {
        current_kv = context->params[i];
        j = i + 1;
        while (j < context->size && bstrcmp(current_kv.key, context->params[j].key) == 0) {
            if (cmp_values(&context->params[i], &context->params[j]) > 0) {
                tmp = context->params[i];
                context->params[i] = context->params[j];
                context->params[j] = tmp;
            }
            j++;
        }
    }
}
