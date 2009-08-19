#include <stddef.h>
#include <unistd.h>
#include <stdint.h>
#include <assert.h>
#include "bfind.h"

void *oscap_bfind (void *ptr, size_t nmemb, size_t size, void *key, int cmpfn (void *, void *))
{
        size_t w, s;
        int cmp;
        uint8_t *p;
        
        w = nmemb;
        s = 0;
        p = (uint8_t *)ptr;
        
        while (w > 0) {
                cmp = cmpfn (key, p + (size * (s + w/2)));
                if (cmp > 0) {
                        s += w/2 + 1;
                        w  = w - w/2 - 1;
                } else if (cmp < 0) {
                        w = w/2;
                } else {
                        return ((void *)(p + (size * (s + w/2))));
                }
        }
        
        return (NULL);
}

int32_t oscap_bfind_i (void *ptr, size_t nmemb, size_t size, void *key, int cmpfn (void *, void *))
{
        size_t w, s;
        int cmp;
        uint8_t *p;

        w = nmemb;
        s = 0;
        p = (uint8_t *)ptr;
        
        while (w > 0) {
                cmp = cmpfn (key, p + (size * (s + w/2)));
                if (cmp > 0) {
                        s += w/2 + 1;
                        w  = w - w/2 - 1;
                } else if (cmp < 0) {
                        w = w/2;
                } else {
                        assert ((s + w/2) <= INT32_MAX);
                        return ((int32_t)(s + w/2));
                }
        }
        
        return INT32_C(-1);
}
