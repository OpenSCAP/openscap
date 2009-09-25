#ifndef __STUB_PROBE
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "generic/common.h"
#include "public/sexp-manip.h"
#include "_sexp-parse.h"
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

        _LOGCALL_;

        p = sm_talloc (SEAP_packet_t);
        memset (p, 0, sizeof (SEAP_packet_t));
        p->type = SEAP_PACKET_INV;
        
        return (p);
}

void SEAP_packet_free (SEAP_packet_t *packet)
{
        _LOGCALL_;
        sm_free (packet);
}

void *SEAP_packet_settype (SEAP_packet_t *packet, uint8_t type)
{
        _A(packet != NULL);
        _A(type == SEAP_PACKET_MSG ||
           type == SEAP_PACKET_CMD ||
           type == SEAP_PACKET_ERR);

        _LOGCALL_;

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
        _LOGCALL_;
        return (packet->type);
}

SEAP_msg_t *SEAP_packet_msg (SEAP_packet_t *packet)
{
        _LOGCALL_;
        return (&(packet->data.msg));
}

SEAP_cmd_t *SEAP_packet_cmd (SEAP_packet_t *packet)
{
        _LOGCALL_;
        return (&(packet->data.cmd));
}

SEAP_err_t *SEAP_packet_err (SEAP_packet_t *packet)
{
        _LOGCALL_;
        return (&(packet->data.err));
}

int SEAP_packet_sexp2msg (SEXP_t *sexp_msg, SEAP_msg_t *seap_msg)
{
        size_t msg_icnt, msg_i, attr_i;
        SEXP_t *attr_name, *attr_val;
        
        _A(sexp_msg != NULL);
        _A(seap_msg != NULL);
        _LOGCALL_;
        
        memset (seap_msg, 0, sizeof (SEAP_msg_t));
        msg_icnt = SEXP_list_length (sexp_msg);
        
        /* 
         * In the worst case there should be (msg_icnt - 2) - 1 attributes.
         * That is: The mandatory attribute "id" which has a value + other
         * attributes which could all be attributes without a value. The
         * last element of the message is the S-exp to be transfered.
         */
        seap_msg->attrs_cnt = (msg_icnt - 2) - 1;
        seap_msg->attrs = sm_alloc (sizeof (SEAP_attr_t) * (seap_msg->attrs_cnt));
        
        /* BEG: attribute loop */
        for (msg_i = 2, attr_i = 0; msg_i <= (msg_icnt - 1); ++msg_i) {
        
                attr_name = SEXP_list_nth (sexp_msg, msg_i);
                if (attr_name == NULL) {
                        _D("Unexpected error: No S-exp (attr_name) at position %u in the message (%p).\n",
                           msg_i, sexp_msg);
                        
                        sm_free (seap_msg->attrs);
                        
                        return (SEAP_EUNEXP);
                }
                 
                if (SEXP_strncmp (attr_name, ":", 1) == 0) {
                        /* with value */
                        
                        if (SEXP_strcmp (attr_name, ":id") == 0) {
                                
                                attr_val = SEXP_list_nth (sexp_msg, msg_i + 1);
                                if (attr_val == NULL) {
                                        _D("Unexpected error: \"%s\": No attribute value at position %u in the message (%p).\n",
                                           "id", msg_i + 1, sexp_msg);
                                        
                                        sm_free (seap_msg->attrs);
                                        
                                        return (SEAP_EUNEXP);
                                }

                                if (SEXP_number_get (attr_val, &(seap_msg->id),
                                                     SEXP_NUM_UINT64) != 0)
                                {
                                        _D("\"id\": Invalid value or type: sexp=%p, type=%s.\n",
                                           attr_val, SEXP_strtype (attr_val));
                                        
                                        sm_free (seap_msg->attrs);
                                        
                                        return (SEAP_EINVAL);
                                }
                        } else {
                                
                                seap_msg->attrs[attr_i].name  = SEXP_string_subcstr (attr_name, 1, 0);
                                seap_msg->attrs[attr_i].value = SEXP_list_nth (sexp_msg, msg_i + 1);
                                
                                if (seap_msg->attrs[attr_i].value == NULL) {
                                        _D("Unexpected error: \"%s\": No attribute value at position %u in the message (%p).\n",
                                           seap_msg->attrs[attr_i].name, msg_i + 1, sexp_msg);
                                        
                                        sm_free (seap_msg->attrs);
                                        
                                        /* FIXME: free already processed attributes */

                                        return (SEAP_EINVAL);
                                }
                                                                
                                ++attr_i;
                        }
                        
                        ++msg_i;
                        ++msg_i;
                } else {
                        /* without value */
                        
                        seap_msg->attrs[attr_i].name = SEXP_string_cstr (attr_name);
                        seap_msg->attrs[attr_i].value = NULL;
                        
                        ++msg_i;
                        ++attr_i;
                }
        } /* END: attribute loop */
        
        seap_msg->sexp = SEXP_list_last (sexp_msg);
        
        return (0);
}

SEXP_t *SEAP_packet_msg2sexp (SEAP_msg_t *msg)
{
        SEXP_t *sexp;
        char   *attr_name;
        size_t  attr_namelen;
        uint16_t i;
        
        _A(msg != NULL);
        _LOGCALL_;

        /* FIXME: memory leak */
        sexp = SEXP_list_new (SEXP_string_new (SEAP_SYM_MSG, strlen (SEAP_SYM_MSG)),
                              SEXP_string_new (":id", 3),
                              SEXP_number_newu_64 (msg->id),
                              NULL);
        
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

int SEAP_packet_sexp2cmd (SEXP_t *sexp_cmd, SEAP_cmd_t *seap_cmd)
{
        SEXP_t *item, *val;
        size_t i;
        int mattrs;

        _LOGCALL_;

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
                seap_cmd->code = SEXP_number_getu_16 (item);
        } else {
                errno = EINVAL;
                return (-1);
        }
        
        seap_cmd->args = SEXP_list_nth (sexp_cmd, ++i);
        
        return (0);
}

SEXP_t *SEAP_packet_cmd2sexp (SEAP_cmd_t *cmd)
{
        SEXP_t *sexp;
        
        _A(cmd != NULL);
        _LOGCALL_;

        sexp = SEXP_list_new (SEXP_string_new (SEAP_SYM_CMD, strlen (SEAP_SYM_CMD)),
                              SEXP_string_new (":id", 3),
                              SEXP_number_newu_16 (cmd->id),
                              NULL);
        
        if (cmd->flags & SEAP_CMDFLAG_REPLY) {
                SEXP_list_add (sexp,
                               SEXP_string_new (":reply_id", 9));
                SEXP_list_add (sexp,
                               SEXP_number_newu_16 (cmd->rid));
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
                       SEXP_number_newu_16 (cmd->code));
        
        if (cmd->args != NULL)
                SEXP_list_add (sexp, cmd->args);
        
        SEXP_VALIDATE(sexp);
        return (sexp);
}

int SEAP_packet_sexp2err (SEXP_t *sexp_err, SEAP_err_t *seap_err)
{
        _LOGCALL_;
        return (-1);
}

SEXP_t *SEAP_packet_err2sexp (SEAP_err_t *err)
{
        SEXP_t *sexp;

        _LOGCALL_;

        sexp = SEXP_list_new (SEXP_string_new (SEAP_SYM_ERR, strlen (SEAP_SYM_ERR)),
                              SEXP_string_new (":orig_id", 8),
                              SEXP_number_newu_64 (err->id),
                              SEXP_string_new (":type", 5),
                              SEXP_number_newu (err->type),
                              SEXP_number_newu (err->code),
                              NULL);
        
        if (err->data != NULL)
                SEXP_list_add (sexp, err->data);
        
        return (sexp);
}

SEXP_t *SEAP_packet2sexp (SEAP_packet_t *packet)
{
        SEXP_t *sexp = NULL;
        
        _LOGCALL_;
        
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
        const char *psym_cstr;
        
        _LOGCALL_;

        dsc = SEAP_desc_get (&(ctx->sd_table), sd);
        
        if (dsc == NULL)
                return (-1);
        
        /*
         * Check packet queue
         */
        if (pqueue_notempty (dsc->pck_queue)) {
                /* TODO */
        }

        if (dsc->sexpbuf != NULL) {
                if (SEXP_list_length (dsc->sexpbuf) > 0)
                        goto sexp_buf_recv;
        }
        
        psetup = SEXP_psetup_new ();
                
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
                
                _D("return from select\n");
                
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
                                _D("An error ocured while locking the read mutex: dsc=%p, errno=%u, %s.\n",
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
        
        for (;;) {
                data_buffer = sm_alloc (SEAP_RECVBUF_SIZE);
                data_buflen = SEAP_RECVBUF_SIZE;
                data_length = SCH_RECV(dsc->scheme, dsc, data_buffer, data_buflen, 0);
                
                if (data_length < 0) {
                        protect_errno {
                                _D("FAIL: recv failed: dsc=%p, errno=%u, %s.\n", dsc, errno, strerror (errno));
                                
                                sm_free (data_buffer);
                                SEXP_psetup_free (psetup);

                                if (pstate != NULL)
                                        SEXP_pstate_free (pstate);
                        }
                        return (-1);
                } else if (data_length == 0) {
                        _D("zero bytes received -> EOF\n");
                        sm_free (data_buffer);
                        SEXP_psetup_free (psetup);
                        
                        if (pstate != NULL) {
                                _D("FAIL: incomplete S-exp received\n");
                                SEXP_psetup_free (psetup);
                                errno = ENETRESET;
                                return (-1);
                        } else {
                                errno = ECONNRESET;
                                return (-1);
                        }
                }
                
                _A(data_length > 0);
                
                if (data_buflen != (size_t)(data_length))
                        data_buffer = sm_realloc (data_buffer, data_length);
                
                sexp_buffer = SEXP_parse (psetup, data_buffer, data_length, &pstate);
                
                if (sexp_buffer != NULL) {
                        _A(pstate == NULL);
                        
                        DESC_RUNLOCK(dsc);
                        
                        if (SEXP_list_length (sexp_buffer) > 0) {
                                break;
                        } else {
                                SEXP_list_free (sexp_buffer);
                                _D("eloop_restart\n");
                                goto eloop_start;
                        }
                }
                
                if (SCH_SELECT(dsc->scheme, dsc, SEAP_IO_EVREAD, ctx->recv_timeout, 0) != 0) {
                        switch (errno) {
                        case ETIMEDOUT:
                                protect_errno {
                                        _D("FAIL: recv failed (timeout): dsc=%p, time=%hu, errno=%u, %s.\n",
                                           dsc, ctx->recv_timeout, errno, strerror (errno));
                                }
                        default:
                                protect_errno {
                                        _D("FAIL: recv failed: dsc=%p, errno=%u, %s.\n",
                                           dsc, errno, strerror (errno));
                                        
                                        sm_free (data_buffer);
                                        SEXP_psetup_free (psetup);
                                        SEXP_pstate_free (pstate);
                                }
                                return (-1);
                        }
                }
        }
        
        SEXP_psetup_free (psetup);
        dsc->sexpbuf = sexp_buffer;
        
sexp_buf_recv:
        SEXP_VALIDATE(dsc->sexpbuf);
        sexp_packet = SEXP_list_pop (dsc->sexpbuf);
        
        _A(sexp_packet != NULL);

        { FILE *fp;
                
                fp = fopen ("packet-recv.log", "a");
                setbuf (fp, NULL);
                fprintf (fp, "--- PACKET ---\n");
                SEXP_fprintfa (fp, sexp_packet);
                fprintf (fp, "\n-----------\n");
                fclose (fp);
        }
        
        if (!SEXP_listp(sexp_packet)) {
                _D("Invalid SEAP packet received: %s.\n", "not a list");
                SEXP_free (sexp_packet);
                errno = EINVAL;
                return (-1);
        } else if (SEXP_list_length (sexp_packet) < 2) {
                _D("Invalid SEAP packet received: %s.\n", "list length < 2");
                SEXP_free (sexp_packet);
                errno = EINVAL;
                return (-1);
        }
        
        psym_sexp = SEXP_list_first (sexp_packet);
        
        if (!SEXP_stringp(psym_sexp)) {
                _D("Invalid SEAP packet received: %s.\n", "first list item is not a string");
                SEXP_free (sexp_packet);
                /* SEXP_free (psym_sexp) */
                errno = EINVAL;
                return (-1);
        } else if (SEXP_string_length (psym_sexp) != (strlen (SEAP_SYM_PREFIX) + 3)) {
                _D("Invalid SEAP packet received: %s.\n", "invalid packet type symbol length");
                SEXP_free (sexp_packet);
                /* SEXP_free (psym_sexp) */
                errno = EINVAL;
                return (-1);
        } else if (SEXP_strncmp (psym_sexp, SEAP_SYM_PREFIX, strlen (SEAP_SYM_PREFIX)) != 0) {
                _D("Invalid SEAP packet received: %s.\n", "invalid prefix");
                SEXP_free (sexp_packet);
                /* SEXP_free (psym_sexp) */
                errno = EINVAL;
                return (-1);
        }
        
        psym_cstr = SEXP_string_cstrp (psym_sexp) + strlen (SEAP_SYM_PREFIX);
        
        switch (psym_cstr[0]) {
        case 'm':
                if (psym_cstr[1] == 's' &&
                    psym_cstr[2] == 'g')
                {
                        (*packet) = SEAP_packet_new ();
                        (*packet)->type = SEAP_PACKET_MSG;

                        if (SEAP_packet_sexp2msg (sexp_packet, &((*packet)->data.msg)) != 0) {
                                /* error */
                                _D("Invalid SEAP packet received: %s.\n", "can't translate to msg struct");
                                return (-1);
                        }
                        break;
                }
                goto invalid;
        case 'c':
                if (psym_cstr[1] == 'm' &&
                    psym_cstr[2] == 'd')
                {
                        (*packet) = SEAP_packet_new ();
                        (*packet)->type = SEAP_PACKET_CMD;

                        if (SEAP_packet_sexp2cmd (sexp_packet, &((*packet)->data.cmd)) != 0) {
                                /* error */
                                _D("Invalid SEAP packet received: %s.\n", "can't translate to cmd struct");
                                return (-1);
                        }
                        break;
                }
                goto invalid;
        case 'e':
                if (psym_cstr[1] == 'r' &&
                    psym_cstr[2] == 'r')
                {
                        (*packet) = SEAP_packet_new ();
                        (*packet)->type = SEAP_PACKET_ERR;
                        
                        if (SEAP_packet_sexp2err (sexp_packet, &((*packet)->data.err)) != 0) {
                                /* error */
                                _D("Invalid SEAP packet received: %s.\n", "can't translate to err struct");
                                return (-1);
                        }
                        break;
                }
        default:
        invalid:
                _D("Invalid SEAP packet received: %s.\n", "invalid packet type symbol");
                SEXP_free (sexp_packet);
                errno = EINVAL;
                return (-1);
        }
        
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

        _LOGCALL_;
        
        dsc = SEAP_desc_get (&(ctx->sd_table), sd);
        
        if (dsc == NULL)
                return (-1);
        
        for (;;) {
                pck = NULL;
                ret = SEAP_packet_recv (ctx, sd, &pck);
        
                if (ret != 0)
                        return (ret);
                else if (pck->type == type)
                        break;
                else if (pqueue_add (dsc->pck_queue, pck) != 0)
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
                
        _LOGCALL_;

        ret = -1;
        dsc = SEAP_desc_get (&(ctx->sd_table), sd);
        
        if (dsc == NULL)
                return (-1);
        
        packet_sexp = SEAP_packet2sexp (packet);
        
        if (packet_sexp == NULL) {
                _D("Can't convert S-exp to packet\n");
                return (-1);
        }

        if (DESC_WLOCK (dsc)) {
                ret = 0;
                
                if (SCH_SENDSEXP(dsc->scheme, dsc, packet_sexp, 0) < 0) {
                        ret = -1;
                        
                        protect_errno {
                                _D("FAIL: errno=%u, %s.\n", errno, strerror (errno));
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
        _LOGCALL_;
        return (-1);
}
#endif
