/*
 * Copyright 2010 Red Hat Inc., Durham, North Carolina.
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

#include <seap.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
#include <stdlib.h>

#include "common/_error.h"
#include "common/alloc.h"
#include "common/assume.h"
#include "common/util.h"
#include "common/bfind.h"
#include "common/debug_priv.h"
#include "probes/public/probe-api.h"
#include "oval_probe_ext.h"
#include "oval_sexp.h"
#include "oval_probe_meta.h"

#define __ERRBUF_SIZE 128

static oval_pdtbl_t *oval_pdtbl_new(void);
static void          oval_pdtbl_free(oval_pdtbl_t *table);
static int           oval_pdtbl_add(oval_pdtbl_t *table, oval_subtype_t type, int sd, const char *uri);
static oval_pd_t    *oval_pdtbl_get(oval_pdtbl_t *table, oval_subtype_t type);

/*
 * oval_pext_
 */
oval_pext_t *oval_pext_new(void)
{
        oval_pext_t *pext;

        pext = oscap_talloc(oval_pext_t);

        pext->do_init = true;
        pthread_mutex_init(&pext->lock, NULL);

#if defined(OVAL_PROBEDIR_ENV)
        pext->probe_dir = getenv("OVAL_PROBE_DIR");
#else
        pext->probe_dir = NULL;
#endif
        if (pext->probe_dir == NULL)
                pext->probe_dir = OVAL_PROBE_DIR;

        pext->pdtbl     = NULL;
        pext->pdsc      = NULL;
        pext->pdsc_cnt  = 0;

        return(pext);
}

void oval_pext_free(oval_pext_t *pext)
{
        if (!pext->do_init) {
                /* free structs */
		free(pext->pdsc);
		pext->pdsc     = NULL;
		pext->pdsc_cnt = 0;
                oval_pdtbl_free(pext->pdtbl);
        }

        pthread_mutex_destroy(&pext->lock);
        free(pext);
}

/*
 * oval_pdtbl_
 */
static oval_pdtbl_t *oval_pdtbl_new(void)
{
	oval_pdtbl_t *p_tbl;

	p_tbl = oscap_talloc(oval_pdtbl_t);
	p_tbl->memb = NULL;
	p_tbl->count = 0;
	p_tbl->ctx = SEAP_CTX_new();

	return (p_tbl);
}

static void oval_pdtbl_free(oval_pdtbl_t *tbl)
{
        register size_t i;

        for (i = 0; i < tbl->count; ++i) {
                SEAP_close(tbl->ctx, tbl->memb[i]->sd);
                free(tbl->memb[i]->uri);
		free(tbl->memb[i]);
        }

        free(tbl->memb);
        SEAP_CTX_free(tbl->ctx);
        free(tbl);

	return;
}

static int oval_pdtbl_pdcmp(const oval_pd_t **a, const oval_pd_t **b)
{
	assume_d(*a != NULL, -1);
	assume_d(*b != NULL, -1);
	return ((*a)->subtype - (*b)->subtype);
}

static int oval_pdtbl_typecmp(oval_subtype_t *a, oval_pd_t **b)
{
	assume_d(a != NULL, -1);
	assume_d(*b != NULL, -1);
        return (*a - (*b)->subtype);
}

static int oval_pdtbl_add(oval_pdtbl_t *tbl, oval_subtype_t type, int sd, const char *uri)
{
	oval_pd_t *pd;

	assume_d (tbl != NULL, -1);
	assume_d (uri != NULL, -1);

	pd = oscap_talloc(oval_pd_t);
	pd->subtype = type;
	pd->sd      = sd;
	pd->uri     = oscap_strdup(uri);

	tbl->memb = realloc(tbl->memb, sizeof(oval_pd_t *) * (++tbl->count));

	assume_d(tbl->memb != NULL, -1);

	tbl->memb[tbl->count - 1] = pd;

	qsort(tbl->memb, tbl->count, sizeof(oval_pd_t *),
              (int (*)(const void *, const void *))oval_pdtbl_pdcmp);

	return (0);
}

static oval_pd_t *oval_pdtbl_get(oval_pdtbl_t * tbl, oval_subtype_t type)
{
	oval_pd_t **pdp;

	pdp = oscap_bfind(tbl->memb, tbl->count, sizeof(oval_pd_t *),
			  &type, (int (*)(void *, void *))oval_pdtbl_typecmp);

	return (pdp == NULL ? NULL : *pdp);
}

/*
 * oval_probe_cmd_
 */
static SEXP_t *oval_probe_cmd_obj_eval(SEXP_t *sexp, void *arg);
static SEXP_t *oval_probe_cmd_ste_fetch(SEXP_t *sexp, void *arg);
static int     oval_probe_cmd_init(oval_pext_t *pext);

static int oval_probe_cmd_init(oval_pext_t *pext)
{
        assume_d (pext != NULL, -1);

	if (SEAP_cmd_register(pext->pdtbl->ctx, PROBECMD_OBJ_EVAL, SEAP_CMDREG_USEARG,
                              &oval_probe_cmd_obj_eval, (void *)pext) != 0)
        {
		dE("Can't register command: %s: errno=%u, %s.", "obj_eval", errno, strerror(errno));
		return (-1);
	}

	if (SEAP_cmd_register(pext->pdtbl->ctx, PROBECMD_STE_FETCH, SEAP_CMDREG_USEARG,
			      &oval_probe_cmd_ste_fetch, (void *)pext) != 0) {
		dE("Can't register command: %s: errno=%u, %s.", "ste_fetch", errno, strerror(errno));

		/* FIXME: unregister the first command */

		return (-1);
	}

	return (0);
}

static SEXP_t *oval_probe_cmd_obj_eval(SEXP_t *sexp, void *arg)
{
	char *id_str;
	struct oval_definition_model *defs;
	struct oval_object  *obj;
	struct oval_syschar *res;
	oval_pext_t *pext = (oval_pext_t *) arg;
	SEXP_t *ret, *ret_code;
	int r;

        assume_d (sexp != NULL, NULL);
        assume_d (arg  != NULL, NULL);

	if (!SEXP_stringp(sexp)) {
		dE("Invalid argument: type=%s.", SEXP_strtype(sexp));
		return (NULL);
	}

	id_str = SEXP_string_cstr(sexp);
	defs   = oval_syschar_model_get_definition_model(*(pext->model));
	obj    = oval_definition_model_get_object(defs, id_str);
	ret    = SEXP_list_new (sexp, NULL);

	dI("Get_object: %s.", id_str);

	if (obj == NULL) {
		dE("Can't find obj: id=%s.", id_str);
		free(id_str);
                SEXP_free(ret);

		return (NULL);
	}

	oscap_clearerr();
	r = oval_probe_query_object(pext->sess_ptr, obj, OVAL_PDFLAG_NOREPLY|OVAL_PDFLAG_SLAVE, &res);
	if (r < 0)
		ret_code = SEXP_number_newu((unsigned int) SYSCHAR_FLAG_COMPLETE);
	else
		ret_code = SEXP_number_newu((unsigned int) oval_syschar_get_flag(res));

	SEXP_list_add(ret, ret_code);
	SEXP_free(ret_code);

	if (oscap_err()) {
		dE("Failed: id: %s, err: %d, %s.",
			       id_str, oscap_err_family(), oscap_err_desc());
		oscap_clearerr();
		free(id_str);
		SEXP_free(ret);

		return (NULL);
	}

	free(id_str);

	return (ret);
}

static SEXP_t *oval_probe_cmd_ste_fetch(SEXP_t *sexp, void *arg)
{
	SEXP_t *id, *ste_list, *ste_sexp;
	char *id_str;
	struct oval_state *ste;
	struct oval_definition_model *definition_model;
	oval_pext_t *pext = (oval_pext_t *)arg;
	int ret;

        assume_d (sexp != NULL, NULL);
        assume_d (arg  != NULL, NULL);

	ste_list = SEXP_list_new(NULL);

	SEXP_list_foreach(id, sexp) {
		if (SEXP_stringp(id)) {
			id_str = SEXP_string_cstr(id);
			definition_model = oval_syschar_model_get_definition_model(*(pext->model));
			ste = oval_definition_model_get_state(definition_model, id_str);

			if (ste == NULL) {
				dE("Can't find ste: id: %s.", id_str);
				SEXP_list_free(ste_list);
				free(id_str);
                                SEXP_free(id);

				return (NULL);
			}

			ret = oval_state_to_sexp(pext->sess_ptr, ste, &ste_sexp);
			if (ret !=0) {
				dE("Failed to convert OVAL state to SEXP, id: %s.",
					       id_str);
				SEXP_list_free(ste_list);
				free(id_str);
                                SEXP_free(id);

				return (NULL);
			}

			SEXP_list_add(ste_list, ste_sexp);
                        SEXP_free(ste_sexp);

			free(id_str);
		}
	}

	return (ste_list);
}

static inline const char *_probe_strerror(uint32_t error_code)
{
	const char *codemsg;

	/*
	 * Errors of type USER should all be from the probe "namespace" (i.e. only codes
	 * defined at public/probe-api.h.
	 */
	switch (error_code) {
	case PROBE_EINVAL: codemsg = "Invalid type, value or format";
		break;
	case PROBE_ENOELM: codemsg = "Missing element";
		break;
	case PROBE_ENOVAL: codemsg = "Missing value";
		break;
	case PROBE_ENOATTR: codemsg = "Missing attribute";
		break;
	case PROBE_EINIT: codemsg = "Initialization failed";
		break;
	case PROBE_ENOMEM: codemsg = "Insufficient memory";
		break;
	case PROBE_EOPNOTSUPP: codemsg = "Operation not supported";
		break;
	case PROBE_ERANGE: codemsg = "Value out of range";
		break;
	case PROBE_EDOM: codemsg = "Value out of domain";
		break;
	case PROBE_EFAULT: codemsg = "Memory fault or NULL value";
		break;
	case PROBE_EACCESS: codemsg = "Operation not permitted";
		break;
	case PROBE_ESETEVAL: codemsg = "Set evaluation failed";
		break;
	case PROBE_ENOENT: codemsg = "Missing entity";
		break;
	case PROBE_EFATAL: codemsg = "Unrecoverable error";
		break;
	case PROBE_EUNKNOWN:
	default:
		codemsg = "Unknown error";
	}

	return codemsg;
}

static inline int _handle_SEAP_receive_failure(SEAP_CTX_t *ctx, oval_pd_t *pd, SEAP_msg_t *s_omsg, int flags)
{
	protect_errno {
		dW("Can't receive message: %u, %s.", errno, strerror(errno));
	}

	if (errno == ECANCELED) {
		SEAP_err_t *err = NULL;

		switch(SEAP_recverr_byid(ctx, pd->sd, &err,
					 SEAP_msg_id(s_omsg)))
		{
		case  0:
			break;
		case  1: /* no error found */
			dE("Internal error: An error was signaled on sd=%d but the error queue is empty.");
			oscap_seterr(OSCAP_EFAMILY_OVAL, "SEAP_recverr_byid: internal error: empty error queue.");
			return (-1);
		case -1: /* internal error */
			dE("Internal error: SEAP_recverr_byid returned -1");
			oscap_seterr(OSCAP_EFAMILY_OVAL, "SEAP_recverr_byid: internal error.");
			return (-1);
		}

		/*
		 * decide what to do based on the error code/type
		 */
		switch (err->type) {
		case SEAP_ETYPE_USER:
		{
			oscap_seterr(OSCAP_EFAMILY_OVAL, "Probe at sd=%d (%s) reported an error: %s",
					pd->sd, oval_subtype_to_str(pd->subtype), _probe_strerror(err->code));
			break;
		}
		case SEAP_ETYPE_INT:
			oscap_seterr(OSCAP_EFAMILY_OVAL, "Internal error");
			break;
		}

		SEAP_error_free(err);
		return (-1);
	}

	if (flags & OVAL_PDFLAG_SLAVE) {
		char errbuf[__ERRBUF_SIZE];

		if (oscap_strerror_r (errno, errbuf, sizeof errbuf - 1) != 0)
			oscap_seterr (OSCAP_EFAMILY_OVAL, "Unable to receive a message to probe");
		else
			oscap_seterr (OSCAP_EFAMILY_OVAL, errbuf);

		return (-1);
	}

	if (SEAP_close(ctx, pd->sd) != 0) {
		char errbuf[__ERRBUF_SIZE];

		protect_errno {
			dE("Can't close sd: %u, %s.", errno, strerror(errno));
		}

		if (oscap_strerror_r (errno, errbuf, sizeof errbuf - 1) != 0)
			oscap_seterr (OSCAP_EFAMILY_OVAL, "Unable to close probe sd");
		else
			oscap_seterr (OSCAP_EFAMILY_OVAL, errbuf);
	}

	pd->sd = -1;
	return (-1);
}

static int oval_probe_comm(SEAP_CTX_t *ctx, oval_pd_t *pd, const SEXP_t *s_iobj, int flags, SEXP_t **out_sexp)
{
	int retry, ret;

	SEAP_msg_t *s_imsg, *s_omsg;
	SEXP_t *s_oobj;

	assume_d (pd != NULL, -1);
        assume_d (s_iobj != NULL, -1);

	for (retry = 0;;) {
		/*
		 * Establish connection to probe. The connection may be
		 * already set up by previous calls to this function or
		 * by the probe context handling functions.
		 */
		if (pd->sd == -1) {
			pd->sd = SEAP_connect(ctx, pd->uri, 0);

			if (pd->sd < 0) {
                                protect_errno {
                                        dW("Can't connect: %u, %s.", errno, strerror(errno));
                                }

				if (++retry <= OVAL_PROBE_MAXRETRY) {
					dI("Connect: retry %u/%u.", retry, OVAL_PROBE_MAXRETRY);
					continue;
				} else {
                                        char errbuf[__ERRBUF_SIZE];

                                        protect_errno {
                                                dE("Connect: retry limit (%u) reached.", OVAL_PROBE_MAXRETRY);
                                        }

                                        if (oscap_strerror_r (errno, errbuf, sizeof errbuf - 1) != 0)
                                                oscap_seterr (OSCAP_EFAMILY_OVAL, "Can't connect to the probe");
                                        else
                                                oscap_seterr (OSCAP_EFAMILY_OVAL, errbuf);

					return (-1);
				}
			}
		}

		s_omsg = SEAP_msg_new();
		SEAP_msg_set(s_omsg, (SEXP_t *) s_iobj);

		if (flags & OVAL_PDFLAG_NOREPLY) {
			if (SEAP_msgattr_set(s_omsg, "no-reply", NULL) != 0) {
                                protect_errno {
                                        dE("Can't set no-reply attribute.");
                                }

                                SEAP_msg_free(s_omsg);
                                oscap_seterr (OSCAP_EFAMILY_OVAL, "OVAL_EPROBEUNKNOWN");

				return (-1);
			}
		}

		dD("Sending message.");

		ret = SEAP_sendmsg(ctx, pd->sd, s_omsg);
		if (ret != 0) {
                        protect_errno {
                                dW("Can't send message: %u, %s.", errno, strerror(errno));
                        }

			if (flags & OVAL_PDFLAG_SLAVE) {
                                char errbuf[__ERRBUF_SIZE];

                                if (oscap_strerror_r (errno, errbuf, sizeof errbuf - 1) != 0)
                                        oscap_seterr (OSCAP_EFAMILY_OVAL, "Unable to send a message to probe");
                                else
					oscap_seterr (OSCAP_EFAMILY_OVAL, errbuf);

				SEAP_msg_free(s_omsg);
				return (-1);
			}

			if (SEAP_close(ctx, pd->sd) != 0) {
                                char errbuf[__ERRBUF_SIZE];

                                protect_errno {
                                        dE("Can't close sd: %u, %s.", errno, strerror(errno));
                                        SEAP_msg_free(s_omsg);
                                }

                                if (oscap_strerror_r (errno, errbuf, sizeof errbuf - 1) != 0)
                                        oscap_seterr (OSCAP_EFAMILY_OVAL, "Can't close sd");
                                else
                                        oscap_seterr (OSCAP_EFAMILY_OVAL, errbuf);

				pd->sd = -1;
				return (-1);
			}

			pd->sd = -1;

			if (++retry <= OVAL_PROBE_MAXRETRY) {
				dI("Send: retry %u/%u.", retry, OVAL_PROBE_MAXRETRY);
				continue;
			} else {
                                char errbuf[__ERRBUF_SIZE];

                                protect_errno {
                                        dE("Send: retry limit (%u) reached.", OVAL_PROBE_MAXRETRY);
                                        SEAP_msg_free(s_omsg);
                                }

                                if (oscap_strerror_r (errno, errbuf, sizeof errbuf - 1) != 0)
                                        oscap_seterr (OSCAP_EFAMILY_OVAL, "Unable to send a message to probe");
                                else
                                        oscap_seterr (OSCAP_EFAMILY_OVAL, errbuf);

				return (ret);
			}
		}

		dD("Waiting for reply.");

		/* recv_retry: */
		s_imsg = NULL;

		ret = SEAP_recvmsg(ctx, pd->sd, &s_imsg);
		if (ret != 0) {
			protect_errno {
				ret = _handle_SEAP_receive_failure(ctx, pd, s_omsg, flags);
				SEAP_msg_free(s_imsg);
				SEAP_msg_free(s_omsg);
			}
			if (errno == ECONNABORTED) {
				dI("Connection was aborted.");
				return (-2);
			} else {
				if (++retry <= OVAL_PROBE_MAXRETRY) {
					dI("Recv: retry %u/%u.", retry, OVAL_PROBE_MAXRETRY);
					continue;
				} else {
					protect_errno {
						dE("Recv: retry limit (%u) reached.", OVAL_PROBE_MAXRETRY);
					}

					char errbuf[__ERRBUF_SIZE];
					if (oscap_strerror_r (errno, errbuf, sizeof errbuf - 1) == 0)
						oscap_seterr(OSCAP_EFAMILY_OVAL, errbuf);
					oscap_seterr(OSCAP_EFAMILY_OVAL, "Unable to receive a message from probe");

					return ret;
				}
			}
		}

		dD("Message received.");
		break;
	}

	s_oobj = SEAP_msg_get(s_imsg);

	SEAP_msg_free(s_imsg);
	SEAP_msg_free(s_omsg);

	*out_sexp = s_oobj;
	return (0);
}

static int oval_pdsc_typecmp(oval_subtype_t *a, oval_pdsc_t *b)
{
        return (*a - b->type);
}

static int oval_pdsc_cmp(oval_pdsc_t *a, oval_pdsc_t *b)
{
	return (a->type - b->type);
}

static oval_pdsc_t *oval_pdsc_lookup(oval_pdsc_t pdsc[], int count, oval_subtype_t type)
{
	return oscap_bfind(pdsc, count, sizeof(oval_pdsc_t), &type,
                           (int (*)(void *, void *))oval_pdsc_typecmp);
}

static int oval_probe_sys_eval(SEAP_CTX_t *ctx, oval_pd_t *pd, struct oval_syschar_model *model, struct oval_sysinfo **out_sysinf)
{
	struct oval_sysinfo *sysinf;
	struct oval_sysint *ife;
	SEXP_t *s_obj, *s_sinf, *ent, *r0, *r1;
	int ret;

	/*
	 * Prepare a dummy object. We can't simply send an empty object
	 * because the preprocessing machinery in probes need an id that
	 * is used as key in cache lookups.
	 */
        {
                SEXP_t *r2, *r3;

                r0 = SEXP_list_new (r1 = SEXP_string_newf ("%s", "sysinfo_object"),
                                    r2 = SEXP_string_newf (":%s", "id"),
                                    r3 = SEXP_string_newf ("sysinfo:0"),
                                    NULL);

				SEXP_free(r1);
				SEXP_free(r2);
				SEXP_free(r3);
                s_obj = SEXP_list_new (r0, NULL);
                SEXP_free (r0);
        }

        ret = oval_probe_comm(ctx, pd, s_obj, 0, &r0);
        SEXP_free(s_obj);

	if (ret != 0)
		return (ret);

	r1 = probe_cobj_get_items(r0);
	s_sinf = SEXP_list_first(r1);
	SEXP_free(r0);
	SEXP_free(r1);

	if (s_sinf == NULL)
		return (-1);

	sysinf = oval_sysinfo_new(model);

	/*
	 * Translate S-exp to sysinfo structure
	 */
#define SYSINF_EXT(obj, name, sysinf, fail)                             \
        do {                                                            \
                SEXP_t *val;                                            \
                char    buf[128+1];                                     \
                                                                        \
                val = probe_obj_getentval (obj, #name, 1);     \
                                                                        \
                if (val == NULL) {                                      \
                        dI("No entity or value: %s", #name); \
                        goto fail;                                      \
                }                                                       \
                                                                        \
                if (SEXP_string_cstr_r (val, buf, sizeof buf) >= sizeof buf) { \
                        dI("Value too large: %s", #name);    \
                        SEXP_free (val);                                \
                        goto fail;                                      \
                }                                                       \
                                                                        \
                oval_sysinfo_set_##name (sysinf, buf);                  \
                SEXP_free (val);                                        \
        } while (0)

	SYSINF_EXT(s_sinf, os_name, sysinf, fail_gen);
	SYSINF_EXT(s_sinf, os_version, sysinf, fail_gen);
	SYSINF_EXT(s_sinf, os_architecture, sysinf, fail_gen);
	SYSINF_EXT(s_sinf, primary_host_name, sysinf, fail_gen);

	/*
	 * Extract interface info
	 */
	{
		uint32_t n;

		for (n = 1; (ent = probe_obj_getent(s_sinf, "interface", n)) != NULL; ++n) {
			ife = oval_sysint_new(model);

#define SYSINF_IEXT(ent, name, sysint, fail)                            \
                        do {                                            \
                                SEXP_t *val;                            \
                                char    buf[128+1];                     \
                                                                        \
                                val = probe_ent_getattrval (ent, #name); \
                                                                        \
                                if (val == NULL) {                      \
                                        dI("No value: %s", #name); \
                                        goto fail;                      \
                                }                                       \
                                                                        \
                                if (SEXP_string_cstr_r (val, buf, sizeof buf) >= sizeof buf) { \
                                        dI("Value too large: %s", #name); \
                                        SEXP_free (val);                \
                                        goto fail;                      \
                                }                                       \
                                                                        \
                                oval_sysint_set_##name (sysint, buf);   \
                                SEXP_free (val);                        \
                                                                        \
                        } while (0)

			SYSINF_IEXT(ent, ip_address, ife, fail_int);
			SYSINF_IEXT(ent, mac_address, ife, fail_int);
			SYSINF_IEXT(ent, name, ife, fail_int);

			oval_sysinfo_add_interface(sysinf, ife);
			oval_sysint_free(ife);
			SEXP_free(ent);
		}
	}

	SEXP_free(s_sinf);

	*out_sysinf = sysinf;
	return (0);
 fail_int:
	SEXP_free(ent);
	oval_sysint_free(ife);
 fail_gen:
	SEXP_free(s_sinf);
	oval_sysinfo_free(sysinf);

	return (-1);
}

int oval_probe_sys_handler(oval_subtype_t type, void *ptr, int act, ...)
{
        int ret = 0;
        oval_pext_t *pext = (oval_pext_t *)ptr;
        va_list ap;
        oval_pd_t *pd;

        va_start(ap, act);

        switch(act) {
        case PROBE_HANDLER_ACT_EVAL:
        {
                struct oval_sysinfo **inf;
                /* int flags = va_arg(ap, int); */

                va_arg(ap, struct oval_object *);
                inf = va_arg(ap, struct oval_sysinfo **);
                pd  = oval_pdtbl_get(pext->pdtbl, type);

                if (pd == NULL) {
                        if (oval_probe_sys_handler(type, ptr, PROBE_HANDLER_ACT_OPEN) != 0) {
				va_end(ap);
                                return(-1);
			}

                        pd = oval_pdtbl_get(pext->pdtbl, type);
                }

                assume_r(pd != NULL, -1);
		ret = oval_probe_sys_eval(pext->pdtbl->ctx, pd, *(pext->model), inf);
                break;
        }
        case PROBE_HANDLER_ACT_OPEN:
        {
                char         probe_uri[PATH_MAX + 1];
                size_t       probe_urilen;
                char        *probe_dir;
                oval_pdsc_t *probe_dsc;

                probe_dir = pext->probe_dir;
                probe_dsc = oval_pdsc_lookup(pext->pdsc, pext->pdsc_cnt, type);

		if (probe_dsc == NULL) {
			oscap_seterr (OSCAP_EFAMILY_OVAL, "subtype %u not supported", type);

			ret = -1;
			break;
		}

                probe_urilen = snprintf(probe_uri, sizeof probe_uri,
                                        "%s://%s/%s", OVAL_PROBE_SCHEME, probe_dir, probe_dsc->file);

                if (probe_urilen >= sizeof probe_uri) {
                        oscap_seterr (OSCAP_EFAMILY_GLIBC, "probe URI too long");

                        ret = -1;
                } else {
                        dI("Starting probe on URI '%s'.", probe_uri);

                        if (oval_pdtbl_add(pext->pdtbl, type, -1, probe_uri) != 0) {
                                oscap_seterr (OSCAP_EFAMILY_OVAL, "%s probe not supported", probe_dsc->name);

                                ret = -1;
                        }
                }
                break;
        }
        case PROBE_HANDLER_ACT_INIT:
                ret = oval_probe_ext_init(pext);
                break;
        default:
                errno = EINVAL;
                ret   = -1;
        }

        va_end(ap);
        return(ret);
}

int oval_probe_ext_handler(oval_subtype_t type, void *ptr, int act, ...)
{
        int          ret = 0;
        va_list      ap;
        oval_pext_t *pext = (oval_pext_t *)ptr;
        oval_pd_t   *pd;

        va_start(ap, act);

        switch(act) {
        case PROBE_HANDLER_ACT_EVAL:
        {
		struct oval_object *obj;
		struct oval_syschar *sys;
		int flags;

		sys = va_arg(ap, struct oval_syschar *);
		flags = va_arg(ap, int);
		obj = oval_syschar_get_object(sys);
                pd = oval_pdtbl_get(pext->pdtbl, oval_object_get_subtype(obj));

                if (pd == NULL) {
                        char         probe_uri[PATH_MAX + 1];
                        size_t       probe_urilen;
                        char        *probe_dir;
                        oval_pdsc_t *probe_dsc;

                        probe_dir = pext->probe_dir;
                        probe_dsc = oval_pdsc_lookup(pext->pdsc, pext->pdsc_cnt, oval_object_get_subtype(obj));

			if (probe_dsc == NULL) {
				oval_syschar_add_new_message(sys, "OVAL object not supported", OVAL_MESSAGE_LEVEL_WARNING);
				oval_syschar_set_flag(sys, SYSCHAR_FLAG_NOT_COLLECTED);
				va_end(ap);
				return (1);
			}

                        probe_urilen = snprintf(probe_uri, sizeof probe_uri,
                                                "%s://%s/%s", OVAL_PROBE_SCHEME, probe_dir, probe_dsc->file);

                        if (probe_urilen >= sizeof probe_uri) {
                                oscap_seterr (OSCAP_EFAMILY_GLIBC, "probe URI too long");
				va_end(ap);
                                return (-1);
                        }

                        dI("Starting probe on URI '%s'.", probe_uri);

                        if (oval_pdtbl_add(pext->pdtbl, oval_object_get_subtype(obj), -1, probe_uri) != 0) {
				oval_syschar_add_new_message(sys, "OVAL object not supported", OVAL_MESSAGE_LEVEL_WARNING);
				oval_syschar_set_flag(sys, SYSCHAR_FLAG_NOT_COLLECTED);
				va_end(ap);
                                return (1);
			}

			pd = oval_pdtbl_get(pext->pdtbl, oval_object_get_subtype(obj));

                        if (pd == NULL) {
                                oscap_seterr (OSCAP_EFAMILY_OVAL, "internal error");
				va_end(ap);
                                return (-1);
                        }
                }

		ret = oval_probe_ext_eval(pext->pdtbl->ctx, pd, pext, sys, flags);

		if (ret >= 0)
			ret = 0;

		if (ret < 0 && errno == ECONNABORTED) {
			if (!(flags & OVAL_PDFLAG_SLAVE)) {
				if (!pext->do_init) {
					oval_pdtbl_free(pext->pdtbl);
				}

				pext->do_init  = true;
				pext->pdtbl    = NULL;
				pext->pdsc     = NULL;
				pext->pdsc_cnt = 0;

				oval_probe_ext_init(pext);

				errno = ECONNABORTED;
			}
		}

		va_end(ap);
		return ret;
        }
        case PROBE_HANDLER_ACT_OPEN:
                break;
        case PROBE_HANDLER_ACT_INIT:
                ret = oval_probe_ext_init(pext);
                break;
        case PROBE_HANDLER_ACT_RESET:
	case PROBE_HANDLER_ACT_ABORT:
        {
                if (type == OVAL_SUBTYPE_ALL) {
                        /*
                         * Iterate thru probe descriptor table and execute the reset operation
                         * for each probe descriptor.
                         */
                        for (size_t i = 0; i < pext->pdtbl->count; ++i) {
                                pd  = pext->pdtbl->memb[i];

				if (pd == NULL) {
					va_end(ap);
					return(0);
				}

				if (act == PROBE_HANDLER_ACT_RESET)
					ret = oval_probe_ext_reset(pext->pdtbl->ctx, pd, pext);
				else
					ret = oval_probe_ext_abort(pext->pdtbl->ctx, pd, pext);

				if (ret != 0) {
					va_end(ap);
					return (ret);
				}
                        }

			va_end(ap);
                        return(0);
                } else {
                        /*
                         * Reset only the probe of specified subtype.
                         */
                        pd = oval_pdtbl_get(pext->pdtbl, type);

			va_end(ap);
                        if (pd == NULL) 
                                return(0);

			if (act == PROBE_HANDLER_ACT_RESET)
				return oval_probe_ext_reset(pext->pdtbl->ctx, pd, pext);
			else
				return oval_probe_ext_abort(pext->pdtbl->ctx, pd, pext);
                }
                break;
        }
        case PROBE_HANDLER_ACT_FREE:
        case PROBE_HANDLER_ACT_CLOSE:
        default:
                va_end(ap);
                errno = EINVAL;
                return(-1);
        }

        va_end(ap);
        return(ret);
}

int oval_probe_ext_init(oval_pext_t *pext)
{
        int ret = 0;

        pthread_mutex_lock(&pext->lock);

        if (pext->do_init) {
		char curdir[PATH_MAX];
		struct stat st;
		register unsigned int i, r;

		if (getcwd(curdir, PATH_MAX) == NULL) {
			dE("getcwd() failed");
                        ret = -1;
                        goto _ret;
		}

		if (chdir(pext->probe_dir) != 0) {
			dE("Can't chdir to \"%s\"", pext->probe_dir);
                        ret = -1;
                        goto _ret;
		}

		pext->pdsc = malloc(sizeof(oval_pdsc_t) * OSCAP_GSYM(__probe_meta_count));

                dD("__probe_meta_count = %zu", OSCAP_GSYM(__probe_meta_count));

		for (r = 0, i = 0; i < OSCAP_GSYM(__probe_meta_count); ++i) {
                        if (!(OSCAP_GSYM(__probe_meta)[i].flags & OVAL_PROBEMETA_EXTERNAL)) {
                                dD("skipped: %s (not an external probe)", OSCAP_GSYM(__probe_meta)[i].stype);
                                continue;
                        }

			if (stat(OSCAP_GSYM(__probe_meta)[i].pname, &st) != 0) {
				dD("skipped: %s (stat failed, errno=%d)", OSCAP_GSYM(__probe_meta)[i].stype, errno);
				continue;
			}

			if (!S_ISREG(st.st_mode)) {
				dD("skipped: %s (not a regular file)", OSCAP_GSYM(__probe_meta)[i].stype);
				continue;
			}

                        pext->pdsc[r].type = OSCAP_GSYM(__probe_meta)[i].otype;
                        pext->pdsc[r].name = OSCAP_GSYM(__probe_meta)[i].stype;
                        pext->pdsc[r].file = OSCAP_GSYM(__probe_meta)[i].pname;

			++r;
		}

		if (r < OSCAP_GSYM(__probe_meta_count))
			pext->pdsc = realloc(pext->pdsc, sizeof(oval_pdsc_t) * r);

		pext->pdsc_cnt = r;
		qsort(pext->pdsc, pext->pdsc_cnt, sizeof(oval_pdsc_t),
		      (int(*)(const void *, const void *))oval_pdsc_cmp);

		if (chdir(curdir) != 0) {
			dE("Can't chdir back to \"%s\"", curdir);
			free(pext->pdsc);
			pext->pdsc_cnt = 0;
                        ret = -1;
                        goto _ret;
		}

                pext->pdtbl = oval_pdtbl_new();

                if (oval_probe_cmd_init(pext) != 0)
                        ret = -1;
                else
                        pext->do_init = false;
        }
_ret:
        pthread_mutex_unlock(&pext->lock);

        return(ret);
}

int oval_probe_ext_eval(SEAP_CTX_t *ctx, oval_pd_t *pd, oval_pext_t *pext, struct oval_syschar *syschar, int flags)
{
        SEXP_t *s_obj, *s_sys;
	struct oval_object *object;
	int ret;

	if (syschar == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OVAL, "Internal error: syschar == NULL");
		return (-1);
	}

	object = oval_syschar_get_object(syschar);
	ret = oval_object_to_sexp(pext->sess_ptr, oval_subtype_to_str(oval_object_get_subtype(object)), syschar, &s_obj);

	if (ret != 0)
		return (1);

	ret = oval_probe_comm(ctx, pd, s_obj, flags, &s_sys);
	SEXP_free(s_obj);

	if (ret != 0) {
		switch (errno) {
		case ECONNABORTED:
			dI("Closing sd=%d (pd=%p) after abort", pd->sd, pd);

			SEAP_close(ctx, pd->sd);
			pd->sd = -1;
			errno  = ECONNABORTED;
		}
		return (ret);
	}

	if (flags & OVAL_PDFLAG_NOREPLY) {
		if (s_sys != NULL) {
                        /*
                         * The no-reply flag is set and oval_probe_comm returned some
                         * data. This is considered a non-fatal error.
                         */
                        dW("Obtrusive data from probe!");
                        SEXP_free(s_sys);
		}
		return (0);
	}

        /*
	 * Convert the received S-exp to OVAL system characteristic.
	 */
	ret = oval_sexp_to_sysch(s_sys, syschar);
	SEXP_free(s_sys);

	return (ret);
}

int oval_probe_ext_reset(SEAP_CTX_t *ctx, oval_pd_t *pd, oval_pext_t *pext)
{
        SEAP_cmd_exec(ctx, pd->sd, SEAP_EXEC_RECV, PROBECMD_RESET, NULL, SEAP_CMDTYPE_SYNC, NULL, NULL);

        return (0);
}

#include <signal.h>
#include "SEAP/_seap-types.h"
#include "SEAP/seap-descriptor.h"
#include "SEAP/_seap-scheme.h"
#include "SEAP/sch_pipe.h"

int oval_probe_ext_abort(SEAP_CTX_t *ctx, oval_pd_t *pd, oval_pext_t *pext)
{
	SEAP_desc_t *dsc;
	/*
	 * Send SIGUSR1 to the probe
	 */

	assume_d(ctx  != NULL, -1);
	assume_d(pd   != NULL, -1);
	assume_d(pext != NULL, -1);

	dI("Sending abort to sd=%d", pd->sd);

	dsc = SEAP_desc_get(ctx->sd_table, pd->sd);

	if (dsc == NULL)
		return (-1);

	switch (dsc->scheme) {
	case SCH_PIPE:
	{
#ifdef _WIN32
		dE("Operation oval_probe_ext_abort is not supported on Windows!");
#else
		sch_pipedata_t *pipeinfo = (sch_pipedata_t *)dsc->scheme_data;
		dI("Sending SIGUSR1 to pid=%u", pipeinfo->pid);
		if (kill(pipeinfo->pid, SIGUSR1) != 0)
			dW("kill(SIGUSR1, %u): %u, %s", errno, strerror(errno));
#endif
		break;
	}
	default:
		return (-1);
	}

	return (0);
}

const char *oval_probe_ext_getdir(void)
{
    const char *probe_dir;

#if defined(OVAL_PROBEDIR_ENV)
    probe_dir = getenv("OVAL_PROBE_DIR");
#else
    probe_dir = NULL;
#endif
    if (probe_dir == NULL)
	probe_dir = OVAL_PROBE_DIR;

    return (probe_dir);
}
