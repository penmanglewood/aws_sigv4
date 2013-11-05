/*
 * =====================================================================================
 *
 *       Filename:  aws_sigv4.h
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

#ifndef AWS_H
#define AWS_H

#define SIGNING_ALGORITHM "AWS4-HMAC-SHA256"

typedef struct aws_context *aws_t;

/* Initialize aws context */
aws_t aws_init(const char *host, const char *path, const char *http_method);

/* Clean up aws instance */
int aws_cleanup(aws_t context);

/* Add header to the current context */
int aws_add_header(aws_t context, const char *key, const char *value);

/* Add query string parameter to the current context */
int aws_add_param(aws_t context, const char *key, const char *value);

/* Should this be public? */
void generate_canonical_request(aws_t context);

#endif
