#include "seap-command.h"
#include "seap-command-backendL.h"

int  SEAP_cmdtbl_backendL_add (SEAP_cmdtbl_t *t, SEAP_cmdrec_t *r)
{
        return (-1);
}

int  SEAP_cmdtbl_backendL_ins (SEAP_cmdtbl_t *t, SEAP_cmdrec_t *r)
{
        return (-1);
}

int  SEAP_cmdtbl_backendL_del (SEAP_cmdtbl_t *t, SEAP_cmdrec_t *r)
{
        return (-1);
}

SEAP_cmdrec_t *SEAP_cmdtbl_backendL_get (SEAP_cmdtbl_t *t, SEAP_cmdcode_t c)
{
        return (NULL);
}

int  SEAP_cmdtbl_backendL_cmp (SEAP_cmdrec_t *a, SEAP_cmdrec_t *b)
{
        return (-1);
}

void SEAP_cmdtbl_backendL_free (SEAP_cmdtbl_t *t)
{
        return;
}

int SEAP_cmdtbl_backendL_apply (SEAP_cmdtbl_t *t, int (*func) (SEAP_cmdrec_t *r, void *), void *arg)
{
        return (-1);
}
