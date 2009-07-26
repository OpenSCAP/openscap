#pragma once
#ifndef SEAP_COMMAND_BACKENDS
#define SEAP_COMMAND_BACKENDS

#define SEAP_COMMAND_BACKENDS_MAXCAPACITY 769

int  SEAP_cmdtbl_backendS_add (SEAP_cmdtbl_t *t, SEAP_cmdrec_t *r);
int  SEAP_cmdtbl_backendS_ins (SEAP_cmdtbl_t *t, SEAP_cmdrec_t *r);
int  SEAP_cmdtbl_backendS_del (SEAP_cmdtbl_t *t, SEAP_cmdrec_t *r);
SEAP_cmdrec_t *SEAP_cmdtbl_backendS_get (SEAP_cmdtbl_t *t, SEAP_cmdcode_t c);
int  SEAP_cmdtbl_backendS_cmp (SEAP_cmdrec_t *a, SEAP_cmdrec_t *b);
void SEAP_cmdtbl_backendS_free (SEAP_cmdtbl_t *t);
int  SEAP_cmdtbl_backendS_apply (SEAP_cmdtbl_t *t, int (*func) (SEAP_cmdrec_t *r, void *), void *arg);

#endif /* SEAP_COMMAND_BACKENDS */
