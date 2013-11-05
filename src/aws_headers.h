#ifndef AWS_HEADERS_H
#define AWS_HEADERS_H

#include "bstrlib.h"

typedef struct aws_headers *aws_headers_t;

/**
 * Initialize a header context
 *
 * return aws_headers_t context
 */
aws_headers_t aws_headers_init();

/**
 * Deallocates all headers and the context variable 
 *
 * return AWS_OK on success, AWS_ERR on failure
 */
int aws_headers_destroy(aws_headers_t context);

/**
 * Add a header key and value
 *
 * return AWS_OK on success, AWS_ERR on failure
 */
int aws_headers_add(aws_headers_t context, const char *key, const char *value);

/**
 * Returns headers in a canonical form 
 *
 * return bstring. If failure, returns empty bstring.
 */
bstring aws_headers_canonicalize(aws_headers_t context);

/**
 * Returns signed headers
 *
 * return bstring. If failure, returns empty bstring.
 */
bstring aws_headers_sign(aws_headers_t context);

#endif
