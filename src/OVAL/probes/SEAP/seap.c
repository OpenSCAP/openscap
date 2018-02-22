/*
 * Copyright 2009 Red Hat Inc., Durham, North Carolina.
 * All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:
 *      "Daniel Kopecek" <dkopecek@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <errno.h>
#include "public/seap.h"
#include "public/sm_alloc.h"
#include "generic/common.h"
#include "_sexp-types.h"
#include "_sexp-parser.h"
#include "_seap-types.h"
#include "_seap-scheme.h"
#include "_seap-message.h"
#include "_seap-command.h"
#include "_seap-error.h"
#include "_seap-packet.h"
#include "_seap.h"
#include "seap-descriptor.h"

static void SEAP_CTX_initdefault (SEAP_CTX_t *ctx)
{
        _A(ctx != NULL);

        ctx->parser  = NULL /* PARSER(label) */;
        ctx->pflags  = SEXP_PFLAG_EOFOK;
        ctx->fmt_in  = SEXP_FMT_CANONICAL;
        ctx->fmt_out = SEXP_FMT_CANONICAL;

        /* Initialize descriptor table */
        ctx->sd_table    = SEAP_desctable_new();
        ctx->cmd_c_table = SEAP_cmdtbl_new ();

        ctx->recv_timeout = 5;
        ctx->send_timeout = 5;
        ctx->cflags       = 0;

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
        SEAP_desctable_free(ctx->sd_table);
        SEAP_cmdtbl_free (ctx->cmd_c_table);
        sm_free (ctx);

        return;
}

int SEAP_connect (SEAP_CTX_t *ctx, const char *uri, uint32_t flags)
{
        SEAP_desc_t  *dsc;
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

        sd = SEAP_desc_add (ctx->sd_table, NULL, scheme, NULL);

        if (sd < 0) {
                dI("Can't create/add new SEAP descriptor");
                return (-1);
        }

        dsc = SEAP_desc_get (ctx->sd_table, sd);

        if (dsc == NULL) {
                errno = ESRCH;
                return(-1);
        }
	dsc->subtype = ctx->subtype;

        if (SCH_CONNECT(scheme, dsc, uri + schstr_len + 1, flags) != 0) {
                dI("FAIL: errno=%u, %s.", errno, strerror (errno));
                SEAP_desc_del(ctx->sd_table, sd);

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
        SEAP_desc_t *dsc;
        int sd;

        sd = SEAP_desc_add (ctx->sd_table, NULL, SCH_QUEUE, NULL);

        if (sd < 0) {
                dI("Can't create/add new SEAP descriptor");
                return (-1);
        }

        dsc = SEAP_desc_get (ctx->sd_table, sd);

        if (dsc == NULL) {
                errno = ESRCH;
                return(-1);
        }

        if (SCH_OPENFD2(SCH_GENERIC, dsc, ifd, ofd, flags) != 0) {
                dI("FAIL: errno=%u, %s.", errno, strerror (errno));
                return (-1);
        }

        return (sd);
}

int SEAP_add_probe (SEAP_CTX_t *ctx, sch_queuedata_t *data)
{
	int sd = SEAP_desc_add(ctx->sd_table, NULL, SCH_QUEUE, data);
	dI("SEAP_add_probe");
	if (sd < 0) {
		dI("Can't create/add new SEAP descriptor");
		return (-1);
	}
	SEAP_desc_t *dsc = SEAP_desc_get (ctx->sd_table, sd);

	if (dsc == NULL) {
		dI("dsc == NULL");
	}
    return sd;
}

int SEAP_recvsexp (SEAP_CTX_t *ctx, int sd, SEXP_t **sexp)
{
        SEAP_msg_t *msg = NULL;

        if (SEAP_recvmsg (ctx, sd, &msg) == 0) {
                *sexp = SEAP_msg_get (msg);
                SEAP_msg_free (msg);

                return (0);
        }

        *sexp = NULL;
        return (-1);
}

static int __SEAP_cmdexec_reply (SEAP_CTX_t *ctx, int sd, SEAP_cmd_t *cmd)
{
        SEAP_desc_t   *dsc;
        SEXP_t        *res;
        SEAP_packet_t *packet;
        SEAP_cmd_t    *cmdrep;

        dsc = SEAP_desc_get (ctx->sd_table, sd);

        if (dsc == NULL)
                return (-1);

        res = SEAP_cmd_exec (ctx, sd, SEAP_EXEC_LOCAL,
                             cmd->code, cmd->args,
                             SEAP_CMDCLASS_USR, NULL, NULL);

        packet = SEAP_packet_new ();
        cmdrep = SEAP_packet_settype (packet, SEAP_PACKET_CMD);

        cmdrep->id     = SEAP_desc_gencmdid (ctx->sd_table, sd);
        cmdrep->rid    = cmd->id;
        cmdrep->flags |= SEAP_CMDFLAG_REPLY;
        cmdrep->args   = res;
        cmdrep->class  = cmd->class;
        cmdrep->code   = cmd->code;

        if (SEAP_packet_send (ctx, sd, packet) != 0) {
                protect_errno {
                        dI("FAIL: errno=%u, %s.", errno, strerror (errno));
                        SEAP_packet_free (packet);
                }
                return (-1);
        }

        if (res != NULL)
                SEXP_free(res);
        
        SEAP_packet_free (packet);

        return (0);
}

static void *__SEAP_cmdexec_worker (void *arg)
{
        SEAP_cmdjob_t *job = (SEAP_cmdjob_t *)arg;

        _A(job != NULL);
        _A(job->cmd != NULL);
#if defined(HAVE_PTHREAD_SETNAME_NP)
# if defined(__APPLE__)
	pthread_setname_np("command_worker");
# else
	pthread_setname_np(pthread_self(), "command_worker");
# endif
#endif

        if (job->cmd->flags & SEAP_CMDFLAG_REPLY) {
                (void) SEAP_cmd_exec (job->ctx, job->sd, SEAP_EXEC_WQUEUE,
                                      job->cmd->rid, job->cmd->args,
                                      SEAP_CMDCLASS_USR, NULL, NULL);
        } else {
                (void)__SEAP_cmdexec_reply (job->ctx, job->sd, job->cmd);
        }

        return (NULL);
}

int __SEAP_recvmsg_process_cmd (SEAP_CTX_t *ctx, int sd, SEAP_cmd_t *cmd)
{
        _A(ctx != NULL);
        _A(cmd != NULL);

        if (ctx->cflags & SEAP_CFLG_THREAD) {
                /*
                 *  Create a new thread for processing this command
                 */
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
                job->cmd = cmd;

                /* Create the worker */
                if (pthread_create (&th, &th_attrs,
                                    &__SEAP_cmdexec_worker, (void *)job) != 0)
                {
                        dI("Can't create worker thread: %u, %s.", errno, strerror (errno));
                        SEAP_cmdjob_free (job);
                        pthread_attr_destroy (&th_attrs);

                        return (-1);
                }

                pthread_attr_destroy (&th_attrs);
        } else {
                if (cmd->flags & SEAP_CMDFLAG_REPLY) {
                        (void) SEAP_cmd_exec (ctx, sd, SEAP_EXEC_WQUEUE,
                                              cmd->rid, cmd->args,
                                              SEAP_CMDCLASS_USR, NULL, NULL);
                } else {
                        int ret;

                        ret = __SEAP_cmdexec_reply (ctx, sd, cmd);

                        if (cmd->args != NULL)
                                SEXP_free(cmd->args);

                        return (ret);
                }
        }

        return (0);
}

static int __SEAP_recvmsg_process_err (SEAP_CTX_t *ctx, int sd, SEAP_err_t *err)
{
	SEAP_desc_t *sd_desc;
	SEAP_err_t  *cloned_err;
	void *prev_err;

	sd_desc = SEAP_desc_get(ctx->sd_table, sd);

	if (sd_desc == NULL) {
		errno = EBADF;
		return (-1);
	}

	cloned_err = SEAP_error_clone(err);
	prev_err   = NULL;

	/* XXX: handle 64bit message ids */
	if (rbt_i32_add(sd_desc->err_queue,
			(uint32_t)(err->id), cloned_err, (void **)&prev_err) != 0)
	{
		SEAP_error_free(cloned_err);
		errno = EINVAL;
		return (-1);
	}

	if (prev_err != NULL) {
		/* XXX: log this */
		SEAP_error_free((SEAP_err_t*)prev_err);
	}

        return (0);
}

int SEAP_recvmsg (SEAP_CTX_t *ctx, int sd, SEAP_msg_t **seap_msg)
{
        SEAP_packet_t *packet;

        _A(ctx      != NULL);
        _A(seap_msg != NULL);

        (*seap_msg) = NULL;

        /*
         * Packet loop
         */
        for (;;) {
                if (SEAP_packet_recv (ctx, sd, &packet) != 0) {
			protect_errno {
				dI("FAIL: ctx=%p, sd=%d, errno=%u, %s.",
				   ctx, sd, errno, strerror (errno));
			}
                        return (-1);
                }

                switch (SEAP_packet_gettype (packet)) {
                case SEAP_PACKET_MSG:

                        (*seap_msg) = sm_talloc (SEAP_msg_t);
                        memcpy ((*seap_msg), SEAP_packet_msg (packet), sizeof (SEAP_msg_t));

			SEAP_packet_free (packet);
                        return (0);
                case SEAP_PACKET_CMD:
                        switch (__SEAP_recvmsg_process_cmd (ctx, sd, SEAP_packet_cmd (packet))) {
                        case  0:
                                SEAP_packet_free (packet);
                                continue;
                        default:
                                errno = EDOOFUS;
                                return (-1);
                        }
                case SEAP_PACKET_ERR:
                        switch (__SEAP_recvmsg_process_err (ctx, sd, SEAP_packet_err (packet))) {
                        case 0:
                                break;
                        case -1:
                                SEAP_packet_free (packet);
                                return (-1);
                        default:
                                errno = EDOOFUS;
                                return (-1);
                        }

                        SEAP_packet_free (packet);
                        errno = ECANCELED;
                        return (-1);
                default:
                        abort ();
                }

                SEAP_packet_free (packet);
        }

        /* NOTREACHED */
        errno = EDOOFUS;
        return (-1);
}

int SEAP_sendmsg (SEAP_CTX_t *ctx, int sd, SEAP_msg_t *seap_msg)
{
        int ret;
        SEAP_packet_t *packet;
        SEAP_msg_t    *msg;

        packet = SEAP_packet_new ();
        msg    = (SEAP_msg_t *)SEAP_packet_settype (packet, SEAP_PACKET_MSG);

        seap_msg->id = SEAP_desc_genmsgid (ctx->sd_table, sd);
        memcpy (msg, seap_msg, sizeof (SEAP_msg_t));

        ret = SEAP_packet_send (ctx, sd, packet);

        protect_errno {
                SEAP_packet_free (packet);
        }
        return (ret);
}

int SEAP_sendsexp (SEAP_CTX_t *ctx, int sd, SEXP_t *sexp)
{
        SEAP_msg_t *msg;
        int ret;

        msg = SEAP_msg_new ();

        if (SEAP_msg_set (msg, sexp) == 0)
                ret = SEAP_sendmsg (ctx, sd, msg);
        else
                ret = -1;

        protect_errno {
                SEAP_msg_free (msg);
        }
        return (ret);
}

int SEAP_reply (SEAP_CTX_t *ctx, int sd, SEAP_msg_t *rep_msg, SEAP_msg_t *req_msg)
{
        SEXP_t *r0;

        _A(ctx     != NULL);
        _A(rep_msg != NULL);
        _A(req_msg != NULL);

        if (SEAP_msgattr_set (rep_msg, "reply-id",
#if SEAP_MSGID_BITS == 64
                              r0 = SEXP_number_newu_64 (req_msg->id)
#else
                              r0 = SEXP_number_newu_32 (req_msg->id)
#endif
                    ) == 0)
        {
                SEXP_free (r0);

                if (SEAP_msgattr_exists (req_msg, "no-reply")) {
                        SEAP_msg_t *nil_msg;
                        int         ret;

                        dD("no-reply set: sending empty reply");

                        nil_msg = SEAP_msg_clone (rep_msg);
                        SEAP_msg_unset (nil_msg);
                        ret = SEAP_sendmsg (ctx, sd, nil_msg);
                        SEAP_msg_free (nil_msg);

                        return (ret);
                } else {
                        dD("no-reply not set: sending full reply");
                        return SEAP_sendmsg (ctx, sd, rep_msg);
                }
        } else {
                SEXP_free (r0);
                return (-1);
        }
}

static int __SEAP_senderr (SEAP_CTX_t *ctx, int sd, SEAP_err_t *err, unsigned int type)
{
        SEAP_packet_t *packet;
        SEAP_err_t    *errptr;

        _A(ctx != NULL);
        _A(err != NULL);

        packet = SEAP_packet_new ();
        errptr = SEAP_packet_settype (packet, SEAP_PACKET_ERR);

        memcpy (errptr, err, sizeof (SEAP_err_t));
        errptr->type = type;

        if (SEAP_packet_send (ctx, sd, packet) != 0) {
                protect_errno {
                        dI("FAIL: errno=%u, %s.", errno, strerror (errno));
                        SEAP_packet_free (packet);
                }

                return (-1);
        }

        SEAP_packet_free (packet);
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
        errno = EOPNOTSUPP;
        return (-1);
}

/**
 * Receive the last error that happened on `sd' and was caused by a message
 * with ID `id'.
 * If an error is in the queue, it's saved at `*err' and zero is returned.
 * The caller takes care of freeing the memory used by the returned SEAP_err_t
 * structure. If there is not an error, 1 is returned. In case of an internal
 * error, -1 is returned an errno is set.
 */
int SEAP_recverr_byid (SEAP_CTX_t *ctx, int sd, SEAP_err_t **err, SEAP_msgid_t id)
{
	SEAP_desc_t *sd_desc;
	void *data;

	if (err == NULL) {
		errno = EINVAL;
		return (-1);
	}

	sd_desc = SEAP_desc_get(ctx->sd_table, sd);

	if (sd_desc == NULL) {
		errno = EBADF;
		return (-1);
	}

	data = NULL;

	if (rbt_i32_del(sd_desc->err_queue, (uint32_t)id, &data) != 0)
		return (1);
	else {
		if (data == NULL) {
			return -1;
		}
		*err = (SEAP_err_t *)data;
	}

        return (0);
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
        SEAP_desc_t *dsc;
        int ret;

        _A(ctx != NULL);

        if (sd < 0) {
                errno = EBADF;
                return (-1);
        }

        dsc = SEAP_desc_get (ctx->sd_table, sd);

        if (dsc == NULL) {
                errno = EBADF;
                return (-1);
        }

        ret = SCH_CLOSE(dsc->scheme, dsc, 0); /* TODO: Are flags usable here? */

        protect_errno {
                if (SEAP_desc_del (ctx->sd_table, sd) != 0) {
                        /* something very bad happened */
                        dI("SEAP_desc_del failed");
                        return(-1);
                }
        }

        return(ret);
}
