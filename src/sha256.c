/*
 * =====================================================================================
 *
 *       Filename:  sha256.h
 *
 *    Description:  Wrapper for OpenSSL's SHA-256
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

#include <stdio.h>
#include <string.h>

#define SHA256_DIGEST_LENGTH 32

#if defined(__APPLE__)
#include <CommonCrypto/CommonDigest.h>
#else
#include <openssl/sha.h>
#endif

void sha256(unsigned char out[32], const char *data)
{
#if defined(__APPLE__)
    CC_SHA256(data, strlen(data), (unsigned char *)out);
#else
    SHA256_CTX sha;
    SHA256_Init(&sha);
    SHA256_Update(&sha, data, strlen(data));
    SHA256_Final(out, &sha);
#endif
}

void sha256_hex(char out[65], const char *data)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    int i;

    sha256(hash, data);

    for (i = 0; i < SHA256_DIGEST_LENGTH; i++)
        sprintf(out + (i * 2), "%02x", hash[i]);

    out[64] = '\0';
}
