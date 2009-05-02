#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <config.h>
#include "xmalloc.h"
#include "common.h"
#include "seap.h"

static void SEAP_CTX_initdefault (SEAP_CTX_t *ctx)
{
        _A(ctx != NULL);
        
        ctx->parser  = PARSER(label);
        ctx->pflags  = PF_EOFOK;
        ctx->fmt_in  = FMT_CANONICAL;
        ctx->fmt_out = FMT_CANONICAL;

        /* Initialize descriptor table */
        ctx->sd_table.sd = NULL;
        ctx->sd_table.sdsize = 0;
        bitmap_init (&(ctx->sd_table.bitmap), SEAP_MAX_OPENDESC);
        return;
}

SEAP_CTX_t *SEAP_CTX_new (void)
{
        SEAP_CTX_t *ctx;
        
        ctx = xmalloc (sizeof (SEAP_CTX_t));
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
        xfree ((void **)&ctx);
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
                        sd_table->sd = xrealloc (sd_table->sd, sizeof (SEAP_desc_t) * sd_table->sdsize);
                }

                sd_table->sd[sd].next_id = 0;
                sd_table->sd[sd].sexpbuf = NULL;
                /* sd_table->sd[sd].sexpcnt = 0; */
                sd_table->sd[sd].pstate  = pstate;
                sd_table->sd[sd].scheme  = scheme;
                sd_table->sd[sd].scheme_data = scheme_data;
                sd_table->sd[sd].ostate  = NULL;

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
                /* TODO: free msg obj */
                *sexp = msg->sexp;
                return (0);
        } else {
                *sexp = NULL;
                return (-1);
        }
}

SEAP_msg_t *SEAP_msg_new (void)
{
        SEAP_msg_t *new;

        new = xmalloc (sizeof (SEAP_msg_t));
        new->id = 0;
        new->attrs = NULL;
        new->attrs_cnt = 0;
        new->sexp = NULL;
        
        return (new);
}

SEAP_msg_t *SEAP_msg_free (SEAP_msg_t *msg)
{
        _A(msg != NULL);

        if (msg->attrs != NULL)
                xfree ((void **)&(msg->attrs));
        
        xfree ((void **)&msg);
        
        return (NULL);
}

int SEAP_recvmsg (SEAP_CTX_t *ctx, int sd, SEAP_msg_t **seap_msg)
{
        SEAP_desc_t *desc;
        char  *buffer;
        size_t buflen, recvlen;
        SEXP_t *sexp_msg;
        SEAP_msg_t *msg;
        SEXP_psetup_t psetup;
        
        if (sd >= 0 && sd < ctx->sd_table.sdsize) {
                desc = &(ctx->sd_table.sd[sd]);
                
                /* _A(desc->scheme < (sizeof __schtbl / sizeof (SEAP_schemefn_t))); */
                
                SEXP_psetup_init (&psetup);
                
                /* message loop */
                for (;;) {
                        if (desc->sexpbuf == NULL) {
                                /* receive & parse loop */
                                for (;;) {
                                        buffer = xmalloc (sizeof (char) * SEAP_BUFFER_SIZE); /* TODO: make buffer size configurable */
                                        buflen = SEAP_BUFFER_SIZE;
                                        
                                        /* Receive raw data */
                                        if ((recvlen = SCH_RECV(desc->scheme, desc, buffer, buflen, 0)) <= 0) {
                                                /* check errno here (non-blocking mode?) */
                                                xfree ((void **)&buffer);
                                                return (-1);
                                        }
                                 
                                        _D("received data -> recvlen=%u\n", recvlen);
       
                                        /* Try to parse the buffer */
                                        desc->sexpbuf = SEXP_parse (&psetup, buffer, recvlen, &(desc->pstate));
                                        
                                        if (desc->sexpbuf != NULL) {
                                                sexp_msg = SEXP_list_pop (&(desc->sexpbuf));
                                                break;
                                        }
                                }
                        } else {
                                sexp_msg = SEXP_list_pop (&(desc->sexpbuf));
                        }
                        
                        /* check if the received S-exp is valid SEAP msg */
                        if (!SEXP_listp (sexp_msg)) {
                                _D("invalid msg received - not a list\n");
                                continue;
                        }
                        
                        if (SEXP_strncmp (SEXP_list_first (sexp_msg), "seap.msg", 8) != 0) {
                                _D("invalid msg received - first s-exp != \"msg\"\n");
                                continue;
                        }
                        
                        if (SEXP_listp (SEXP_list_last (sexp_msg))) {
                                uint32_t i;

                                msg = SEAP_msg_new ();
                                msg->sexp = SEXP_list_last (sexp_msg);

                                /* parse attributes */
                                _A(SEXP_length (sexp_msg) > 0);
                                
                                for (i = 2; i < (SEXP_length (sexp_msg) - 1); ++i) {
                                        SEXP_t *attr;
                                        
                                        attr = SEXP_list_nth (sexp_msg, i);
                                        if (SEXP_stringp (attr)) {
                                                char *attrname;
                                                
                                                attrname = SEXP_string_cstr (attr);
                                                if (attrname != NULL) {
                                                        SEXP_t *attrval = NULL;
                                                        
                                                        if (*attrname == ':') {
                                                                if (strcmp (attrname, ":id") == 0) {
                                                                        attrval = SEXP_list_nth (sexp_msg, i + 1);
                                                                        
                                                                        if (SEXP_numberp (attrval)) {
                                                                                SEXP_number_get (attrval, &(msg->id), NUM_UINT64);
                                                                                ++i;
                                                                                _D("Msg id=%llu, %u, %hu, %hhu\n",
                                                                                   msg->id, (uint32_t)msg->id, (uint16_t)msg->id, (uint8_t)msg->id);
                                                                                continue;
                                                                        } else {
                                                                                _D("Non-numeric id!\n");
                                                                                errno = EINVAL;
                                                                                return (-1);
                                                                        }
                                                                } else {
                                                                        if ((i + 1) <= (SEXP_length (sexp_msg) - 1)) {
                                                                                attrval = SEXP_list_nth (sexp_msg, i + 1);
                                                                                memmove (attrname, attrname + 1,
                                                                                         sizeof (char) * (strlen (attrname)));
                                                                        } else {
                                                                                _D("Expected attribute value at position %u.\n", i + 1);
                                                                                continue;
                                                                        }
                                                                }
                                                                ++i;
                                                        }
                                                        
                                                        _D("New attribute: n=%s, vt=%s\n",
                                                           attrname, SEXP_strtype (attrval));
                                                        
                                                        SEAP_msgattr_set (msg, attrname, attrval);
                                                } else {
                                                        _D("Got NULL from SEXP_string_cstr.\n");
                                                        continue;
                                                }
                                        } else {
                                                _D("Expected attribute name at position %u but got something else of type: %s.\n",
                                                   i, SEXP_strtype (attr));
                                                continue;
                                        }
                                }

                                *seap_msg = msg;
                                
                                return (0);
                        } else {
                                /* invalid msg - last s-exp is not a list */
                                _D("invalid msg received - last s-exp is not a list\n");
                                continue;
                        }
                }
        } else {
                errno = EBADF;
                return (-1);
        }
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
        SEXP_list_add (sexp, SEXP_string_new ("seap.msg", 8));
        
        /* add message attributes */
        for (i = 0; i < msg->attrs_cnt; ++i) {
                if (msg->attrs[i].value != NULL) {
                        attr_namelen = strlen (msg->attrs[i].name) + 2;
                        attr_name    = xmalloc (sizeof (char) * attr_namelen);
                        
                        snprintf (attr_name, attr_namelen, ":%s", msg->attrs[i].name);
                        
                        SEXP_list_add (sexp, SEXP_string_new (attr_name, attr_namelen - 1));
                        SEXP_list_add (sexp, msg->attrs[i].value);
                        
                        xfree ((void **)&attr_name);
                } else {
                        SEXP_list_add (sexp, SEXP_string_new (msg->attrs[i].name,
                                                              strlen (msg->attrs[i].name)));
                }
        }
        
        /* add data */
        SEXP_list_add (sexp, msg->sexp);
        
        return (sexp);
}

int SEAP_msgattr_set (SEAP_msg_t *msg, const char *attr, SEXP_t *value)
{
        _A(msg != NULL);
        _A(attr != NULL);

        msg->attrs = xrealloc (msg->attrs, sizeof (SEAP_attr_t) * (++msg->attrs_cnt));
        msg->attrs[msg->attrs_cnt - 1].name  = strdup (attr);
        msg->attrs[msg->attrs_cnt - 1].value = value;
        
        return (0);
}

SEXP_t *SEAP_msgattr_get (SEAP_msg_t *msg, const char *name)
{
        SEXP_t *value = NULL;

        _A(msg  != NULL);
        _A(name != NULL);

        return (value);
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
        
                /* add id */
                msg_id = desc->next_id++;
                
                /* msg_id = __sync_fetch_and_add (&(desc->next_id), 1); */

                /* FIXME!!! */
                SEAP_msgattr_set (seap_msg, "id", SEXP_number_new (&(msg_id), NUM_UINT64));
                
                /* msg -> sexp */
                sexp_msg = __SEAP_msg2sexp (seap_msg);
                
                /* send */
                if (SCH_SENDSEXP(desc->scheme, desc, sexp_msg, 0) < 0) {
                        /* error */
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
        uint64_t req_id;

        _A(ctx != NULL);
        _A(rep_msg != NULL);
        _A(req_msg != NULL);
        
        SEAP_msgattr_set (rep_msg, "reply-id", SEXP_number_new (&(req_msg->id), NUM_UINT64));
        
        return SEAP_sendmsg (ctx, sd, rep_msg);
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
                return (-1);
        }
}
