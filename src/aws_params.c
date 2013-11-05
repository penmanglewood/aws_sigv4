#include <stdlib.h>
#include <string.h>
#include "aws_params.h"
#include "aws_status.h"
#include "bstrlib.h"

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

    bkey = bfromcstr(key);
    if (!bkey)
        return AWS_ERR;

    bval = bfromcstr(value);
    if (!bval)
        return AWS_ERR;

    /* TODO URI encode bkey */
    /* TODO URI encode bval */

    KeyValue param = {bkey, bval};

    context->params[context->size] = param;
    context->size++;

    return AWS_OK;
}

bstring aws_params_canonicalize(aws_params_t context)
{
    int i, cmp;
    bstring last_key = bfromcstr("");
    bstring c = bfromcstr("");

    /* TODO sort by key */

    for (i = 0; i < context->size; i++) {
        cmp = biseq(context->params[i].key, last_key);
        if (cmp == 0) {
            bconcat(c, context->params[i].key);
            bconchar(c, '=');
            bconcat(c, context->params[i].value);

            if (i < context->size - 1)
                bconchar(c, '&');
        } else if (cmp == -1) {
            break;
        }

        bdestroy(last_key);
        last_key = bstrcpy(context->params[i].key);
    }

    bdestroy(last_key);

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

static int sort_param(const void *a, const void *b)
{
    return ((struct keyvalue *) a)->key->data[0] - ((struct keyvalue *) b)->key->data[0];
}
