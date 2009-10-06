#pragma once
#ifndef SEAP_COMMAND_BACKENDL_H
#define SEAP_COMMAND_BACKENDL_H

#include "_seap-command.h"
#include "../../../common/util.h"

OSCAP_HIDDEN_START;

int  SEAP_cmdtbl_backendL_add (SEAP_cmdtbl_t *t, SEAP_cmdrec_t *r);
int  SEAP_cmdtbl_backendL_ins (SEAP_cmdtbl_t *t, SEAP_cmdrec_t *r);
int  SEAP_cmdtbl_backendL_del (SEAP_cmdtbl_t *t, SEAP_cmdrec_t *r);
SEAP_cmdrec_t *SEAP_cmdtbl_backendL_get (SEAP_cmdtbl_t *t, SEAP_cmdcode_t c);
int  SEAP_cmdtbl_backendL_cmp (SEAP_cmdrec_t *a, SEAP_cmdrec_t *b);
void SEAP_cmdtbl_backendL_free (SEAP_cmdtbl_t *t);
int  SEAP_cmdtbl_backendL_apply (SEAP_cmdtbl_t *t, int (*func) (SEAP_cmdrec_t *r, void *), void *arg);

OSCAP_HIDDEN_END;

#endif /* SEAP_COMMAND_BACKEND_H */
