#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "bstrlib.h"
#include "stringy.h"

#define HEX_CHAR_LEN 3 /* Percent sign plus two characters */

/* static char hextoint(const char c); */
static char inttohex(const char c);
static void printhex(char *dest, const char src);

bstring trimall(const char *str)
{
    bstring trimmed;
    char *buf, *bufItr;
    const char *strItr;
    int trimmed_str_count = 1,
        unnecessary_whitespace = 0,
        start_idx = -1,
        end_idx,
        i;
    bool in_quotes = false;
    char last_char;

    if (str == NULL || strcmp(str, "") == 0)
        return bfromcstr("");

    for (i = 0; i < (strlen(str) + 1); i++) {
        if (!isspace(str[i]) && str[i] != '\0' && str[i] != EOF) {
            if (start_idx < 0)
                start_idx = i;
            end_idx = i+1;
        }

        if (str[i] == '\'' || str[i] == '\"')
            in_quotes = !in_quotes;

        if (isspace(str[i])) {
            if (!in_quotes && isspace(last_char) && start_idx >= 0) {
                unnecessary_whitespace++;
            }
        }

        last_char = str[i];
    }

    if (end_idx - start_idx == 0)
        return bfromcstr("");

    strItr = str + (strlen(str) - 1);
    for (i = strlen(str) - 1; i >= 0; i--) {
        if (isspace(str[i]))
            unnecessary_whitespace--;
    }

    trimmed_str_count = ((end_idx - start_idx) - unnecessary_whitespace) + 1;

    buf = malloc(sizeof(char) * trimmed_str_count);
    if (!buf)
        return bfromcstr("");

    bufItr = buf;
    last_char = str[start_idx];
    /* printf("For [%s], iterate from %d..%d for length of %d, with %d unnecessary ws\n", str, start_idx, end_idx, trimmed_str_count, unnecessary_whitespace); */
    for (i = start_idx; i < end_idx; i++) {
        if (str[i] == '\'' || str[i] == '\"')
            in_quotes = !in_quotes;

        if (!in_quotes && isspace(last_char) && isspace(str[i]))
            continue;

        *bufItr++ = str[i];
        /* printf("\tAdd [%c]\n", str[i]); */

        last_char = str[i];
    }
    *bufItr = '\0';

    trimmed = bfromcstr(buf);
    free(buf);

    return trimmed;
}

bstring uri_encode(const char *str)
{
    char *oldStrBuf,
         *newStrBuf,
         *encodedString;
    int encodableCharacters = 0,
        newStrLen = 0;
    bstring encoded;

    oldStrBuf = (char *)str;
    while(*oldStrBuf) {
        if (*oldStrBuf != '\0' &&
            !isalnum(*oldStrBuf) &&
            *oldStrBuf != '-' && *oldStrBuf != '_' &&
            *oldStrBuf != '.' && *oldStrBuf != '~') {
            encodableCharacters++;
        }

        oldStrBuf++;
    }

    newStrLen = (strlen(str) - encodableCharacters) +
        (encodableCharacters * HEX_CHAR_LEN) +
        1;

    encodedString = malloc(newStrLen);
    if (encodedString == NULL)
        return bfromcstr("");

    newStrBuf = encodedString;
    oldStrBuf = (char *)str;
    newStrBuf[newStrLen - 1] = '\0';
    while (*oldStrBuf) {
        if (*oldStrBuf != '\0' &&
            !isalnum(*oldStrBuf) &&
            *oldStrBuf != '-' && *oldStrBuf != '_' && *oldStrBuf != '.' && *oldStrBuf != '~')
            *newStrBuf++ = '%', printhex(newStrBuf++, *oldStrBuf);
        else
            *newStrBuf = *oldStrBuf;

        oldStrBuf++;
        newStrBuf++;
    }

    encoded = bfromcstr(encodedString);
    free(encodedString);

    return encoded;
}

/* Writes the hexadecimal 2-character byte to dest as a string */
static void printhex(char *dest, const char src)
{
    *dest = inttohex(src >> 4);
    *(dest + 1) = inttohex(src & 15);
}

/* Convert hex character to its integer value */
/* static char hextoint(const char c) */
/* { */
/*     return isdigit(c) ? c - '0' : tolower(c) - 'a' + 10; */
/* } */

/* Convert integer to its hex value */
static char inttohex(const char c)
{
    static char hex[] = "0123456789ABCDEF";
    return hex[c & 15];
}
