#include "common.h"
#include "xmalloc.h"
#include "bsearch.h"

#define BPTR(ptr) ((uint8_t *)(ptr))

void *bsearch (void *ptr, size_t nmemb, size_t size, void *key, int (*cmp) (void *key, void *b))
{
        size_t w, s;
        int  cmpval;
        
        w = nmemb;
        s = 0;

        while (w > 0) {
                cmpval = cmp (key, BPTR(ptr) + (size * (s + w/2)));
                if (cmp > 0) {
                        s += w/2 + 1;
                        w  = w - w/2 - 1;
                } else if (cmp < 0) {
                        w = w/2;
                } else {
                        return &(BPTR(ptr) + (size * (s + w/2)));
                }
        }

        return (NULL);
}

#undef BPTR
