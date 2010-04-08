#pragma once
#ifndef CRAPI_SHA1_H
#define CRAPI_SHA1_H

#include <stddef.h>

void *crapi_sha1_init (void *dst, void *size);
int   crapi_sha1_update (void *ctxp, void *bptr, size_t blen);
int   crapi_sha1_fini (void *ctxp);
void  crapi_sha1_free (void *ctxp);

int crapi_sha1_fd (int fd, void *dst, size_t *size);

#endif /* CRAPI_SHA1_H */
