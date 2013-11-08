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

#define HMAC_DIGEST_LENGTH 32

/**
 * Hashes string with HMAC-256. Stores the hash in outputBuffer.
 */
void hmac(unsigned char out[32], const unsigned char *key, size_t key_len, const unsigned char *data, size_t data_len);

/**
 * Hashes string with HMAC-256 and hex-encodes output.
 * Stores the hash in outputBuffer.
 */
void hmac_hex(char out[65], const unsigned char *key, size_t key_len, const unsigned char *data, size_t data_len);

#endif
