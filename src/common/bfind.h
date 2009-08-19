#pragma once
#ifndef BFIND_H
#define BFIND_H

#include <stddef.h>
#include <unistd.h>
#include <stdint.h>

void    *oscap_bfind   (void *ptr, size_t nmemb, size_t size, void *key, int cmpfn (void *, void *));
int32_t  oscap_bfind_i (void *ptr, size_t nmemb, size_t size, void *key, int cmpfn (void *, void *));

#endif /* BFIND_H */
