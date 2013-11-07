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

void sha256(char out[33], const char *data)
{
    int i;

#if defined(__APPLE__)
    CC_SHA256(data, strlen(data), (unsigned char *)out);
#else
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data, strlen(data));
    SHA256_Final(out, &sha256);
#endif

    out[32] = '\0';
}

void sha256_hex(char out[65], const char *data)
{
    char hash[SHA256_DIGEST_LENGTH + 1];
    int i;

    sha256(hash, data);

    for (i = 0; i < SHA256_DIGEST_LENGTH; i++)
        sprintf(out + (i * 2), "%02x", hash[i]);

    out[64] = '\0';
}
