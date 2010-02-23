#pragma once
#ifndef SEAP_COMMAND_H
#define SEAP_COMMAND_H

#include <stdint.h>
#include <stdarg.h>
#include <seap-debug.h>
#include <sexp-types.h>
#include <seap-types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef SEXP_t * (*SEAP_cmdfn_t) (SEXP_t *, void *);

typedef uint16_t SEAP_cmdcode_t;
typedef uint16_t SEAP_cmdid_t;
typedef uint8_t  SEAP_cmdtype_t;

#define SEAP_CMDTYPE_SYNC  1
#define SEAP_CMDTYPE_ASYNC 2

#define SEAP_CMDREG_LOCAL  0x00000001
#define SEAP_CMDREG_USEARG 0x00000002
#define SEAP_CMDREG_THREAD 0x00000004

int SEAP_cmd_register   (SEAP_CTX_t *ctx, SEAP_cmdcode_t code, uint32_t flags, SEAP_cmdfn_t func, ...);
int SEAP_cmd_unregister (SEAP_CTX_t *ctx, SEAP_cmdcode_t code);

SEXP_t *SEAP_cmd_exec (SEAP_CTX_t    *ctx,
                       int            sd,
                       uint32_t       flags,
                       SEAP_cmdcode_t code,
                       SEXP_t        *args,
                       SEAP_cmdtype_t type,
                       SEAP_cmdfn_t   func,
                       void          *funcarg);

#ifdef __cplusplus
}
#endif

#endif /* SEAP_COMMAND_H */
