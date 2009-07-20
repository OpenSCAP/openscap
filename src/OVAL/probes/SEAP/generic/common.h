#pragma once
#ifndef COMMON_H
#define COMMON_H

#define _STR(x) #x
#define  STR(x) _STR(x)

#include <stddef.h>
#include <stdint.h>

void    *xmemdup (const void *src, size_t len);
uint32_t xnumdigits (size_t size);
int      xstrncoll (const char *a, size_t alen, const char *b, size_t blen);
void     xsrandom (unsigned long seed);
long     xrandom (void);

#ifndef EDOOFUS
# define EDOOFUS 88
#endif

#endif /* COMMON_H */
