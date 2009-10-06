#pragma once
#ifndef BFIND_H
#define BFIND_H

#include <stddef.h>
#include <unistd.h>

#include "../../../../common/util.h"

OSCAP_HIDDEN_START;

void    *bfind   (void *ptr, size_t nmemb, size_t size, void *key, int cmpfn (void *, void *));
ssize_t  bfind_i (void *ptr, size_t nmemb, size_t size, void *key, int cmpfn (void *, void *));

OSCAP_HIDDEN_END;

#endif /* BFIND_H */
