#pragma once
#ifndef COMMON_H
#define COMMON_H

#define _STR(x) #x
#define  STR(x) _STR(x)

#ifndef _D
# ifndef NDEBUG
#  include <stdio.h>
#  include <errno.h>
#  define _D(...) do {                                          \
                int e = errno;                                  \
                fprintf (stderr, "%s: ", __PRETTY_FUNCTION__);  \
                errno = e;                                      \
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
void     xsrandom (unsigned long seed);
long     xrandom (void);

#endif /* COMMON_H */
