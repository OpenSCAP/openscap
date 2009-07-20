#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <config.h>
#include "public/sm_alloc.h"
#include "common.h"

#ifndef _A
#define _A(x) assert(x)
#endif

void *xmemdup (const void *src, size_t len)
{
        void *new;
        
        _A(src != NULL);

        new = sm_alloc (len);
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

        _A(a != NULL);
        _A(b != NULL);
        
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

void xsrandom (unsigned long seed)
{
        srandom (seed);
}

long xrandom (void)
{
        return random ();
}
