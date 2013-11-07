/*
 * =====================================================================================
 *
 *       Filename:  hmac.h
 *
 *    Description:  Calculates an HMAC
 *
 *        Version:  1.0
 *       Compiler:  gcc
 *      Libraries:  ssl, crypto
 *
 *         Author:  Eric Rubio
 *
 * =====================================================================================
 */

#ifndef HMAC256_H
#define HMAC256_H

/**
 * Hashes string with HMAC-256. Stores the hash in outputBuffer.
 */
void hmac(char out[33], const char *key, const char *data);

/**
 * Hashes string with HMAC-256 and hex-encodes output.
 * Stores the hash in outputBuffer.
 */
void hmac_hex(char out[65], const char *key, const char *data);

#endif
