#include <stdlib.h>
#include <string.h>
#include <uriparser/Uri.h>
#include "bstrlib.h"
#include "uri.h"

bstring uri_normalize(const char *uri)
{
    int st, origSize, normSize;
    char *normal_str;
    bstring normalized;
    UriParserStateA state;
    UriUriA u;
    state.uri = &u;

    /* Parse URI */
    if (uriParseUriA(&state, uri) != URI_SUCCESS) {
        uriFreeUriMembersA(&u);
        return bfromcstr("");
    }

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
