#pragma once
#ifndef CRAPI_SHA2_H
#define CRAPI_SHA2_H

#include <stddef.h>

void *crapi_sha256_init (void *dst, void *size);
int   crapi_sha256_update (void *ctxp, void *bptr, size_t blen);
int   crapi_sha256_fini (void *ctxp);
void  crapi_sha256_free (void *ctxp);

int crapi_sha256_fd (int fd, void *dst, size_t *size);

void *crapi_sha512_init (void *dst, void *size);
int   crapi_sha512_update (void *ctxp, void *bptr, size_t blen);
int   crapi_sha512_fini (void *ctxp);
void  crapi_sha512_free (void *ctxp);

int crapi_sha512_fd (int fd, void *dst, size_t *size);

#endif /* CRAPI_SHA2_H */
