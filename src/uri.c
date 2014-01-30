#include <stdlib.h>
#include <string.h>
#include <uriparser/Uri.h>
#include "bstrlib.h"
#include "uri.h"

void clean_uri(const char *uri, char *out);

bstring uri_normalize(const char *uri)
{
    int st, origSize, normSize;
    char *normal_str, *cleaned_uri;
    bstring normalized;
    UriParserStateA state;
    UriUriA u;
    state.uri = &u;

    cleaned_uri = malloc((sizeof(char) * strlen(uri)) + 1);
    clean_uri(uri, cleaned_uri);

    /* Parse URI */
    if (uriParseUriA(&state, cleaned_uri) != URI_SUCCESS) {
        free(cleaned_uri);
        uriFreeUriMembersA(&u);
        return bfromcstr("");
    }

    free(cleaned_uri);

    /* Normalize URI */
    st = uriNormalizeSyntaxA(&u);
    if (st != URI_SUCCESS) {
        uriFreeUriMembersA(&u);
        return bfromcstr("");
    }

    origSize = strlen(uri);
    normal_str = malloc(sizeof(char) * origSize + 1);
    if (normal_str == NULL) {
        uriFreeUriMembersA(&u);
        return bfromcstr("");
    }

    st = uriToStringA(normal_str, &u, origSize + 1, &normSize);
   if (st != URI_SUCCESS) {
        uriFreeUriMembersA(&u);
        return bfromcstr("");
    }

    normalized = bfromcstr(normal_str);

    free(normal_str);
    uriFreeUriMembersA(&u);

    return normalized;
}

void clean_uri(const char *uri, char *out)
{
    char last_last_char = 0, last_char = 0;
    int uri_len, i, newlen = 0;

    uri_len = strlen(uri);

    for (i = 0; i < uri_len; i++) {
        if (uri[i] == '/' && (last_char == '/' && last_last_char != ':'))
            continue;

        last_last_char = last_char;
        last_char = uri[i];
        out[newlen] = uri[i];
        newlen++;
    }

    out[newlen] = '\0';
}
