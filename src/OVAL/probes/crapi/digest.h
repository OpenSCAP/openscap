#pragma once
#ifndef CRAPI_DIGEST_H
#define CRAPI_DIGEST_H

#include <stdarg.h>
#include <stddef.h>

typedef enum {
        CRAPI_DIGEST_MD5    = 0x01,
        CRAPI_DIGEST_SHA1   = 0x02,
        CRAPI_DIGEST_SHA256 = 0x04,
        CRAPI_DIGEST_SHA512 = 0x08,
        CRAPI_DIGEST_RMD160 = 0x10
} crapi_alg_t;

int crapi_digest_fd (int fd, crapi_alg_t alg, void *dst, size_t *size);
int crapi_mdigest_fd (int fd, int num, crapi_alg_t alg, void *dst, size_t *size, ...);

#endif /* CRAPI_DIGEST_H */
