#pragma once
#ifndef CRAPI_DIGEST_H
#define CRAPI_DIGEST_H

#include <stddef.h>

typedef enum {
        CRAPI_DIGEST_MD5,
        CRAPI_DIGEST_SHA1,
        CRAPI_DIGEST_SHA256,
        CRAPI_DIGEST_SHA512,
        CRAPI_DIGEST_RMD160
} crapi_alg_t;

int crapi_digest_fd (crapi_alg_t alg, int fd, void *dst, size_t *size);

#endif /* CRAPI_DIGEST_H */
