#include <stdarg.h>
#include <string.h>
#include <errno.h>
#if defined(SEAP_THREAD_SAFE)
# include <pthread.h>
#endif

#include "generic/common.h"
#include "public/sm_alloc.h"
#include "_seap-types.h"
#include "_sexp-types.h"
#include "_sexp-manip.h"
#include "generic/redblack.h"
#include "_seap-command.h"

int SEAP_cmd_register (SEAP_CTX_t *ctx, SEAP_cmdcode_t code, uint32_t flags, SEAP_cmdfn_t func, ...) /* sd, arg */
{
        va_list ap;
        int     sd;
        void   *arg;
        
        SEAP_desc_t   *dsc;
        SEAP_cmdtbl_t *tbl;
        
        _A(ctx  != NULL);
        _A(func != NULL);

        sd  = -1;
        arg = NULL;

        va_start (ap, func);
                
        if (flags & SEAP_CMDREG_LOCAL) {
                sd = va_arg (ap, int);

                /* FIXME: not thread safe! */
                if (sd < 0 || sd >= ctx->sd_table.sdsize) {
                        errno = EBADF;
                        return (-1);
                }

                dsc = &(ctx->sd_table.sd[sd]);
                tbl = dsc->cmd_c_table;
                                
        } else {
                tbl = ctx->cmd_c_table;
        }
        
        _A(tbl != NULL);
        
        if (flags & SEAP_CMDREG_USEARG) {
                arg = va_arg (ap, void *);
                _A(arg != NULL);
        }

        
}


/*
int SEAP_cmd_register (SEXP_CTX_t *ctx, int flags, SEAP_cmdcode_t code, void *func, ...)
{
        return (-1);
}
*/

/* 
 *  SEAP_cmd_register (ctx, SEAP_CMDREG_USEARG, 1, cmdfn, sd, NULL);
 *  SEAP_cmd_register (ctx, SEAP_CMDREG_GLOBAL, 1, cmdfn);
 *  SEAP_cmd_register (ctx, 0, 1, cmdfn, sd);
 *  SEAP_cmd_register (ctx, SEAP_CMDREG_USEARG | SEAP_CMDREG_GLOBAL, 1, cmdfn, NULL);
 */ 

/*
int SEAP_cmd_register (SEAP_CTX_t *ctx, uint32_t code, SEXP_t * (*cmdfn)(SEXP_t *, void *), void *arg)
{
        return (-1);
}
*/

SEAP_cmdtbl_t *SEAP_cmdtbl_new (void)
{
        SEAP_cmdtbl_t *t;
        
        t = sm_talloc (SEAP_cmdtbl_t);

        t->flags = 0;
        t->table = NULL;
        t->maxcnt = 0;

#if defined(SEAP_THREAD_SAFE)
        if (pthread_rwlock_init (&t->lock, NULL) != 0) {
                _D("Can't initialize rwlock: %u, %s.\n",
                   errno, strerror (errno));
                sm_free (t);
                return (NULL);
        }
#endif
        return (t);
}

void SEAP_cmdtbl_free (SEAP_cmdtbl_t *t)
{
        _A(t != NULL);
        
        if (t->flags & SEAP_CMDTBL_LARGE)
                SEAP_cmdtbl_backendL_free (t);
        else
                SEAP_cmdtbl_backendS_free (t);
        
        return;
}

int SEAP_cmdtbl_setsize (SEAP_cmdtbl_t *t, size_t maxsz)
{
        _A(t != NULL);
        
        t->maxcnt = maxsz;
        return (0);
}

int SEAP_cmdtbl_setfl (SEAP_cmdtbl_t *t, uint8_t f);
int SEAP_cmdtbl_unsetfl (SEAP_cmdtbl_t *t, uint8_t f);

int SEAP_cmd_unregister (SEAP_CTX_t *ctx, SEAP_cmdcode_t code)
{
        return (-1);
}

static SEXP_t *__cmd2sexp (SEAP_cmd_t *cmd)
{
        SEXP_t *sexp;
        
        _A(cmd != NULL);
        
        sexp = SEXP_list_new ();
        SEXP_list_add (sexp,
                       SEXP_string_new (SEAP_SYM_CMD, strlen (SEAP_SYM_CMD)));
        
        SEXP_list_add (sexp,
                       SEXP_string_new (":id", 3));
        SEXP_list_add (sexp,
                       SEXP_number_newhu (cmd->id));
        
        if (cmd->flags & SEAP_CMDFLAG_REPLY) {
                SEXP_list_add (sexp,
                               SEXP_string_new (":reply_id", 9));
                SEXP_list_add (sexp,
                               SEXP_number_newhu (cmd->rid));
        }
        
        SEXP_list_add (sexp,
                       SEXP_string_new (":class", 6));
        switch (cmd->class) {
        case SEAP_CMDCLASS_USR:
                SEXP_list_add (sexp,
                               SEXP_string_new ("usr", 4));
                break;
        case SEAP_CMDCLASS_INT:
                SEXP_list_add (sexp,
                               SEXP_string_new ("int", 8));
                break;
        default:
                abort ();
        }
        
        SEXP_list_add (sexp,
                       SEXP_string_new (":type", 5));
        
        SEXP_list_add (sexp,
                       (cmd->flags & SEAP_CMDFLAG_SYNC ?
                        SEXP_string_new ("sync", 4) : SEXP_string_new ("async", 5)));
        
        SEXP_list_add (sexp,
                       SEXP_number_newhu (cmd->code));
        
        if (cmd->args != NULL)
                SEXP_list_add (sexp, cmd->args);
        
        SEXP_VALIDATE(sexp);
        return (sexp);
}

SEXP_t *SEAP_cmd_exec (SEAP_CTX_t    *ctx,
                       int            where,
                       SEAP_cmdcode_t cmd,
                       SEXP_t        *args,
                       SEAP_cmdtype_t type,
                       SEAP_cmdfn_t   func,
                       void          *funcarg)
{
        _A(ctx != NULL);
        
        if (where < 0) {
                /* local */
                
                /* lookup */
                /* exec */
                /* func */
                /* ret */
        } else {
                /* remote */
                
                switch (type) {
                case SEAP_CMDTYPE_SYNC:
                        /* construct message */
                        /* send */
                        /* recv */
                        /* func */
                        /* ret */
                        break;
                case SEAP_CMDTYPE_ASYNC:
                        /* construct message */
                        /* register func */
                        /* send */
                        /* ret */
                        break;
                default:
                        errno = EINVAL;
                        return (NULL);
                }
        }
}
