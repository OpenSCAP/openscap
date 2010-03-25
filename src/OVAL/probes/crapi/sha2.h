#pragma once
#ifndef CRAPI_SHA2_H
#define CRAPI_SHA2_H

#include <stddef.h>

int crapi_sha256_fd (int fd, void *dst, size_t *size);
int crapi_sha512_fd (int fd, void *dst, size_t *size);

#endif /* CRAPI_SHA2_H */
