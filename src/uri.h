#ifndef URI_H
#define URI_H

#include "bstrlib.h"

/**
 * Normalize URI according to RFC 3986
 *
 * @param uri [in] URI string
 * @return bstring. Empty bstring on failure. Must be freed.
 */
bstring uri_normalize(const char *uri);

#endif
