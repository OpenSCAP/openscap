#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <config.h>
#include "xmalloc.h"
#include "common.h"

void *xmemdup (const void *src, size_t len)
{
        void *new;

        new = xmalloc (len);
        memcpy (new, src, len);
        
        return (new);
}

uint32_t xnumdigits (size_t size)
{
        uint32_t i = 0;

        do {
                ++i;
                size /= 10;
        } while (size > 0);

        return (i);
}

int xstrncoll (const char *a, size_t alen,
               const char *b, size_t blen)
{
        int cmp;
        
        if (alen > blen) {
                cmp = strncmp (a, b, blen);
                return (cmp == 0 ?  1 : cmp);
        } else if (blen > alen) {
                cmp = strncmp (a, b, alen);
                return (cmp == 0 ? -1 : cmp);
        } else {
                return strncmp (a, b, alen);
        }
}
