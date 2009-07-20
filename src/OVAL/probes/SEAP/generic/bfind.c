#include <stddef.h>
#include <unistd.h>
#include <stdint.h>
#include "bfind.h"

void *bfind (void *ptr, size_t nmemb, size_t size, void *key, int cmpfn (void *, void *))
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

ssize_t bfind_i (void *ptr, size_t nmemb, size_t size, void *key, int cmpfn (void *, void *))
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
                        return (s + w/2);
                }
        }
        
        return (-1);
}
