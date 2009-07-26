#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <errno.h>
#include <config.h>
#include "public/sm_alloc.h"
#include "generic/common.h"
#include "_sexp-types.h"
#include "_sexp-parse.h"
#include "_seap-types.h"
#include "_seap-scheme.h"
#include "public/seap.h"

static void SEAP_CTX_initdefault (SEAP_CTX_t *ctx)
{
        _A(ctx != NULL);
        
        ctx->parser  = NULL /* PARSER(label) */;
        ctx->pflags  = PF_EOFOK;
        ctx->fmt_in  = SEXP_FMT_CANONICAL;
        ctx->fmt_out = SEXP_FMT_CANONICAL;

        /* Initialize descriptor table */
        ctx->sd_table.sd = NULL;
        ctx->sd_table.sdsize = 0;
        bitmap_init (&(ctx->sd_table.bitmap), SEAP_MAX_OPENDESC);
        
        ctx->cmd_c_table = SEAP_cmdtbl_new ();
        return;
}

SEAP_CTX_t *SEAP_CTX_new (void)
{
        SEAP_CTX_t *ctx;
        
        ctx = sm_talloc (SEAP_CTX_t);
        SEAP_CTX_initdefault (ctx);
        
        return (ctx);
}

void SEAP_CTX_init (SEAP_CTX_t *ctx)
{
        _A(ctx != NULL);
        SEAP_CTX_initdefault (ctx);
        return;
}

void SEAP_CTX_free (SEAP_CTX_t *ctx)
{
        _A(ctx != NULL);

        /* TODO: free sd_table */
        bitmap_free (&(ctx->sd_table.bitmap));
        SEAP_cmdtbl_free (ctx->cmd_c_table);
        sm_free (ctx);
        return;
}

int SEAP_desc_add (SEAP_desctable_t *sd_table, SEXP_pstate_t *pstate,
                         SEAP_scheme_t scheme, void *scheme_data)
{
        bitmap_bitn_t sd;
        
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
        return (sd_table->sdsize > sd ? &(sd_table->sd[sd]) : NULL);
}

int SEAP_connect (SEAP_CTX_t *ctx, const char *uri, uint32_t flags)
{
        SEAP_scheme_t scheme;
        size_t schstr_len = 0;
        int sd;

        while (uri[schstr_len] != ':') {
                if (uri[schstr_len] == '\0') {
                        errno = EINVAL;
                        return (-1);
                }
                ++schstr_len;
        }

        scheme = SEAP_scheme_search (__schtbl, uri, schstr_len);
        if (scheme == SCH_NONE) {
                /* scheme not found */
                errno = EPROTONOSUPPORT;
                return (-1);
        }

        if (uri[schstr_len + 1] == '/') {
                if (uri[schstr_len + 2] == '/') {
                        ++schstr_len;
                        ++schstr_len;
                } else {
                        errno = EINVAL;
                        return (-1);
                }
        } else {
                errno = EINVAL;
                return (-1);
        }
        
        sd = SEAP_desc_add (&(ctx->sd_table), NULL, scheme, NULL);
        if (sd < 0) {
                _D("Can't create/add new SEAP descriptor\n");
                return (-1);
        }

        if (SCH_CONNECT(scheme, &(ctx->sd_table.sd[sd]),
                        uri + schstr_len + 1, flags) != 0)
        {
                /* FIXME: delete SEAP descriptor */
                _D("SCH_CONNECT failed\n");
                return (-1);
        }
        
        return (sd);
}

int SEAP_open (SEAP_CTX_t *ctx, const char *path, uint32_t flags)
{
        errno = EOPNOTSUPP;
        return (-1);
}

int SEAP_openfd (SEAP_CTX_t *ctx, int fd, uint32_t flags)
{
        errno = EOPNOTSUPP;
        return (-1);
}

int SEAP_openfd2 (SEAP_CTX_t *ctx, int ifd, int ofd, uint32_t flags)
{
        int sd;

        sd = SEAP_desc_add (&(ctx->sd_table), NULL, SCH_GENERIC, NULL);
        if (sd < 0) {
                _D("Can't create/add new SEAP descriptor\n");
                return (-1);
        }

        if (SCH_OPENFD2(SCH_GENERIC, &(ctx->sd_table.sd[sd]),
                        ifd, ofd, flags) != 0)
        {
                _D("SCH_OPENFD2 failed\n");
                return (-1);
        }

        return (sd);
}

#if 0
int SEAP_openfp (SEAP_CTX_t *ctx, FILE *fp, uint32_t flags)
{
        errno = EOPNOTSUPP;
        return (-1);
}
#endif /* 0 */

int SEAP_recvsexp (SEAP_CTX_t *ctx, int sd, SEXP_t **sexp)
{
        SEAP_msg_t *msg = NULL;

        if (SEAP_recvmsg (ctx, sd, &msg) == 0) {
                *sexp = msg->sexp;
                msg->sexp = NULL;
                SEAP_msg_free (msg);
                
                return (0);
        } else {
                *sexp = NULL;
                return (-1);
        }
}

SEAP_msg_t *SEAP_msg_new (void)
{
        SEAP_msg_t *new;

        new = sm_talloc (SEAP_msg_t);
        new->id = 0;
        new->attrs = NULL;
        new->attrs_cnt = 0;
        new->sexp = NULL;
        
        return (new);
}

void SEAP_msg_free (SEAP_msg_t *msg)
{
        _A(msg != NULL);

        if (msg->attrs != NULL)
                sm_free (msg->attrs);
        
        sm_free (msg);
        return;
}

int SEAP_msg_set (SEAP_msg_t *msg, SEXP_t *sexp)
{
        msg->sexp = sexp;
        return (0);
}

SEXP_t *SEAP_msg_get (SEAP_msg_t *msg)
{
        return (msg->sexp);
}

static int __SEXP_recvmsg_process_msg (SEXP_t *sexp_msg, SEAP_msg_t **seap_msg)
{
        size_t msg_icnt, msg_i, attr_i;
        SEXP_t *attr_name, *attr_val;
        
        _A(sexp_msg != NULL);
        _A(seap_msg != NULL);
        
        (*seap_msg) = SEAP_msg_new ();
        msg_icnt = SEXP_list_length (sexp_msg);
        
        /* 
         * In the worst case there should be (msg_icnt - 2) - 1 attributes.
         * That is: The mandatory attribute "id" which has a value + other
         * attributes which could all be attributes without a value. The
         * last element of the message is the S-exp to be transfered.
         */
        (*seap_msg)->attrs_cnt = (msg_icnt - 2) - 1;
        (*seap_msg)->attrs = sm_alloc (sizeof (SEAP_attr_t) * ((*seap_msg)->attrs_cnt));
        
        /* BEG: attribute loop */
        for (msg_i = 2, attr_i = 0; msg_i <= (msg_icnt - 1); ++msg_i) {
        
                attr_name = SEXP_list_nth (sexp_msg, msg_i);
                if (attr_name == NULL) {
                        _D("Unexpected error: No S-exp (attr_name) at position %u in the message (%p).\n",
                           msg_i, sexp_msg);
                        
                        sm_free ((*seap_msg)->attrs);
                        sm_free ((*seap_msg));
                        
                        return (SEAP_EUNEXP);
                }
                 
                if (SEXP_strncmp (attr_name, ":", 1) == 0) {
                        /* with value */
                        
                        if (SEXP_strcmp (attr_name, ":id") == 0) {
                                
                                attr_val = SEXP_list_nth (sexp_msg, msg_i + 1);
                                if (attr_val == NULL) {
                                        _D("Unexpected error: \"%s\": No attribute value at position %u in the message (%p).\n",
                                           "id", msg_i + 1, sexp_msg);
                                        
                                        sm_free ((*seap_msg)->attrs);
                                        sm_free ((*seap_msg));
                                        
                                        return (SEAP_EUNEXP);
                                }

                                if (SEXP_number_get (attr_val, &((*seap_msg)->id),
                                                     NUM_UINT64) != 0)
                                {
                                        _D("\"id\": Invalid value or type: sexp=%p, type=%s.\n",
                                           attr_val, SEXP_strtype (attr_val));
                                        
                                        sm_free ((*seap_msg)->attrs);
                                        sm_free ((*seap_msg));
                                        
                                        return (SEAP_EINVAL);
                                }

                                /* Mark the origial list item as empty */
                                SEXP_SETTYPE(attr_val, ATOM_EMPTY);
                        } else {
                                
                                (*seap_msg)->attrs[attr_i].name = SEXP_string_subcstr (attr_name, 1, 0);
                                (*seap_msg)->attrs[attr_i].value = SEXP_list_nth_dup (sexp_msg, msg_i + 1);
                                
                                if ((*seap_msg)->attrs[attr_i].value == NULL) {
                                        _D("Unexpected error: \"%s\": No attribute value at position %u in the message (%p).\n",
                                           (*seap_msg)->attrs[attr_i].name, msg_i + 1, sexp_msg);
                                        
                                        sm_free ((*seap_msg)->attrs);
                                        sm_free ((*seap_msg));
                                        
                                        /* FIXME: free already processed attributes */

                                        return (SEAP_EINVAL);
                                }
                                
                                /* Mark the original list item as empty */
                                SEXP_SETTYPE(SEXP_list_nth (sexp_msg, msg_i + 1), ATOM_EMPTY);
                                
                                ++attr_i;
                        }
                        
                        ++msg_i;
                        ++msg_i;
                } else {
                        /* without value */
                        
                        (*seap_msg)->attrs[attr_i].name = SEXP_string_cstr (attr_name);
                        (*seap_msg)->attrs[attr_i].value = NULL;
                        
                        ++msg_i;
                        ++attr_i;
                }
        } /* END: attribute loop */
        
        (*seap_msg)->sexp = SEXP_dup (SEXP_list_last (sexp_msg));
        SEXP_SETTYPE(SEXP_list_last (sexp_msg), ATOM_EMPTY);
        
        return (0);
}

static int __SEXP_recvmsg_process_err (void)
{
        return (-1);
}

static void *__SEAP_cmdexec_worker (void *arg)
{
        SEAP_cmdjob_t *job;
        SEXP_t        *res;

        job = (SEAP_cmdjob_t *)arg;
        res = SEAP_cmd_exec (job->ctx, job->sd,
                             SEAP_EXEC_LOCAL,
                             job->cmd.code, job->cmd.args,
                             SEAP_CMDCLASS_USR, NULL, NULL);
        
        /* send */
        
        return (NULL);
}

static int __SEXP_recvmsg_process_cmd (SEAP_CTX_t *ctx, int sd, SEXP_t *cmd_sexp)
{
        SEAP_cmd_t cmd;
        SEXP_t *item, *val;
        size_t i, len;
        int mattrs;
        
        mattrs    = 0;
        cmd.id    = 0;
        cmd.rid   = 0;
        cmd.flags = 0;
        cmd.class = SEAP_CMDCLASS_INT;
        cmd.code  = 0;
        cmd.args  = NULL;

        i = 2;
        while ((item = SEXP_list_nth (cmd_sexp, i)) != NULL) {
                if (SEXP_strncmp (item, ":", 1) == 0) {
                        char *attr_name;
                        
                        attr_name = SEXP_string_cstr (item);
                        
                        switch (*(attr_name + 1)) {
                        case 'i':
                                if (strcmp (attr_name + 1, "id") == 0) {
                                        val = SEXP_list_nth (cmd_sexp, i + 1);
                                        
                                        if (val == NULL)
                                                break;
                                        
                                        if (SEXP_numberp (val)) {
                                                cmd.id = SEXP_number_gethu (val);
                                                ++mattrs;
                                        }
                                }
                                break;
                        case 'r':
                                if (strcmp (attr_name + 1, "reply_id") == 0) {
                                        val = SEXP_list_nth (cmd_sexp, i + 1);
                                        
                                        if (val == NULL)
                                                break;

                                        if (SEXP_numberp (val)) {
                                                cmd.rid = SEXP_number_gethu (val);
                                                cmd.flags |= SEAP_CMDFLAG_REPLY;
                                        }
                                }
                                break;
                        case 'c':
                                if (strcmp (attr_name + 1, "class") == 0) {
                                        val = SEXP_list_nth (cmd_sexp, i + 1);

                                        if (val == NULL)
                                                break;

                                        if (SEXP_stringp (val)) {
                                                if (SEXP_strcmp (val, "usr") == 0) {
                                                        cmd.class = SEAP_CMDCLASS_USR;
                                                }
                                        }
                                }
                                break;
                        case 't':
                                if (strcmp (attr_name + 1, "type") == 0) {
                                        val = SEXP_list_nth (cmd_sexp, i + 1);
                                        
                                        if (val == NULL)
                                                break;
                                        
                                        if (SEXP_stringp (val)) {
                                                if (SEXP_strcmp (val, "sync") == 0)
                                                        cmd.flags |= SEAP_CMDFLAG_SYNC;
                                        }
                                }
                                break;
                        }
                        
                        free (attr_name);
                        ++i; ++i;
                } else {
                        break;
                }
        }
        
        if (item == NULL || mattrs < 1) {
                errno = EINVAL;
                return (-1);
        }

        if (SEXP_numberp (item)) {
                cmd.code = SEXP_number_gethu (item);
        } else {
                errno = EINVAL;
                return (-1);
        }
        
        cmd.args = SEXP_list_nth (cmd_sexp, ++i);

        if (ctx->cflags & SEAP_CFLG_THREAD) {
                pthread_t      th;
                pthread_attr_t th_attrs;
                
                SEAP_cmdjob_t *job;
                
                /* Initialize thread stuff */
                pthread_attr_init (&th_attrs);
                pthread_attr_setdetachstate (&th_attrs, PTHREAD_CREATE_DETACHED);
                
                /* Prepare the job */
                job = SEAP_cmdjob_new ();
                job->ctx = ctx;
                job->sd  = sd;
                memcpy (&(job->cmd), &cmd, sizeof (SEAP_cmd_t));
                
                /* Create the worker */
                if (pthread_create (&th, &th_attrs,
                                    &__SEAP_cmdexec_worker, (void *)job) != 0)
                {
                        _D("Can't create worker thread: %u, %s.\n",
                           errno, strerror (errno));
                        
                        SEAP_cmdjob_free (job);
                        pthread_attr_destroy (&th_attrs);

                        return (-1);
                }
                
                pthread_attr_destroy (&th_attrs);
        } else {
                SEXP_t *res, *sexp;
                SEAP_desc_t  *dsc;
                
                if (cmd.flags & SEAP_CMDFLAG_REPLY) {
                        res = SEAP_cmd_exec (ctx, sd, SEAP_EXEC_WQUEUE,
                                             cmd.rid, cmd.args,
                                             SEAP_CMDCLASS_USR, NULL, NULL);
                } else {
                        res = SEAP_cmd_exec (ctx, sd, SEAP_EXEC_LOCAL,
                                             cmd.code, cmd.args,
                                             SEAP_CMDCLASS_USR, NULL, NULL);
                        
                        /* send */
                        dsc = &(ctx->sd_table.sd[sd]);
                        
                        cmd.rid = cmd.id;
#if defined(HAVE_ATOMIC_FUNCTIONS)
                        cmd.id = __sync_fetch_and_add (&(dsc->next_cid), 1);
#else
                        cmd.id = dsc->next_cid++;
#endif
                        cmd.flags |= SEAP_CMDFLAG_REPLY;
                        cmd.args = res;
                        
                        sexp = SEAP_cmd2sexp (&cmd);
                        
                        if (SCH_SENDSEXP(dsc->scheme, dsc, sexp, 0) < 0) {
                                _D("SCH_SENDSEXP: FAIL: %u, %s.\n",
                                   errno, strerror (errno));
                                SEXP_free (sexp);
                                return (-1);
                        }

                        SEXP_free (sexp);
                }
        }
        
        return (0);
}

int SEAP_recvmsg (SEAP_CTX_t *ctx, int sd, SEAP_msg_t **seap_msg)
{
        int ret = 0;
        SEAP_desc_t *desc;
        char  *buffer;
        size_t buflen;
        ssize_t recvlen;
        SEXP_t *sexp_msg;
        SEAP_msg_t *msg;
        SEXP_psetup_t psetup;
        
        _A(ctx      != NULL);
        _A(seap_msg != NULL);
        
        if (sd < 0 || sd >= ctx->sd_table.sdsize) {
                errno = EBADF;
                return (-1);
        } 

        desc = &(ctx->sd_table.sd[sd]);
        SEXP_psetup_init (&psetup);

        _A(desc != NULL);
        
        /*
         * Message loop
         */
        for (;;) {
                /* Read from socket only if the S-exp buffer is empty */
                if (desc->sexpbuf == NULL) {
                        /*
                         * Receive & Parse loop
                         */
                        for (;;) {
                                buffer = sm_alloc (sizeof (char) * SEAP_BUFFER_SIZE); /* TODO: make buffer size configurable */
                                buflen = SEAP_BUFFER_SIZE;
                                
                                /* Receive raw data */
                                if ((recvlen = SCH_RECV(desc->scheme, desc, buffer, buflen, 0)) <= 0) {
                                        /* check errno here (non-blocking mode?) */
                                        sm_free (buffer);
                                        return (-1);
                                }
                                
                                _D("received data -> recvlen=%zd\n", recvlen);
                                
                                /* Try to parse the buffer */
                                desc->sexpbuf = SEXP_parse (&psetup, buffer, recvlen, &(desc->pstate));
                                
                                /* If there is a complete S-exp stop the receive & parse loop */
                                if (desc->sexpbuf != NULL) {
                                        sexp_msg = SEXP_list_pop (&(desc->sexpbuf));
                                        break;
                                }
                        }
                } else {
                        sexp_msg = SEXP_list_pop (&(desc->sexpbuf));
                }
                
                /* The value of sexp_msg must be a valid S-exp object here */
                SEXP_VALIDATE(sexp_msg);
                
                { FILE *fp;

                        fp = fopen ("recv.log", "a");
                        setbuf (fp, NULL);
                        fprintf (fp, "--- RECV ---\n");
                        SEXP_fprintfa (fp, sexp_msg);
                        fprintf (fp, "\n-----------\n");
                        fclose (fp);
                }
                /*
                 * First sanity check:
                 * The received S-exp must be a list
                 */
                if (SEXP_TYPE(sexp_msg) != ATOM_LIST) {
                        _D("Invalid SEAP message received: %s\n", "not a list");
                        SEXP_free (sexp_msg);
                        continue;
                }
                
                /*
                 * Second sanity check: 
                 * First item of the list must be "seap.msg", "seap.err"
                 * or "seap.cmd"
                 */
                if (SEXP_strcmp (SEXP_list_first (sexp_msg), SEAP_SYM_MSG) == 0) {
                        /* process msg */
                        switch (ret = __SEXP_recvmsg_process_msg (sexp_msg, seap_msg)) {
                        case 0:
                                /*
                                 * Free the sexp version of the message
                                 * Note: Only the attribute names and the message
                                 * symbol are freed because the other items are
                                 * accessible in seap_msg. We can call
                                 * SEXP_list_free_nr here because the the needed
                                 * objects were "shallow" copyied and the original
                                 * list items were marked as empty.
                                 */
                                
                                SEXP_list_free_nr (sexp_msg);
                                return (0);
                        default:
                                return (ret);
                        }
                }

                if (SEXP_strcmp (SEXP_list_first (sexp_msg), SEAP_SYM_ERR) == 0) {
                        /* process err */
                        switch (ret = __SEXP_recvmsg_process_err ()) {
                        default:
                                errno = EDOOFUS;
                                return (-1);
                        }
                }
                
                if (SEXP_strcmp (SEXP_list_first (sexp_msg), SEAP_SYM_CMD) == 0) {
                        /* process cmd */
                        switch (ret = __SEXP_recvmsg_process_cmd (ctx, sd, sexp_msg)) {
                        case  0:
                                SEXP_free (sexp_msg);
                                continue;
                        default:
                                errno = EDOOFUS;
                                return (-1);
                        }
                }
                
                /* Invalid SEAP message */
                _D("Invalid SEAP message received: %s\n", "Unknown message type");
                SEXP_free (sexp_msg);
                continue;
        }
        
        /* NOTREACHED */
        errno = EDOOFUS;
        return (-1);
}

int SEAP_sendsexp (SEAP_CTX_t *ctx, int sd, SEXP_t *sexp)
{
        SEAP_msg_t *msg;
        int ret;

        msg = SEAP_msg_new ();
        msg->sexp = sexp;
        ret = SEAP_sendmsg (ctx, sd, msg);
        SEAP_msg_free (msg);
        
        return (ret);
}

SEXP_t *__SEAP_msg2sexp (SEAP_msg_t *msg)
{
        SEXP_t *sexp;
        char  *attr_name;
        size_t attr_namelen;
        uint16_t i;
        
        _A(msg != NULL);
        
        sexp = SEXP_list_new ();
        SEXP_list_add (sexp, SEXP_string_new (SEAP_SYM_MSG, strlen (SEAP_SYM_MSG)));
        
        /* Add message id */
        SEXP_list_add (sexp, SEXP_string_new (":id", 3));
        SEXP_list_add (sexp, SEXP_number_newllu (msg->id));
        
        /* Add message attributes */
        for (i = 0; i < msg->attrs_cnt; ++i) {
                if (msg->attrs[i].value != NULL) {
                        attr_namelen = strlen (msg->attrs[i].name) + 2;
                        attr_name    = sm_alloc (sizeof (char) * attr_namelen);
                        
                        snprintf (attr_name, attr_namelen, ":%s", msg->attrs[i].name);
                        
                        SEXP_list_add (sexp, SEXP_string_new (attr_name, attr_namelen - 1));
                        SEXP_list_add (sexp, msg->attrs[i].value);
                        
                        sm_free (attr_name);
                } else {
                        SEXP_list_add (sexp, SEXP_string_new (msg->attrs[i].name,
                                                              strlen (msg->attrs[i].name)));
                }
        }
        
        /* Add data */
        SEXP_list_add (sexp, msg->sexp);

        { FILE *fp;
                
                fp = fopen ("sexp.log", "a");
                setbuf (fp, NULL);
                fprintf (fp, "--- SEXP ---\n");
                SEXP_fprintfa (fp, sexp);
                fprintf (fp, "\n-----------\n");
                fclose (fp);
        }
        
        return (sexp);
}

int SEAP_msgattr_set (SEAP_msg_t *msg, const char *attr, SEXP_t *value)
{
        _A(msg != NULL);
        _A(attr != NULL);

        SEXP_VALIDATE(value);

        msg->attrs = sm_realloc (msg->attrs, sizeof (SEAP_attr_t) * (++msg->attrs_cnt));
        msg->attrs[msg->attrs_cnt - 1].name  = strdup (attr);
        msg->attrs[msg->attrs_cnt - 1].value = value;
        
        return (0);
}

SEXP_t *SEAP_msgattr_get (SEAP_msg_t *msg, const char *name)
{
        SEXP_t *value = NULL;
        uint16_t i;

        _A(msg  != NULL);
        _A(name != NULL);

        /* FIXME: this is stupid */
        for (i = 0; i < msg->attrs_cnt; ++i) {
                if (strcmp (name, msg->attrs[i].name) == 0)
                        return (msg->attrs[i].value);
        }
        
        return (NULL);
}

int SEAP_sendmsg (SEAP_CTX_t *ctx, int sd, SEAP_msg_t *seap_msg)
{
        SEAP_desc_t *desc;
        SEXP_t *sexp_msg;
        uint32_t msg_id;

        _A(ctx != NULL);
        _A(seap_msg != NULL);
        
        if (sd >= 0 && sd < ctx->sd_table.sdsize) {
                desc = &(ctx->sd_table.sd[sd]);

                /* _A(desc->scheme < (sizeof __schtbl / sizeof (SEAP_schemefn_t))); */
        
                /*
                 * Atomicaly fill the id field.
                 */
                
#if defined(HAVE_ATOMIC_FUNCTIONS)
                seap_msg->id = __sync_fetch_and_add (&(desc->next_id), 1);
#else
                seap_msg->id = desc->next_id++;
#endif           
                
                /* Convert seap_msg into its S-exp representation */
                sexp_msg = __SEAP_msg2sexp (seap_msg);
                if (sexp_msg == NULL) {
                        _D("Can't convert message into S-exp: %u, %s.\n",
                           errno, strerror (errno));
                        return (-1);
                }
                
                puts ("--- MSG ---");
                SEXP_printfa (sexp_msg);
                puts ("\n--- MSG ---");
                
                /*
                 * Send the message using handler associated
                 * with the descriptor.
                 */
                if (SCH_SENDSEXP(desc->scheme, desc, sexp_msg, 0) < 0) {
                        /* FIXME: Free sexp_msg */
                        return (-1);
                }
                
                /* check if everything was sent */
                if (desc->ostate != NULL) {
                        errno = EINPROGRESS;
                        return (-1);
                }
                
                return (0);
        } else {
                errno = EBADF;
                return (-1);
        }
}

int SEAP_reply (SEAP_CTX_t *ctx, int sd, SEAP_msg_t *rep_msg, SEAP_msg_t *req_msg)
{
        _A(ctx != NULL);
        _A(rep_msg != NULL);
        _A(req_msg != NULL);
        
        SEAP_msgattr_set (rep_msg, "reply-id", SEXP_number_newllu (req_msg->id));
        
        return SEAP_sendmsg (ctx, sd, rep_msg);
}

SEXP_t *__SEAP_err2sexp (SEAP_err_t *err, unsigned int type)
{
        SEXP_t *sexp;

        sexp = SEXP_list_new ();
        SEXP_list_add (sexp, SEXP_string_new (SEAP_SYM_ERR, strlen (SEAP_SYM_ERR)));
        
        /* Add original message id */
        SEXP_list_add (sexp, SEXP_string_new (":orig_id", 8));
        SEXP_list_add (sexp, SEXP_number_newllu (err->id));
        
        /* Add error code type */
        SEXP_list_add (sexp, SEXP_string_new (":type", 5));
        SEXP_list_add (sexp, SEXP_number_newu (type));

        /* Add error code */
        SEXP_list_add (sexp, SEXP_number_newu (err->code));
        
        if (err->data != NULL) {
                SEXP_list_add (sexp, err->data);
        }
        
        return (sexp);
}

int __SEAP_senderr (SEAP_CTX_t *ctx, int sd, SEAP_err_t *err, unsigned int type)
{
        SEAP_desc_t *desc;
        SEXP_t *sexp_err;
        
        _A(ctx != NULL);
        _A(err != NULL);

        _A(type == SEAP_ETYPE_USER || type == SEAP_ETYPE_INT);
        
        if (sd < 0 || sd >= ctx->sd_table.sdsize) {
                errno = EBADF;
                return (-1);
        }

        desc = &(ctx->sd_table.sd[sd]);

        /* Convert the err structure into its S-exp representation */
        sexp_err = __SEAP_err2sexp (err, type);
        if (sexp_err == NULL) {
                _D("Can't convert the err structure into S-exp: %u, %s.\n",
                   errno, strerror (errno));
                return (-1);
        }
        
        /*
         * Send the error using handler associated
         * with the descriptor.
         */

        if (SCH_SENDSEXP(desc->scheme, desc, sexp_err, 0) < 0) {
                /* FIXME: Don't free the attached message */
                SEXP_free (sexp_err);
                return (-1);
        }

        /* Check if everything was sent */
        if (desc->ostate != NULL) {
                errno = EINPROGRESS;
                return (-1);
        }
        
        return (0);
}

int SEAP_senderr (SEAP_CTX_t *ctx, int sd, SEAP_err_t *err)
{
        return (__SEAP_senderr (ctx, sd, err, SEAP_ETYPE_USER));
}

int SEAP_replyerr (SEAP_CTX_t *ctx, int sd, SEAP_msg_t *rep_msg, uint32_t e)
{
        SEAP_err_t err;
        
        _A(ctx != NULL);
        _A(rep_msg != NULL);
        
        err.code = e;
        err.id   = rep_msg->id;
        err.data = NULL; /* FIXME: Attach original message */
        
        return (__SEAP_senderr (ctx, sd, &err, SEAP_ETYPE_USER));
}

int SEAP_recverr (SEAP_CTX_t *ctx, int sd, SEAP_err_t **err)
{
        return (-1);
}

int SEAP_recverr_byid (SEAP_CTX_t *ctx, int sd, SEAP_err_t **err, SEAP_msgid_t id)
{
        return (-1);
}

SEXP_t *SEAP_read (SEAP_CTX_t *ctx, int sd)
{
        errno = EOPNOTSUPP;
        return (NULL);
}

int SEAP_write (SEAP_CTX_t *ctx, int sd, SEXP_t *sexp)
{
        errno = EOPNOTSUPP;
        return (-1);
}

int SEAP_close (SEAP_CTX_t *ctx, int sd)
{
        SEAP_desc_t *desc;
        int ret = 0;
        
        _A(ctx != NULL);
        
        if (sd > 0) {
                desc = &(ctx->sd_table.sd[sd]);
                /* _A(desc->scheme < (sizeof __schtbl / sizeof (SEAP_schemefn_t))); */
                
                ret = SCH_CLOSE(desc->scheme, desc, 0); /* TODO: Are flags usable here? */
                
                if (SEAP_desc_del (&(ctx->sd_table), sd) != 0) {
                        /* something very bad happened */
                        _D("SEAP_desc_del failed\n");
                        if (ret > 0)
                                ret = -1;
                }
                
                return (ret);
        } else {
                _D("Negative SEAP descriptor\n");
                errno = EBADF;
                return (-1);
        }
}
