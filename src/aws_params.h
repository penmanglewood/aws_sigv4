#ifndef AWS_PARAMS_H
#define AWS_PARAMS_H

#include "bstrlib.h"

typedef struct aws_params *aws_params_t;

/**
 * Initialize a context
 *
 * return aws_params_t context
 */
aws_params_t aws_params_init();

/**
 * Deallocates all params and the context variable 
 *
 * return AWS_OK on success, AWS_ERR on failure
 */
int aws_params_destroy(aws_params_t context);

/**
 * Add a query string key and value
 *
 * return AWS_OK on success, AWS_ERR on failure
 */
int aws_params_add(aws_params_t context, const char *key, const char *value);

/**
 * Returns params in a canonical form 
 *
 * return bstring. If failure, returns empty bstring.
 */
bstring aws_params_canonicalize(aws_params_t context);

#endif
