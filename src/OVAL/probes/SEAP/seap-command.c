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
#include "_seap-scheme.h"
#include "generic/redblack.h"
#include "_seap-command.h"

int SEAP_cmd_register (SEAP_CTX_t *ctx, SEAP_cmdcode_t code, uint32_t flags, SEAP_cmdfn_t func, ...) /* sd, arg */
{
        va_list ap;
        int     sd;
        void   *arg;
        
        SEAP_desc_t   *dsc;
        SEAP_cmdtbl_t *tbl;
        SEAP_cmdrec_t *rec;

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

        rec = SEAP_cmdrec_new ();
        rec->code = code;
        rec->func = func;
        rec->arg  = arg;

        switch (SEAP_cmdtbl_add (tbl, rec)) {
        case 0:
                /* rec is freed by SEAP_cmdtbl_add */
                break;
        case SEAP_CMDTBL_ECOLL:
                _D("Can't register command: code=%u, tbl=%p: already registered.\n",
                   code, (void *)tbl);
                SEAP_cmdrec_free (rec);
                return (-1);
        case -1:
                _D("Can't register command: code=%u, func=%p, tbl=%p, arg=%p: errno=%u, %s.\n",
                   code, (void *)func, (void *)tbl, arg, errno, strerror (errno));
                SEAP_cmdrec_free (rec);
                return (-1);
        default:
                SEAP_cmdrec_free (rec);
                errno = EDOOFUS;
                return (-1);
        }
        
        return (0);
}

/* 
 *  SEAP_cmd_register (ctx, SEAP_CMDREG_USEARG, 1, cmdfn, sd, NULL);
 *  SEAP_cmd_register (ctx, SEAP_CMDREG_GLOBAL, 1, cmdfn);
 *  SEAP_cmd_register (ctx, 0, 1, cmdfn, sd);
 *  SEAP_cmd_register (ctx, SEAP_CMDREG_USEARG | SEAP_CMDREG_GLOBAL, 1, cmdfn, NULL);
 */ 

SEAP_cmdrec_t *SEAP_cmdrec_new (void)
{
        SEAP_cmdrec_t *r;

        r = sm_talloc (SEAP_cmdrec_t);
        r->code = 0;
        r->func = NULL;
        r->arg  = NULL;

        return (r);
}

void SEAP_cmdrec_free (SEAP_cmdrec_t *r)
{
        sm_free (r);
}

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

int SEAP_cmdtbl_setfl (SEAP_cmdtbl_t *t, uint8_t f)
{
        _A(t != NULL);
        t->flags |= f;
        return (0);
}

int SEAP_cmdtbl_unsetfl (SEAP_cmdtbl_t *t, uint8_t f)
{
        _A(t != NULL);
        t->flags &= ~(f);
        return (0);
}

int SEAP_cmdtbl_add (SEAP_cmdtbl_t *t, SEAP_cmdrec_t *r)
{
        _A(t != NULL);
        _A(t != NULL);
        return (SEAP_CMDTBL_LARGE & t->flags ?
                SEAP_cmdtbl_backendL_add (t, r) :
                SEAP_cmdtbl_backendS_add (t, r));
}

int SEAP_cmdtbl_ins (SEAP_cmdtbl_t *t, SEAP_cmdrec_t *r)
{
        _A(t != NULL);
        _A(t != NULL);
        return (SEAP_CMDTBL_LARGE & t->flags ?
                SEAP_cmdtbl_backendL_ins (t, r) :
                SEAP_cmdtbl_backendS_ins (t, r));
}

int SEAP_cmdtbl_del (SEAP_cmdtbl_t *t, SEAP_cmdrec_t *r)
{
        _A(t != NULL);
        _A(t != NULL);
        return (SEAP_CMDTBL_LARGE & t->flags ?
                SEAP_cmdtbl_backendL_del (t, r) :
                SEAP_cmdtbl_backendS_del (t, r));
}

SEAP_cmdrec_t *SEAP_cmdtbl_get (SEAP_cmdtbl_t *t, SEAP_cmdcode_t c)
{
        _A(t != NULL);
        return (SEAP_CMDTBL_LARGE & t->flags ?
                SEAP_cmdtbl_backendL_get (t, c) :
                SEAP_cmdtbl_backendS_get (t, c));
}

int SEAP_cmdtbl_cmp (SEAP_cmdrec_t *a, SEAP_cmdrec_t *b)
{
        _A(a != NULL);
        _A(b != NULL);
        return (int)(a->code - b->code);
}

int SEAP_cmd_unregister (SEAP_CTX_t *ctx, SEAP_cmdcode_t code)
{
        return (-1);
}

SEXP_t *SEAP_cmd2sexp (SEAP_cmd_t *cmd)
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
                               SEXP_string_new ("usr", 3));
                break;
        case SEAP_CMDCLASS_INT:
                SEXP_list_add (sexp,
                               SEXP_string_new ("int", 3));
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
                       int            sd,
                       uint32_t       flags,
                       SEAP_cmdcode_t code,
                       SEXP_t        *args,
                       SEAP_cmdtype_t type,
                       SEAP_cmdfn_t   func,
                       void          *funcarg)
{
        SEAP_desc_t   *dsc;
        SEAP_cmdrec_t *rec;
        SEAP_cmdtbl_t *tbl[2];
        SEXP_t        *res;
        int8_t i;
        
        _A(ctx != NULL);
#if !defined(NDEBUG) || defined(VALIDATE_SEXP)
        if (args != NULL) {
                SEXP_VALIDATE(args);
        }
#endif

        if (sd < 0 || sd >= ctx->sd_table.sdsize) {
                errno = EBADF;
                return (NULL);
        }
        
        _D("code=%u, args=%p\n", code, args);

        /* FIXME: not thread safe! */
        dsc = &(ctx->sd_table.sd[sd]);
        
        if (flags & (SEAP_EXEC_LOCAL | SEAP_EXEC_WQUEUE)) {
                _D("EXEC_LOCAL\n");
                
                /* get table pointers */
                if (flags & SEAP_EXEC_WQUEUE) {
                        i = 0;
                        tbl[0] = dsc->cmd_w_table;
                } else if (flags & SEAP_EXEC_LONLY) {
                        i = 0;
                        tbl[0] = dsc->cmd_c_table;
                } else {
                        i = 1;
                        
                        if (flags & SEAP_EXEC_GFIRST) {
                                tbl[1] = ctx->cmd_c_table;
                                tbl[0] = dsc->cmd_c_table;
                        } else {
                                tbl[1] = dsc->cmd_c_table;
                                tbl[0] = ctx->cmd_c_table;
                        }
                }
                
                /* lookup command */
                rec = NULL;
                
                for (; i >= 0; --i)
                        if ((rec = SEAP_cmdtbl_get (tbl[i], code)) != NULL)
                                break;
                
                _D("rec=%p\n", rec);
                
                if (rec == NULL)
                        return (NULL);
                
                /* execute command */
                res = rec->func (args, rec->arg);
                
                _D("res=%p\n", res);

                /* filter result */
                if (func != NULL)
                        res = func (res, funcarg);
                
                _D("func@%p(res)=%p\n", func, res);
                
                return (res);
        } else {
                _D("EXEC_REMOTE\n");
                
                /* remote */  
                SEAP_cmd_t cmd;
                SEXP_t    *cmd_sexp;

                /* construct message */
#if defined(HAVE_ATOMIC_FUNCTIONS)
                cmd.id = __sync_fetch_and_add (&(dsc->next_cid), 1);
#else
                cmd.id = dsc->next_cid++;
#endif
                cmd.rid   = 0;
                cmd.flags = 0;
                cmd.class = SEAP_CMDCLASS_USR;
                cmd.code  = code;
                cmd.args  = args;
                                
                switch (type) {
                case SEAP_CMDTYPE_SYNC:
                        cmd.flags |= SEAP_CMDFLAG_SYNC;
                        
                        /* translate to S-exp */
                        cmd_sexp  = SEAP_cmd2sexp (&cmd);
                        
                        /* TODO */
                        errno = EOPNOTSUPP;
                        return (NULL);
                        break;
                case SEAP_CMDTYPE_ASYNC: {
                        cmd.flags |= SEAP_CMDFLAG_ASYNC;
                        
                        /* translate to S-exp */
                        cmd_sexp  = SEAP_cmd2sexp (&cmd);
                                                
                        /* register func */
                        rec = SEAP_cmdrec_new ();
                        rec->code = cmd.id;
                        rec->func = func;
                        rec->arg  = funcarg;

                        switch (SEAP_cmdtbl_add (dsc->cmd_w_table, rec)) {
                        case 0:
                                break;
                        case SEAP_CMDTBL_ECOLL:
                                _D("Can't register async command handler: id=%u, tbl=%p, sd=%u: already registered.\n",
                                   rec->code, (void *)dsc->cmd_w_table, sd);
                                SEAP_cmdrec_free (rec);
                                return (NULL);
                        case -1:
                                _D("Can't register async command handler: id=%u, tbl=%p, sd=%u: errno=%u, %s.\n",
                                   rec->code, (void *)dsc->cmd_w_table, sd, errno, strerror (errno));
                                SEAP_cmdrec_free (rec);
                                return (NULL);
                        default:
                                SEAP_cmdrec_free (rec);
                                errno = EDOOFUS;
                                return (NULL);
                        }
                        
                        /* send */
                        res = args;
                        
                        if (SCH_SENDSEXP(dsc->scheme, dsc, cmd_sexp, 0) < 0) {
                                _D("SCH_SENDSEXP: FAIL: %u, %s.\n",
                                   errno, strerror (errno));
                                res = NULL;
                        }
                }
                        return (res);
                default:
                        errno = EINVAL;
                        return (NULL);
                }
        }

        errno = EDOOFUS;
        return (NULL);
}

SEAP_cmdjob_t *SEAP_cmdjob_new (void)
{
        SEAP_cmdjob_t *j;

        j = sm_talloc (SEAP_cmdjob_t);
        j->ctx = NULL;
        j->sd  = -1;
        
        return (j);
}

void SEAP_cmdjob_free (SEAP_cmdjob_t *j)
{
        sm_free (j);
}
