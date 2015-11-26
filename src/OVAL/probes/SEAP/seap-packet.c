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

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "generic/common.h"
#include "public/sexp-manip.h"
#include "_sexp-parser.h"
#include "_seap-packetq.h"
#include "_seap-packet.h"
#include "_seap-scheme.h"
#include "seap-descriptor.h"
#include "public/seap-message.h"
#include "public/seap-command.h"
#include "public/seap-error.h"
#include "public/sm_alloc.h"

SEAP_packet_t *SEAP_packet_new (void)
{
        SEAP_packet_t *p;

        p = sm_talloc (SEAP_packet_t);
        memset (p, 0, sizeof (SEAP_packet_t));
        p->type = SEAP_PACKET_INV;

        return (p);
}

void SEAP_packet_free (SEAP_packet_t *packet)
{
        sm_free (packet);
}

void *SEAP_packet_settype (SEAP_packet_t *packet, uint8_t type)
{
        _A(packet != NULL);
        _A(type == SEAP_PACKET_MSG ||
           type == SEAP_PACKET_CMD ||
           type == SEAP_PACKET_ERR);

        packet->type = type;

        switch (type) {
        case SEAP_PACKET_MSG:
                return ((void *)&(packet->data.msg));
        case SEAP_PACKET_ERR:
                return ((void *)&(packet->data.msg));
        case SEAP_PACKET_CMD:
                return ((void *)&(packet->data.msg));
        default:
                abort ();
        }

        /* not reached */
        return (NULL);
}

uint8_t SEAP_packet_gettype (SEAP_packet_t *packet)
{
        _A(packet != NULL);
        return (packet->type);
}

SEAP_msg_t *SEAP_packet_msg (SEAP_packet_t *packet)
{
        return (&(packet->data.msg));
}

SEAP_cmd_t *SEAP_packet_cmd (SEAP_packet_t *packet)
{
        return (&(packet->data.cmd));
}

SEAP_err_t *SEAP_packet_err (SEAP_packet_t *packet)
{
        return (&(packet->data.err));
}

static int SEAP_packet_sexp2msg (SEXP_t *sexp_msg, SEAP_msg_t *seap_msg)
{
        size_t msg_icnt, msg_n, attr_i;
        SEXP_t *attr_name, *attr_val;

        _A(sexp_msg != NULL);
        _A(seap_msg != NULL);

        msg_icnt = SEXP_list_length (sexp_msg);

        seap_msg->attrs_cnt = msg_icnt - 4;
        seap_msg->attrs     = sm_alloc (sizeof (SEAP_attr_t) * seap_msg->attrs_cnt);

        for (msg_n = 2, attr_i = 0; msg_n < msg_icnt; ++msg_n) {

                attr_name = SEXP_list_nth (sexp_msg, msg_n);
                if (attr_name == NULL) {
                        dI("Unexpected error: No S-exp (attr_name) at position %u in the message (%p).\n",
                           msg_n, sexp_msg);

                        sm_free (seap_msg->attrs);

                        return (SEAP_EUNEXP);
                }

                if (SEXP_string_nth (attr_name, 1) == ':') {
                        if (SEXP_strcmp (attr_name, ":id") == 0) {

                                attr_val = SEXP_list_nth (sexp_msg, msg_n + 1);
                                if (attr_val == NULL) {
                                        dI("Unexpected error: \"%s\": No attribute value at position %u in the message (%p).\n",
                                           "id", msg_n + 1, sexp_msg);

                                        sm_free (seap_msg->attrs);
					SEXP_free(attr_name);

                                        return (SEAP_EUNEXP);
                                }

#if SEAP_MSGID_BITS == 64
                                seap_msg->id = SEXP_number_getu_64 (attr_val);
                                if (seap_msg->id == UINT64_MAX)
#else
                                seap_msg->id = SEXP_number_getu_32 (attr_val);
                                if (seap_msg->id == UINT32_MAX)
#endif
                                {
                                        switch (errno) {
                                        case EDOM:
                                                dI("id: invalid value or type: s_exp=%p, type=%s\n",
                                                   (void *)attr_val, SEXP_strtype (attr_val));

                                                sm_free (seap_msg->attrs);
                                                SEXP_free(attr_val);
                                                SEXP_free(attr_name);

                                                return (SEAP_EINVAL);
                                        case EFAULT:
                                                dI("id: not found\n");

                                                sm_free (seap_msg->attrs);
                                                SEXP_free(attr_val);
                                                SEXP_free(attr_name);

                                                return (SEAP_EINVAL);
                                        }
                                }

                                SEXP_free (attr_val);
                        } else {
                                seap_msg->attrs[attr_i].name  = SEXP_string_subcstr (attr_name, 1, 0);
                                seap_msg->attrs[attr_i].value = SEXP_list_nth (sexp_msg, msg_n + 1);

                                if (seap_msg->attrs[attr_i].value == NULL) {
                                        dI("Unexpected error: \"%s\": No attribute value at position %u in the message (%p).\n",
                                           seap_msg->attrs[attr_i].name, msg_n + 1, sexp_msg);

                                        sm_free (seap_msg->attrs[attr_i].name);

                                        for (; attr_i > 0; --attr_i) {
                                                sm_free (seap_msg->attrs[attr_i - 1].name);

                                                if (seap_msg->attrs[attr_i - 1].value != NULL)
                                                        SEXP_free (seap_msg->attrs[attr_i - 1].value);
                                        }

                                        sm_free (seap_msg->attrs);
                                        SEXP_free(attr_name);

                                        return (SEAP_EINVAL);
                                }

                                ++attr_i;
                        }

                        ++msg_n;
                } else {
                        seap_msg->attrs[attr_i].name  = SEXP_string_cstr (attr_name);
                        seap_msg->attrs[attr_i].value = NULL;

                        ++attr_i;
                }

                SEXP_free (attr_name);
        }

        _A(attr_i <= (SEXP_list_length (sexp_msg) - 4));
        _A(attr_i >= (SEXP_list_length (sexp_msg) - 4)/2);

        seap_msg->attrs_cnt = attr_i;
        seap_msg->attrs     = sm_realloc (seap_msg->attrs, sizeof (SEAP_attr_t) * seap_msg->attrs_cnt);
        seap_msg->sexp      = SEXP_list_last (sexp_msg);

        return (0);
}

static SEXP_t *SEAP_packet_msg2sexp (SEAP_msg_t *msg)
{
        SEXP_t *sexp;
        uint16_t i;

        SEXP_t *r0, *r1, *r2;

        _A(msg != NULL);

        sexp = SEXP_list_new (r0 = SEXP_string_new (SEAP_SYM_MSG, strlen (SEAP_SYM_MSG)),
                              r1 = SEXP_string_new (":id", 3),
#if SEAP_MSGID_BITS == 64
                              r2 = SEXP_number_newu_64 (msg->id),
#else
                              r2 = SEXP_number_newu_32 (msg->id),
#endif
                              NULL);

        SEXP_vfree (r0, r1, r2, NULL);

        /* Add message attributes */
        for (i = 0; i < msg->attrs_cnt; ++i) {
                if (msg->attrs[i].value != NULL) {
                        SEXP_list_add (sexp, r0 = SEXP_string_newf (":%s", msg->attrs[i].name));
                        SEXP_list_add (sexp, msg->attrs[i].value);
                } else {
                        SEXP_list_add (sexp, r0 = SEXP_string_new (msg->attrs[i].name,
                                                                   strlen (msg->attrs[i].name)));
                }

                SEXP_free (r0);
        }

        /* Add data */
        if (msg->sexp != NULL)
                SEXP_list_add (sexp, msg->sexp);
        else {
                SEXP_list_add (sexp, r0 = SEXP_list_new (NULL)); /* FIXME */
                SEXP_free(r0);
        }

	dI("MSG -> SEXP\n");
	dO(OSCAP_DEBUGOBJ_SEXP, sexp);
	dI("packet size: %zu\n", SEXP_sizeof(sexp));

        return (sexp);
}

static int SEAP_packet_sexp2cmd (SEXP_t *sexp_cmd, SEAP_cmd_t *seap_cmd)
{
        SEXP_t *item, *val;
        size_t i;
        int mattrs;

        memset (seap_cmd, 0, sizeof (SEAP_cmd_t));
        seap_cmd->class = SEAP_CMDCLASS_INT;
        seap_cmd->args  = NULL;

        mattrs = 0;
        i = 2;

        while ((item = SEXP_list_nth (sexp_cmd, i)) != NULL) {
                if (SEXP_strncmp (item, ":", 1) == 0) {
                        char *attr_name;

                        attr_name = SEXP_string_cstr (item);

                        switch (*(attr_name + 1)) {
                        case 'i':
                                if (strcmp (attr_name + 1, "id") == 0) {
                                        val = SEXP_list_nth (sexp_cmd, i + 1);

                                        if (val == NULL)
                                                break;

                                        if (SEXP_numberp (val)) {
                                                seap_cmd->id = SEXP_number_getu_16 (val);
                                                ++mattrs;
                                        }

                                        SEXP_free (val);
                                }
                                break;
                        case 'r':
                                if (strcmp (attr_name + 1, "reply_id") == 0) {
                                        val = SEXP_list_nth (sexp_cmd, i + 1);

                                        if (val == NULL)
                                                break;

                                        if (SEXP_numberp (val)) {
                                                seap_cmd->rid = SEXP_number_getu_16 (val);
                                                seap_cmd->flags |= SEAP_CMDFLAG_REPLY;
                                        }

                                        SEXP_free (val);
                                }
                                break;
                        case 'c':
                                if (strcmp (attr_name + 1, "class") == 0) {
                                        val = SEXP_list_nth (sexp_cmd, i + 1);

                                        if (val == NULL)
                                                break;

                                        if (SEXP_stringp (val)) {
                                                if (SEXP_strcmp (val, "usr") == 0) {
                                                        seap_cmd->class = SEAP_CMDCLASS_USR;
                                                }
                                        }

                                        SEXP_free (val);
                                }
                                break;
                        case 't':
                                if (strcmp (attr_name + 1, "type") == 0) {
                                        val = SEXP_list_nth (sexp_cmd, i + 1);

                                        if (val == NULL)
                                                break;

                                        if (SEXP_stringp (val)) {
                                                if (SEXP_strcmp (val, "sync") == 0)
                                                        seap_cmd->flags |= SEAP_CMDFLAG_SYNC;
                                        }

                                        SEXP_free (val);
                                }
                                break;
                        }

                        free (attr_name);
                        ++i; ++i;
                } else {
                        break;
                }

                SEXP_free (item);
        }

        if (item == NULL || mattrs < 1) {
                if (item != NULL)
                        SEXP_free(item);

                errno = EINVAL;
                return (-1);
        }

        if (SEXP_numberp (item)) {
                seap_cmd->code = SEXP_number_getu_16 (item);
                SEXP_free (item);
        } else {
                SEXP_free (item);
                errno = EINVAL;
                return (-1);
        }

        seap_cmd->args = SEXP_list_nth (sexp_cmd, ++i);

        return (0);
}

static SEXP_t *SEAP_packet_cmd2sexp (SEAP_cmd_t *cmd)
{
        SEXP_t *sexp;
        SEXP_t *r0, *r1, *r2;

        _A(cmd != NULL);

        sexp = SEXP_list_new (r0 = SEXP_string_new (SEAP_SYM_CMD, strlen (SEAP_SYM_CMD)),
                              r1 = SEXP_string_new (":id", 3),
                              r2 = SEXP_number_newu_16 (cmd->id),
                              NULL);

        SEXP_vfree (r0, r1, r2, NULL);

        if (cmd->flags & SEAP_CMDFLAG_REPLY) {
                SEXP_list_add (sexp,
                               r0 = SEXP_string_new (":reply_id", 9));
                SEXP_list_add (sexp,
                               r1 = SEXP_number_newu_16 (cmd->rid));

                SEXP_vfree (r0, r1, NULL);
        }

        SEXP_list_add (sexp,
                       r0 = SEXP_string_new (":class", 6));
        SEXP_free (r0);

        switch (cmd->class) {
        case SEAP_CMDCLASS_USR:
                SEXP_list_add (sexp,
                               r0 = SEXP_string_new ("usr", 3));
                break;
        case SEAP_CMDCLASS_INT:
                SEXP_list_add (sexp,
                               r0 = SEXP_string_new ("int", 3));
                break;
        default:
                abort ();
        }


        SEXP_free (r0);

        SEXP_list_add (sexp,
                       r0 = SEXP_string_new (":type", 5));

        SEXP_list_add (sexp,
                       r1 = (cmd->flags & SEAP_CMDFLAG_SYNC ?
                             SEXP_string_new ("sync", 4) : SEXP_string_new ("async", 5)));

        SEXP_list_add (sexp,
                       r2 = SEXP_number_newu_16 (cmd->code));

        SEXP_vfree (r0, r1, r2, NULL);

        if (cmd->args != NULL)
                SEXP_list_add (sexp, cmd->args);

	dI("CMD -> SEXP\n");
	dO(OSCAP_DEBUGOBJ_SEXP, sexp);
	dI("packet size: %zu\n", SEXP_sizeof(sexp));

        SEXP_VALIDATE(sexp);
        return (sexp);
}

static int SEAP_packet_sexp2err (SEXP_t *sexp_err, SEAP_err_t *seap_err)
{
        SEXP_t  *member;
        uint32_t n;

        seap_err->id   = 0;
        seap_err->code = SEAP_EUNKNOWN;
        seap_err->type = SEAP_ETYPE_INT;
        seap_err->data = NULL;

        n = 2;

        while ((member = SEXP_list_nth (sexp_err, n)) != NULL) {
                switch (SEXP_typeof (member)) {
                case SEXP_TYPE_STRING:
                        if (SEXP_string_nth (member, 1) != ':') {
                                dI("Invalid string/attribute in packet\n");
                                SEXP_free (member);
                                errno = EINVAL;
                                return (-1);
                        }

                        if (SEXP_strcmp (member, ":orig_id") == 0) {
                                SEXP_t *val;

                                val = SEXP_list_nth (sexp_err, ++n);

                                if (!SEXP_numberp (val)) {
                                        dI("Invalid type of :orig_id value\n");
                                        SEXP_free (val);
                                        SEXP_free (member);
                                        errno = EINVAL;
                                        return (-1);
                                }

#if SEAP_MSGID_BITS == 64
                                seap_err->id = SEXP_number_getu_64 (val);
#else
                                seap_err->id = SEXP_number_getu_32 (val);
#endif
                                SEXP_free (val);
                        } else if (SEXP_strcmp (member, ":type") == 0) {
                                SEXP_t *val;

                                val = SEXP_list_nth (sexp_err, ++n);
				seap_err->type = SEXP_number_getu_32(val);

				if (!SEXP_numberp(val)) {
                                        dI("Invalid type of the :type attribute\n");
                                        SEXP_free (val);
                                        SEXP_free (member);
                                        errno = EINVAL;
                                        return (-1);
                                }

                                if (!(seap_err->type == SEAP_ETYPE_INT ||
				      seap_err->type == SEAP_ETYPE_USER)) {
					dI("Invalid value of the :type attribute\n");
					SEXP_free(val);
					SEXP_free(member);
					errno = EINVAL;
					return -1;
				}

                                SEXP_free (val);
                        } else {
                                dI("Unknown packet attribute\n");
                                SEXP_free (member);
                                errno = EINVAL;
                                return (-1);
                        }

                        break;
                case SEXP_TYPE_NUMBER:
                        seap_err->code = SEXP_number_getu_32 (member);
                        SEXP_free (member);

                        goto loop_exit;
                default:
                        dI("Unexpected type of packet member: list\n");
                        SEXP_free (member);
                        errno = EINVAL;
                        return (-1);
                }

                SEXP_free(member);
                ++n;
        }
loop_exit:
        seap_err->data = SEXP_list_nth (sexp_err, n + 1);

        return (0);
}

static SEXP_t *SEAP_packet_err2sexp (SEAP_err_t *err)
{
        SEXP_t *sexp;
        SEXP_t *r0, *r1, *r2, *r3, *r4, *r5;

        sexp = SEXP_list_new (r0 = SEXP_string_new (SEAP_SYM_ERR, strlen (SEAP_SYM_ERR)),
                              r1 = SEXP_string_new (":orig_id", 8),
#if SEAP_MSGID_BITS == 64
                              r2 = SEXP_number_newu_64 (err->id),
#else
                              r2 = SEXP_number_newu_32 (err->id),
#endif
                              r3 = SEXP_string_new (":type", 5),
                              r4 = SEXP_number_newu (err->type),
                              r5 = SEXP_number_newu (err->code),
                              NULL);

        SEXP_vfree (r0, r1, r2, r3, r4, r5);

        if (err->data != NULL)
                SEXP_list_add (sexp, err->data);

	dI("ERR -> SEXP\n");
	dO(OSCAP_DEBUGOBJ_SEXP, sexp);
	dI("packet size: %zu\n", SEXP_sizeof(sexp));

        return (sexp);
}

static SEXP_t *SEAP_packet2sexp (SEAP_packet_t *packet)
{
        SEXP_t *sexp = NULL;

        switch (packet->type) {
        case SEAP_PACKET_MSG:
                sexp = SEAP_packet_msg2sexp (SEAP_packet_msg (packet));
                break;
        case SEAP_PACKET_CMD:
                sexp = SEAP_packet_cmd2sexp (SEAP_packet_cmd (packet));
                break;
        case SEAP_PACKET_ERR:
                sexp = SEAP_packet_err2sexp (SEAP_packet_err (packet));
                break;
        default:
                errno = EINVAL;
        }

        return (sexp);
}

int SEAP_packet_recv (SEAP_CTX_t *ctx, int sd, SEAP_packet_t **packet)
{
        SEAP_desc_t *dsc;
        SEXP_t      *sexp_buffer;
        SEXP_t      *sexp_packet;
        void        *data_buffer;
        size_t       data_buflen;
        ssize_t      data_length;

        SEXP_psetup_t *psetup;
        SEXP_pstate_t *pstate;

        SEXP_t     *psym_sexp;
        char        psym_cstr_b[16+1];
        char       *psym_cstr;

	SEAP_packet_t *_packet;

        dsc = SEAP_desc_get (ctx->sd_table, sd);

        if (dsc == NULL) {
                errno = EFAULT;
                return (-1);
        }

	if (SEAP_packetq_get(&dsc->pck_queue, packet) != -1)
		return (0);

        /*
         * Event loop
         * The read mutex is not locked during the wait for an event.
         * This allows other threads to hijack the input stream and
         * to perform high priority tasks (e.g. synchronous commands).
         * If there is a read event and nobody (except this thread) has
         * locked the read mutex then the execution of this loop is
         * stopped, the read mutex gets locked and the receiving and
         * parsing of data can begin.
         */

eloop_start:

        for (;;) {
                if (SCH_SELECT(dsc->scheme, dsc, SEAP_IO_EVREAD, 0, 0) != 0)
                        return (-1);

                dD("return from select\n");

                switch (DESC_TRYRLOCK (dsc)) {
                case  1:
                        goto eloop_exit;
                case  0:
                        switch (DESC_RLOCK (dsc)) {
                        case  1:
                                DESC_RUNLOCK(dsc);
                                continue;
                        case -1:
                                goto fail_rmutex;
                        default:
                                abort ();
                        }
                case -1:
                        fail_rmutex:

                        protect_errno {
                                dE("An error ocured while locking the read mutex: dsc=%p, errno=%u, %s.\n",
                                   dsc, errno, strerror (errno));
                        }

                        return (-1);
                default:
                        abort ();
                }
        }
eloop_exit:

        /*
         * Receive loop
         * The read mutex is locked during execution of this loop and
         * the execution stops if the received data forms a valid
         * S-expression.
         */

        pstate = NULL;
        psetup = SEXP_psetup_new ();

        /*
         * All buffer passed to SEXP_parse will be freed by
         * SEXP_pstate_free (i.e. after successful parsing)
         */
        SEXP_psetup_setflags(psetup, SEXP_PFLAG_FREEBUF);

        for (;;) {
                data_buffer = sm_alloc (SEAP_RECVBUF_SIZE);
                data_buflen = SEAP_RECVBUF_SIZE;
                data_length = SCH_RECV(dsc->scheme, dsc, data_buffer, data_buflen, 0);

                if (data_length < 0) {
                        protect_errno {
                                dI("FAIL: recv failed: dsc=%p, errno=%u, %s.\n", dsc, errno, strerror (errno));

                                sm_free (data_buffer);
                                SEXP_psetup_free (psetup);

                                if (pstate != NULL)
                                        SEXP_pstate_free (pstate);
                        }
                        return (-1);
                } else if (data_length == 0) {
                        dI("zero bytes received -> EOF\n");
                        sm_free (data_buffer);
                        SEXP_psetup_free (psetup);

                        if (pstate != NULL) {
                                dI("FAIL: incomplete S-exp received\n");
                                errno = ENETRESET;
                                return (-1);
                        } else {
                                errno = ECONNABORTED;
                                return (-1);
                        }
                }

                _A(data_length > 0);

                if (data_buflen != (size_t)(data_length)) {
                        data_buffer = sm_realloc (data_buffer, data_length);
			data_buflen = data_length;
		}

                sexp_buffer = SEXP_parse (psetup, data_buffer, data_length, &pstate);

                if (sexp_buffer != NULL) {
                        _A(pstate == NULL);

                        DESC_RUNLOCK(dsc);

                        if (SEXP_list_length (sexp_buffer) > 0) {
                                break;
                        } else {
                                SEXP_list_free (sexp_buffer);
                                SEXP_psetup_free (psetup);
				SEXP_free(sexp_buffer);
                                dI("eloop_restart\n");
                                goto eloop_start;
                        }
                } else {
			if (pstate == NULL || SEXP_pstate_errorp(pstate)) {
				dI("FAIL: S-exp parsing error, buffer: length: %ld, content:\n%*.s\n",
				   data_length, data_length, data_buffer);

				SEXP_psetup_free(psetup);
				SEXP_pstate_free(pstate);

				errno = EILSEQ;

				return (-1);
			}
		}

                if (SCH_SELECT(dsc->scheme, dsc, SEAP_IO_EVREAD, ctx->recv_timeout, 0) != 0) {
                        switch (errno) {
                        case ETIMEDOUT:
                                protect_errno {
                                        dI("FAIL: recv failed (timeout): dsc=%p, time=%hu, errno=%u, %s.\n",
                                           dsc, ctx->recv_timeout, errno, strerror (errno));
                                }
                                /* FALLTHROUGH */
                        default:
                                protect_errno {
                                        dI("FAIL: recv failed: dsc=%p, errno=%u, %s.\n",
                                           dsc, errno, strerror (errno));

                                        SEXP_psetup_free (psetup);
                                        SEXP_pstate_free (pstate);
                                }
                                SEXP_free(sexp_buffer);
                                return (-1);
                        }
                }
        }

        SEXP_psetup_free (psetup);
	SEXP_VALIDATE(sexp_buffer);
	(*packet) = NULL;

        while((sexp_packet = SEXP_list_pop (sexp_buffer)) != NULL) {
		if (!SEXP_listp(sexp_packet)) {
			dI("Invalid SEAP packet received: %s.\n", "not a list");

			SEXP_free (sexp_packet);
			SEXP_free (sexp_buffer);

			errno = EINVAL;
			return (-1);
		} else if (SEXP_list_length (sexp_packet) < 2) {
			dI("Invalid SEAP packet received: %s.\n", "list length < 2");

			SEXP_free (sexp_packet);
			SEXP_free (sexp_buffer);

			errno = EINVAL;
			return (-1);
		}

		psym_sexp = SEXP_list_first (sexp_packet);

		if (!SEXP_stringp(psym_sexp)) {
			dI("Invalid SEAP packet received: %s.\n", "first list item is not a string");

			SEXP_free (psym_sexp);
			SEXP_free (sexp_packet);
			SEXP_free (sexp_buffer);

			errno = EINVAL;
			return (-1);
		} else if (SEXP_string_length (psym_sexp) != (strlen (SEAP_SYM_PREFIX) + 3)) {
			dI("Invalid SEAP packet received: %s.\n", "invalid packet type symbol length");

			SEXP_free (psym_sexp);
			SEXP_free (sexp_packet);
			SEXP_free (sexp_buffer);

			errno = EINVAL;
			return (-1);
		} else if (SEXP_strncmp (psym_sexp, SEAP_SYM_PREFIX, strlen (SEAP_SYM_PREFIX)) != 0) {
			dI("Invalid SEAP packet received: %s.\n", "invalid prefix");

			SEXP_free (psym_sexp);
			SEXP_free (sexp_packet);
			SEXP_free (sexp_buffer);

			errno = EINVAL;
			return (-1);
		}

		SEXP_string_cstr_r (psym_sexp, psym_cstr_b, sizeof psym_cstr_b);
		psym_cstr = psym_cstr_b + strlen (SEAP_SYM_PREFIX);
		SEXP_free (psym_sexp);

		switch (psym_cstr[0]) {
		case 'm':
			if (psym_cstr[1] == 's' &&
			    psym_cstr[2] == 'g')
			{
				_packet = SEAP_packet_new ();
				_packet->type = SEAP_PACKET_MSG;

				if (SEAP_packet_sexp2msg (sexp_packet, &(_packet->data.msg)) != 0) {
					/* error */
					dI("Invalid SEAP packet received: %s.\n", "can't translate to msg struct");

					SEXP_free (sexp_packet);
					SEAP_packet_free(_packet);
					SEXP_free (sexp_buffer);

					errno = EINVAL;
					return (-1);
				}
				break;
			}
			goto invalid;
		case 'c':
			if (psym_cstr[1] == 'm' &&
			    psym_cstr[2] == 'd')
			{
				_packet = SEAP_packet_new ();
				_packet->type = SEAP_PACKET_CMD;

				if (SEAP_packet_sexp2cmd (sexp_packet, &(_packet->data.cmd)) != 0) {
					/* error */
					dI("Invalid SEAP packet received: %s.\n", "can't translate to cmd struct");
					SEXP_free (sexp_packet);
					SEAP_packet_free(_packet);
					SEXP_free (sexp_buffer);

					errno = EINVAL;
					return (-1);
				}
				break;
			}
			goto invalid;
		case 'e':
			if (psym_cstr[1] == 'r' &&
			    psym_cstr[2] == 'r')
			{
				_packet = SEAP_packet_new ();
				_packet->type = SEAP_PACKET_ERR;

				if (SEAP_packet_sexp2err (sexp_packet, &(_packet->data.err)) != 0) {
					/* error */
					dI("Invalid SEAP packet received: %s.\n", "can't translate to err struct");
					SEXP_free (sexp_packet);
					SEAP_packet_free(_packet);
					SEXP_free (sexp_buffer);

					errno = EINVAL;
					return (-1);
				}
				break;
			}
			/* FALLTHROUGH */
		default:
		invalid:
			dI("Invalid SEAP packet received: %s.\n", "invalid packet type symbol");
			SEXP_free (sexp_packet);
			SEXP_free (sexp_buffer);
			errno = EINVAL;
			return (-1);
		}


		dI("Received packet\n");
		dO(OSCAP_DEBUGOBJ_SEXP, sexp_packet);
		dI("packet size: %zu\n", SEXP_sizeof(sexp_packet));

		SEXP_free(sexp_packet);

		if (*packet == NULL)
			(*packet) = _packet;
		else
			SEAP_packetq_put(&dsc->pck_queue, _packet);

		_packet = NULL;
	}

        SEXP_free(sexp_buffer);

        return (0);
}

int SEAP_packet_recv_bytype (SEAP_CTX_t *ctx, int sd, SEAP_packet_t **packet, uint8_t type)
{
        int ret;
        SEAP_packet_t *pck;
        SEAP_desc_t   *dsc;

        _A(ctx    != NULL);
        _A(packet != NULL);
        _A(type == SEAP_PACKET_CMD ||
           type == SEAP_PACKET_MSG ||
           type == SEAP_PACKET_ERR);

        dsc = SEAP_desc_get (ctx->sd_table, sd);

        if (dsc == NULL)
                return (-1);

        for (;;) {
                pck = NULL;
                ret = SEAP_packet_recv (ctx, sd, &pck);

                if (ret != 0)
                        return (ret);
                else if (pck->type == type)
                        break;
                else if (SEAP_packetq_put (&dsc->pck_queue, pck) == -1) /* XXX: infinite loop */
                        return (-1);
        }

        *packet = pck;

        return (ret);
}

int SEAP_packet_send (SEAP_CTX_t *ctx, int sd, SEAP_packet_t *packet)
{
        SEXP_t *packet_sexp;
        SEAP_desc_t *dsc;
        int ret;

        ret = -1;
        dsc = SEAP_desc_get (ctx->sd_table, sd);

        if (dsc == NULL)
                return (-1);

        packet_sexp = SEAP_packet2sexp (packet);

        if (packet_sexp == NULL) {
                dI("Can't convert S-exp to packet\n");
                return (-1);
        }

        if (DESC_WLOCK (dsc)) {
                ret = 0;

                if (SCH_SENDSEXP(dsc->scheme, dsc, packet_sexp, 0) < 0) {
                        ret = -1;

                        protect_errno {
                                dI("FAIL: errno=%u, %s.\n", errno, strerror (errno));
                        }
                }

                DESC_WUNLOCK(dsc);
        }

        protect_errno {
                SEXP_free (packet_sexp);
        }

        return (ret);
}

int SEAP_packet_enqueue (SEAP_CTX_t *ctx, int sd, SEAP_packet_t *packet)
{
        return (-1);
}
