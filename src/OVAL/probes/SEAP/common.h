#pragma once
#ifndef COMMON_H
#define COMMON_H

#define _STR(x) #x
#define  STR(x) _STR(x)

#ifndef _D
# ifndef NDEBUG
#  include <stdio.h>
#  define _D(...) do {                                          \
                fprintf (stderr, "%s: ", __PRETTY_FUNCTION__);  \
                fprintf (stderr, __VA_ARGS__);                  \
        } while(0)
# else
#  define _D(...) while(0)
# endif /* NDEBUG */
#endif /* _D */

#include <stddef.h>
#include <stdint.h>

void    *xmemdup (const void *src, size_t len);
uint32_t xnumdigits (size_t size);
int      xstrncoll (const char *a, size_t alen, const char *b, size_t blen);

#endif /* COMMON_H */
