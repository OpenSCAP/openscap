#pragma once
#ifndef SEAP_COMMAND_BACKENDL_H
#define SEAP_COMMAND_BACKENDL_H

#include "_seap-command.h"

int  SEAP_cmdtbl_backendL_add (SEAP_cmdtbl_t *t, SEAP_cmdrec_t *r);
int  SEAP_cmdtbl_backendL_ins (SEAP_cmdtbl_t *t, SEAP_cmdrec_t *r);
int  SEAP_cmdtbl_backendL_del (SEAP_cmdtbl_t *t, SEAP_cmdrec_t *r);
int  SEAP_cmdtbl_backendL_cmp (SEAP_cmdrec_t *a, SEAP_cmdrec_t *b);
void SEAP_cmdtbl_backendL_free (SEAP_cmdtbl_t *t);

#endif /* SEAP_COMMAND_BACKEND_H */
