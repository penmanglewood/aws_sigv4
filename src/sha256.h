/*
 * =====================================================================================
 *
 *       Filename:  sha256.h
 *
 *    Description:  Calculates a string's SHA-256 hash
 *
 *        Version:  1.0
 *        Created:  30/05/2013 11:04:59
 *       Compiler:  gcc
 *      Libraries:  ssl, crypto
 *
 *         Author:  Eric Rubio
 *
 * =====================================================================================
 */

#ifndef SHA256_H
#define SHA256_H

/**
 * Hashes string with SHA-256. Stores the hash in outputBuffer.
 *
 * return void
 */
void sha256(unsigned char out[33], const char *data);

/**
 * Hashes string with SHA-256 and hex-encodes the output data
 *
 * out a character array to write the result to
 * data the string to hash
 *
 * return void
 */
void sha256_hex(unsigned char out[65], const char *data);

#endif
