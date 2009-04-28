#pragma once
#ifndef COMMON_H
#define COMMON_H

void  *xmemdup (const void *src, size_t len);

#include <sys/types.h>
uint32_t xnumdigits (size_t size);

int xstrncoll (const char *a, size_t alen, const char *b, size_t blen);
#endif /* COMMON_H */
