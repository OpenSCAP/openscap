#include <pthread.h>
#include "generic/bitmap.h"
#include "_sexp-parse.h"
#include "_seap-scheme.h"
#include "seap-descriptor.h"

int SEAP_desc_add (SEAP_desctable_t *sd_table, SEXP_pstate_t *pstate,
                         SEAP_scheme_t scheme, void *scheme_data)
{
        bitmap_bitn_t sd;
        pthread_mutexattr_t mutex_attr;
        
        sd = bitmap_setfree (&(sd_table->bitmap));
        
        if (sd >= 0) {
                if (sd >= sd_table->sdsize) {
                        /* sd araay is to small -> realloc */
                        sd_table->sdsize = sd + SDTABLE_REALLOC_ADD;
                        sd_table->sd = sm_realloc (sd_table->sd, sizeof (SEAP_desc_t) * sd_table->sdsize);
                }

                sd_table->sd[sd].next_id = 0;
                sd_table->sd[sd].sexpbuf = NULL;
                /* sd_table->sd[sd].sexpcnt = 0; */
                sd_table->sd[sd].pstate  = pstate;
                sd_table->sd[sd].scheme  = scheme;
                sd_table->sd[sd].scheme_data = scheme_data;
                sd_table->sd[sd].ostate  = NULL;
                sd_table->sd[sd].next_cid = 0;
                sd_table->sd[sd].cmd_c_table = SEAP_cmdtbl_new ();
                sd_table->sd[sd].cmd_w_table = SEAP_cmdtbl_new ();
                sd_table->sd[sd].pck_queue   = pqueue_new (1024); /* FIXME */
                
                pthread_mutexattr_init (&mutex_attr);
                pthread_mutexattr_settype (&mutex_attr, PTHREAD_MUTEX_RECURSIVE);
                
                pthread_mutex_init (&(sd_table->sd[sd].r_lock), &mutex_attr);
                pthread_mutex_init (&(sd_table->sd[sd].w_lock), &mutex_attr);
                
                pthread_mutexattr_destroy (&mutex_attr);
                
                return ((int)sd);
        }

        return (-1);
}

int SEAP_desc_del (SEAP_desctable_t *sd_table, int sd)
{
        
        return (0);
}

SEAP_desc_t *SEAP_desc_get (SEAP_desctable_t *sd_table, int sd)
{
        if (sd < 0 || sd > sd_table->sdsize) {
                errno = EBADF;
                return (NULL);
        }

        return (&(sd_table->sd[sd]));
}

SEAP_msgid_t SEAP_desc_genmsgid (SEAP_desctable_t *sd_table, int sd)
{
        SEAP_desc_t *dsc;
        SEAP_msgid_t  id;
        dsc = SEAP_desc_get (sd_table, sd);
        
        if (dsc == NULL) {
                errno = EINVAL;
                return (-1);
        }
#if defined(HAVE_ATOMIC_FUNCTIONS)
        id = __sync_fetch_and_add (&(dsc->next_id), 1);
#else
        id = dsc->next_id++;
#endif
        return (id);
}

SEAP_cmdid_t SEAP_desc_gencmdid (SEAP_desctable_t *sd_table, int sd)
{
        SEAP_desc_t *dsc;
        SEAP_cmdid_t  id;

        dsc = SEAP_desc_get (sd_table, sd);
        
        if (dsc == NULL) {
                errno = EINVAL;
                return (-1);
        }
#if defined(HAVE_ATOMIC_FUNCTIONS)
        id = __sync_fetch_and_add (&(dsc->next_cid), 1);
#else
        id = dsc->next_cid++;
#endif
        return (id);
}
