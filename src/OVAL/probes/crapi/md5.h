#pragma once
#ifndef CRAPI_MD5_H
#define CRAPI_MD5_H

#include <stddef.h>

void *crapi_md5_init (void *dst, void *size);
int   crapi_md5_update (void *ctxp, void *bptr, size_t blen);
int   crapi_md5_fini (void *ctxp);
void  crapi_md5_free (void *ctxp);

int crapi_md5_fd (int fd, void *dst, size_t *size);

#endif /* CRAPI_MD5_H */
