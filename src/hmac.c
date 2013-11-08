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
#include "hmac.h"

#if defined(__APPLE__)
#include <CommonCrypto/CommonHMAC.h>
#else
#include <openssl/hmac.h>
#endif

void hmac(unsigned char out[32], const unsigned char *key, size_t key_len, const unsigned char *data, size_t data_len)
{
#if defined(__APPLE__)
    CCHmac(kCCHmacAlgSHA256, key, key_len, data, data_len, out);
#else
    HMAC(EVP_sha256(), key, key_len, (unsigned char *)data, data_len, NULL, NULL);
#endif
}

void hmac_hex(char out[65], const unsigned char *key, size_t key_len, const unsigned char *data, size_t data_len)
{
    unsigned char hash[HMAC_DIGEST_LENGTH];
    int i;

    hmac(hash, key, key_len, data, data_len);

    for (i = 0; i < HMAC_DIGEST_LENGTH; i++)
        sprintf(out + (i * 2), "%02x", hash[i]);

    out[64] = '\0';
}
