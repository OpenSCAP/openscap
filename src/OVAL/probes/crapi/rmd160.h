#pragma once
#ifndef CRAPI_RMD160_H
#define CRAPI_RMD160_H

#include <stddef.h>

void *crapi_rmd160_init (void *dst, void *size);
int   crapi_rmd160_update (void *ctxp, void *bptr, size_t blen);
int   crapi_rmd160_fini (void *ctxp);
void  crapi_rmd160_free (void *ctxp);

int crapi_rmd160_fd (int fd, void *dst, size_t *size);

#endif /* CRAPI_RMD160_H */
