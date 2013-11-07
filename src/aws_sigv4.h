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

#define AWS_SIGNING_ALGORITHM "AWS4-HMAC-SHA256"
#define AWS_REQUEST_STRING "aws4_request"

typedef struct aws_context *aws_t;

/**
 * Initialize aws context
 *
 * returns context object
 */
aws_t aws_init(const char *region, const char *service, const char *host, const char *path, const char *http_method);

/**
 * Clean up aws instance
 *
 * returns AWS_ERR if failure, AWS_OK otherwise
 */
int aws_cleanup(aws_t context);

/**
 * Add header to the current context
 *
 * returns AWS_ERR if failure, AWS_OK otherwise
 */
int aws_add_header(aws_t context, const char *key, const char *value);

/**
 * Add query string parameter to the current context
 *
 * returns AWS_ERR if failure, AWS_OK otherwise
 */
int aws_add_param(aws_t context, const char *key, const char *value);

/**
 * Generate the signature
 *
 * returns char * with the signature. The return value must be freed.
 */
char *aws_sign(aws_t context, char date[17]);

#endif
