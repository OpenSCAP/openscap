#pragma once
#ifndef SEAP_COMMAND_BACKENDS
#define SEAP_COMMAND_BACKENDS

int  SEAP_cmdtbl_backendS_add (SEAP_cmdtbl_t *t, SEAP_cmdrec_t *r);
int  SEAP_cmdtbl_backendS_ins (SEAP_cmdtbl_t *t, SEAP_cmdrec_t *r);
int  SEAP_cmdtbl_backendS_del (SEAP_cmdtbl_t *t, SEAP_cmdrec_t *r);
int  SEAP_cmdtbl_backendS_cmp (SEAP_cmdrec_t *a, SEAP_cmdrec_t *b);
void SEAP_cmdtbl_backendS_free (SEAP_cmdtbl_t *t);

#endif /* SEAP_COMMAND_BACKENDS */
