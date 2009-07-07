#pragma once
#ifndef SEAP_COMMAND_H
#define SEAP_COMMAND_H

int SEAP_cmd_register (SEAP_CTX_t *ctx, uint32_t code, SEXP_t * (*cmdfn) (SEXP_t *, void *), void *arg);
int SEAP_cmd_unregister (SEAP_CTX_t *ctx, uint32_t code);

#define SEAP_CMDTYPE_SYNC  1
#define SEAP_CMDTYPE_ASYNC 2
#define SEAP_DESC_SELF    -1

SEXP_t *SEAP_cmd_exec (SEAP_CTX_t *ctx, int where, cmd_t cmd, SEXP_t *args,
                       cmd_type_t type, SEXP_t *(*func)(SEXP_t *, void *), void *);

/*
SEXP_t *SEAP_locexec_cmd (SEAP_CTX_t *ctx, uint32_t code, SEXP_t *args);
SEXP_t *SEAP_remexec_cmd (SEAP_CTX_t *ctx, uint32_t code, SEXP_t *args);
*/

#endif /* SEAP_COMMAND_H */
