#pragma once
#ifndef CRAPI_SHA1_H
#define CRAPI_SHA1_H

#include <stddef.h>

int crapi_sha1_fd (int fd, void *dst, size_t *size);

#endif /* CRAPI_SHA1_H */
