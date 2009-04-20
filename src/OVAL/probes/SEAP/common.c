#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <config.h>
#include "xmalloc.h"

void *xmemdup (const void *src, size_t len)
{
        void *new;

        new = xmalloc (len);
        memcpy (new, src, len);
        
        return (new);
}
