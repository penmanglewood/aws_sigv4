/*
 * =====================================================================================
 *
 *       Filename:  hmac.h
 *
 *    Description:  Wrapper for OpenSSL's HMAC-256
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

#define HMAC_DIGEST_LENGTH 32

#if defined(__APPLE__)
#include <CommonCrypto/CommonHMAC.h>
#else
#include <openssl/hmac.h>
#endif

void hmac(char out[33], const char *key, const char *data)
{
    int i;

#if defined(__APPLE__)
    CCHmac(kCCHmacAlgSHA256, key, strlen(key), data, strlen(data), (unsigned char *)out);
#else
    HMAC(EVP_sha256(), key, strlen(key), (unsigned char *)data, strlen(data), NULL, NULL);
#endif

    out[32] = '\0';
}

void hmac_hex(char out[65], const char *key, const char *data)
{
    char hash[HMAC_DIGEST_LENGTH + 1];
    int i;

    hmac(hash, key, data);

    for (i = 0; i < HMAC_DIGEST_LENGTH; i++)
        sprintf(out + (i * 2), "%02x", hash[i]);

    out[64] = '\0';
}
