
/*
 * Copyright 2008 Red Hat Inc., Durham, North Carolina.
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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/cdefs.h>
#include <seap.h>
#include <assert.h>
#include <errno.h>
#include <common/bfind.h>
#include "common/public/debug.h"
#include "common/public/error.h"
#if defined(OSCAP_THREAD_SAFE)
# include <pthread.h>
#endif

#include "oval_sexp.h"
#include "oval_probe_impl.h"
#include "oval_system_characteristics_impl.h"
#include "probes/public/probe-api.h"
#include "../common/util.h"

#ifndef __XCONCAT
# include <sys/cdefs.h>
# define __XCONCAT(a, b) __CONCAT(a,b)
#endif

encache_t *OSCAP_GSYM(encache) = NULL;

#define protect_errno for (int __XCONCAT(__e,__LINE__) = errno, __XCONCAT(__s,__LINE__) = 1; \
                           __XCONCAT(__s,__LINE__)-- ; errno = __XCONCAT(__e,__LINE__))

#define __ERRBUF_SIZE 128

/* KEEP THIS LIST SORTED! (by subtype) */
static const oval_pdsc_t __ovalp_ltable[] = {
	/*  7001 */ {OVAL_INDEPENDENT_FAMILY,               "family",            "probe_family"},
	/*  7006 */ {OVAL_INDEPENDENT_TEXT_FILE_CONTENT_54, "textfilecontent54", "probe_textfilecontent54"},
	/*  7007 */ {OVAL_INDEPENDENT_TEXT_FILE_CONTENT,    "textfilecontent",   NULL},
	/*  7010 */ {OVAL_INDEPENDENT_XML_FILE_CONTENT,     "xmlfilecontent",    "probe_xmlfilecontent"},
	/*  7999 */ {OVAL_INDEPENDENT_SYSCHAR_SUBTYPE,      "system_info",       "probe_system_info"},
	/*  9001 */ {OVAL_LINUX_DPKG_INFO,                  "dpkginfo",          "probe_dpkginfo"},
	/*  9002 */ {OVAL_LINUX_INET_LISTENING_SERVERS,     "inetlisteningservers", "probe_inetlisteningservers"},
	/*  9003 */ {OVAL_LINUX_RPM_INFO,                   "rpminfo",           "probe_rpminfo"},
//      /*  9004 */ { OVAL_LINUX_SLACKWARE_PKG_INFO_TEST,    "slackwarepkginfo",  "probe_slackwarepkginfo"  },
	/* 13001 */ {OVAL_UNIX_FILE,                        "file",              "probe_file"},
	/* 13003 */ {OVAL_UNIX_INTERFACE,                   "interface",         "probe_interface"},
	/* 13004 */ {OVAL_UNIX_PASSWORD,                    "password",          "probe_password"},
	/* 13005 */ {OVAL_UNIX_PROCESS,                     "process",           "probe_process"},
	/* 13006 */ {OVAL_UNIX_RUNLEVEL,                    "runlevel",          "probe_runlevel"},
	/* 13008 */ {OVAL_UNIX_SHADOW,                      "shadow",            "probe_shadow"},
	/* 13009 */ {OVAL_UNIX_UNAME,                       "uname",             "probe_uname"}
};

#define OVALP_LTBL_SIZE (sizeof __ovalp_ltable / sizeof (oval_pdsc_t))

static SEXP_t *oval_probe_cmd_obj_eval(SEXP_t * sexp, void *arg);
static SEXP_t *oval_probe_cmd_ste_fetch(SEXP_t * sexp, void *arg);
static int oval_probe_cmd_init(SEAP_CTX_t * ctx, oval_pctx_t *);

static oval_pdtbl_t *oval_pdtbl_new(void);
static void oval_pdtbl_free(oval_pdtbl_t *);
static int oval_pdtbl_add(oval_pdtbl_t * table, oval_subtype_t type, int sd, const char *uri);
static oval_pd_t *oval_pdtbl_get(oval_pdtbl_t * table, oval_subtype_t type);
static int oval_pd_cmp(const oval_pd_t * a, const oval_pd_t * b);
static int oval_pdsc_subtype_cmp(oval_subtype_t * a, oval_pdsc_t * b);

static oval_pdtbl_t *oval_pdtbl_new(void)
{
	oval_pdtbl_t *p_tbl;

	p_tbl = oscap_talloc(oval_pdtbl_t);
        
	p_tbl->memb = NULL;
	p_tbl->count = 0;
	p_tbl->ctx = SEAP_CTX_new();

	return (p_tbl);
}

static void oval_pdtbl_free(oval_pdtbl_t * tbl)
{
	return;
}

static int oval_probe_cmd_init(SEAP_CTX_t * ctx, oval_pctx_t * pctx)
{
	_A(ctx != NULL);

	if (SEAP_cmd_register(ctx, PROBECMD_OBJ_EVAL, SEAP_CMDREG_USEARG, &oval_probe_cmd_obj_eval, (void *)pctx) != 0) {
		_D("FAIL: can't register command: %s: errno=%u, %s.\n", "obj_eval", errno, strerror(errno));
		return (-1);
	}

	if (SEAP_cmd_register(ctx, PROBECMD_STE_FETCH, SEAP_CMDREG_USEARG,
			      &oval_probe_cmd_ste_fetch, (void *)pctx) != 0) {
		_D("FAIL: can't register command: %s: errno=%u, %s.\n", "ste_fetch", errno, strerror(errno));

		/* FIXME: unregister the first command */

		return (-1);
	}

	return (0);
}

static int oval_pdsc_subtype_cmp(oval_subtype_t * a, oval_pdsc_t * b)
{
        _A(a != NULL);
        _A(b != NULL);
        return (*a - b->subtype);
}

const oval_pdsc_t *oval_pdsc_lookup(oval_subtype_t typenum)
{
	return (oscap_bfind((void *)__ovalp_ltable, OVALP_LTBL_SIZE, sizeof __ovalp_ltable[0],
			    &typenum, (int (*)(void *, void *))oval_pdsc_subtype_cmp));
}

oval_subtype_t oval_pdsc_lookup_type(const char *name)
{
	unsigned int i;

	_A(name != NULL);

	for (i = 0; i < OVALP_LTBL_SIZE; ++i)
		if (strcmp(__ovalp_ltable[i].subtype_name, name) == 0)
			return __ovalp_ltable[i].subtype;

	return (0);
}

static oval_pd_t *oval_pdtbl_get(oval_pdtbl_t * tbl, oval_subtype_t subtype)
{
	_A(tbl != NULL);
	return oscap_bfind((void *)(tbl->memb), tbl->count, sizeof(oval_pd_t),
			   &subtype, (int (*)(void *, void *))oval_pdsc_subtype_cmp);
}

static int oval_pd_cmp(const oval_pd_t * a, const oval_pd_t * b)
{
	return (a->subtype - b->subtype);
}

static int oval_pdtbl_add(oval_pdtbl_t * tbl, oval_subtype_t type, int sd, const char *uri)
{
	_A(tbl != NULL);
	_A(uri != NULL);

	tbl->memb = oscap_realloc(tbl->memb, sizeof(oval_pd_t) * (++tbl->count));
        
	tbl->memb[tbl->count - 1].subtype = type;
	tbl->memb[tbl->count - 1].sd      = sd;
	tbl->memb[tbl->count - 1].uri     = strdup(uri);

	qsort(tbl->memb, tbl->count, sizeof(oval_pd_t), (int (*)(const void *, const void *))oval_pd_cmp);

	return (0);
}

static int oval_pdtbl_del(oval_pdtbl_t * tbl, oval_subtype_t type)
{
	_A(tbl != NULL);
	/* TODO */
	return (0);
}

oval_pctx_t *oval_pctx_new(struct oval_syschar_model * model)
{
	oval_pctx_t *ctx;

	ctx = oscap_talloc(oval_pctx_t);

	ctx->pdsc_table = (oval_pdsc_t *) __ovalp_ltable;

#if defined(OVAL_PROBEDIR_ENV)
	ctx->p_dir = getenv("OVAL_PROBE_DIR");
#else
	ctx->p_dir = NULL;
#endif

	ctx->p_dir = ctx->p_dir == NULL ? oscap_strdup(OVAL_PROBE_DIR) : oscap_strdup(ctx->p_dir);
	ctx->pd_table = oval_pdtbl_new();
	ctx->model = model;

	if (model != NULL) {
		if (oval_probe_cmd_init(ctx->pd_table->ctx, ctx) != 0) {
			_D("FAIL: Can't initialize SEAP commands\n");
                        oval_pctx_free(ctx);
                        oscap_seterr(OSCAP_EFAMILY_OVAL, OVAL_EPROBEINIT, "Initialization of SEAP commands failed");
			
			return (NULL);
		}
	}

	ctx->p_flags = 0;

	return (ctx);
}

int oval_pctx_setflag(oval_pctx_t * ctx, uint32_t n_flags)
{
	uint32_t o_flags;

	_A(ctx != NULL);

	o_flags = ctx->p_flags;
	n_flags &= OVAL_PCTX_FLAG_MASK;

	if (n_flags)
		OVAL_PCTX_SETFLAG(ctx, n_flags);
	else
		return (-1);
	/*
	 * per-flag special actions
	 */

	if ((n_flags & OVAL_PCTX_FLAG_RUNNOW) && !(o_flags & OVAL_PCTX_FLAG_RUNNOW)) {
		/*
		 * Start all probes
		 */
	}

	return (0);
}

int oval_pctx_unsetflag(oval_pctx_t * ctx, uint32_t u_flags)
{
	uint32_t o_flags;

	_A(ctx != NULL);

	o_flags = ctx->p_flags;
	u_flags &= OVAL_PCTX_FLAG_MASK;

	if (u_flags)
		OVAL_PCTX_UNSETFLAG(ctx, u_flags);
	else
		return (-1);

	return (0);
}

int oval_pctx_setattr(oval_pctx_t * ctx, uint32_t attr, ...)
{
	va_list ap;

	_A(ctx != NULL);

	va_start(ap, attr);

	switch (attr) {
	case OVAL_PCTX_ATTR_DIR:
		if (ctx->p_dir != NULL)
			oscap_free(ctx->p_dir);

		ctx->p_dir = strdup(va_arg(ap, const char *));
		break;
	case OVAL_PCTX_ATTR_MODEL:
		ctx->model = va_arg(ap, struct oval_syschar_model *);
		break;
	default:
		return (-1);
	}

	return (0);
}

void oval_pctx_free(oval_pctx_t * ctx)
{
	oval_pdtbl_free(ctx->pd_table);
	oscap_free(ctx->p_dir);
	oscap_free(ctx);

	return;
}

static SEXP_t *oval_probe_comm(SEAP_CTX_t * ctx, oval_pd_t * pd, const SEXP_t * s_iobj, int noreply)
{
	int retry;

	SEAP_msg_t *s_imsg, *s_omsg;
	SEXP_t *s_oobj;

	_A(pd != NULL);
	_A(s_iobj != NULL);
        
        debug(3) {
                fprintf(stderr, "--- msg out ---\n");
                SEXP_fprintfa(stderr, s_iobj);
                fprintf(stderr, "\n---------------\n");
        }
        
	oscap_dprintf("ctx=%p, pd=%p, s_iobj=%p\n", ctx, pd, s_iobj);
        
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
                                        _D("Can't connect: %u, %s.\n", errno, strerror(errno));
                                }
                                
				if (++retry <= OVAL_PROBE_MAXRETRY) {
					_D("connect: retry %u/%u\n", retry, OVAL_PROBE_MAXRETRY);
					continue;
				} else {
                                        char errbuf[__ERRBUF_SIZE];
                                        
                                        protect_errno {
                                                _D("connect: retry limit (%u) reached.\n", OVAL_PROBE_MAXRETRY);
                                        }
                                        
                                        if (strerror_r (errno, errbuf, sizeof errbuf - 1) != 0)
                                                oscap_seterr (OSCAP_EFAMILY_OVAL, OVAL_EPROBECONN, "Can't connect to the probe");
                                        else
                                                oscap_seterr (OSCAP_EFAMILY_OVAL, OVAL_EPROBECONN, errbuf);
                                        
					return (NULL);
				}
			}
		}

		s_omsg = SEAP_msg_new();
		SEAP_msg_set(s_omsg, (SEXP_t *) s_iobj);

		if (noreply) {
			if (SEAP_msgattr_set(s_omsg, "no-reply", NULL) != 0) {
                                
                                protect_errno {
                                        _D("Can't set no-reply attribute\n");
                                }
                                
                                SEAP_msg_free(s_omsg);
                                oscap_seterr (OSCAP_EFAMILY_OVAL, OVAL_EPROBEUNKNOWN, NULL);
                                
				return (NULL);
			}
		}

		_D("Sending message...\n");

		if (SEAP_sendmsg(ctx, pd->sd, s_omsg) != 0) {
                        
                        protect_errno {
                                _D("Can't send message: %u, %s\n", errno, strerror(errno));
                        }
                        
			if (SEAP_close(ctx, pd->sd) != 0) {
                                char errbuf[__ERRBUF_SIZE];
                                
                                protect_errno {
                                        _D("Can't close sd: %u, %s\n", errno, strerror(errno));
                                        SEAP_msg_free(s_omsg);
                                }
                                
                                if (strerror_r (errno, errbuf, sizeof errbuf - 1) != 0)
                                        oscap_seterr (OSCAP_EFAMILY_OVAL, OVAL_EPROBECLOSE, "Can't close sd");
                                else
                                        oscap_seterr (OSCAP_EFAMILY_OVAL, OVAL_EPROBECLOSE, errbuf);
                                
				return (NULL);
			}

			pd->sd = -1;

			if (++retry <= OVAL_PROBE_MAXRETRY) {
				_D("send: retry %u/%u\n", retry, OVAL_PROBE_MAXRETRY);
				continue;
			} else {
                                char errbuf[__ERRBUF_SIZE];
                                
                                protect_errno {
                                        _D("send: retry limit (%u) reached.\n", OVAL_PROBE_MAXRETRY);
                                        SEAP_msg_free(s_omsg);
                                }
                                
                                if (strerror_r (errno, errbuf, sizeof errbuf - 1) != 0)
                                        oscap_seterr (OSCAP_EFAMILY_OVAL, OVAL_EPROBESEND, "Unable to send a message to probe");
                                else
                                        oscap_seterr (OSCAP_EFAMILY_OVAL, OVAL_EPROBESEND, errbuf);
                                
				return (NULL);
			}
		}

		_D("Waiting for reply...\n");

 recv_retry:
		s_imsg = NULL;

		if (SEAP_recvmsg(ctx, pd->sd, &s_imsg) != 0) {
                        
                        protect_errno {
                                _D("Can't receive message: %u, %s\n", errno, strerror(errno));
                        }
                        
			switch (errno) {
			case ECANCELED:
				{
					SEAP_err_t *err = NULL;

					if (SEAP_recverr_byid(ctx, pd->sd, &err, SEAP_msg_id(s_omsg)) != 0)
						goto recv_retry;

					/* 
					 * decide what to do based on the error code/type
					 */
				}
				break;
			}

			if (SEAP_close(ctx, pd->sd) != 0) {
                                char errbuf[__ERRBUF_SIZE];
                                
                                protect_errno {
                                        _D("Can't close sd: %u, %s\n", errno, strerror(errno));
                                        SEAP_msg_free(s_imsg);
                                        SEAP_msg_free(s_omsg);
                                }
                                
                                if (strerror_r (errno, errbuf, sizeof errbuf - 1) != 0)
                                        oscap_seterr (OSCAP_EFAMILY_OVAL, OVAL_EPROBECLOSE, "Unable to close probe sd");
                                else
                                        oscap_seterr (OSCAP_EFAMILY_OVAL, OVAL_EPROBECLOSE, errbuf);
                                                                
				return (NULL);
			}

			pd->sd = -1;

			if (++retry <= OVAL_PROBE_MAXRETRY) {
				_D("recv: retry %u/%u\n", retry, OVAL_PROBE_MAXRETRY);
				continue;
			} else {
                                char errbuf[__ERRBUF_SIZE];

                                protect_errno {
                                        _D("recv: retry limit (%u) reached.\n", OVAL_PROBE_MAXRETRY);
                                        SEAP_msg_free(s_imsg);
                                        SEAP_msg_free(s_omsg);
                                }
                                
                                if (strerror_r (errno, errbuf, sizeof errbuf - 1) != 0)
                                        oscap_seterr (OSCAP_EFAMILY_OVAL, OVAL_EPROBERECV, "Unable to receive a message from probe");
                                else
                                        oscap_seterr (OSCAP_EFAMILY_OVAL, OVAL_EPROBERECV, errbuf);
                                                                
				return (NULL);
			}
		}

		_D("Message received.\n");
		break;
	}

	s_oobj = SEAP_msg_get(s_imsg);
        
	SEAP_msg_free(s_imsg);
	SEAP_msg_free(s_omsg);

        debug(3) {
                fprintf(stderr, "--- msg in ---\n");
                
                if (noreply)
                        fprintf(stderr, "+ no-reply\n");
                
                SEXP_fprintfa(stderr, s_oobj);
                fprintf(stderr, "\n--------------\n");
        }
        
	return (s_oobj);
}

struct oval_syschar *oval_probe_object_eval(oval_pctx_t * ctx, struct oval_object *object)
{
	const oval_pdsc_t *pdsc;
	oval_pd_t *pd;

	SEXP_t *s_obj, *s_sysc;
	struct oval_syschar *o_sysc;

	_A(ctx != NULL);
	_A(object != NULL);
	_A(ctx->pdsc_table != NULL);

	/* XXX: lock ctx? */

	oscap_clearerr();

	if (ctx->model == NULL) {
                oscap_seterr (OSCAP_EFAMILY_OVAL, OVAL_EPROBECONTEXT, "Invalid probe context; model not set");
                return (NULL);
	}
        
	pdsc = oval_pdsc_lookup(oval_object_get_subtype(object));

	if (pdsc == NULL) {
                oscap_seterr (OSCAP_EFAMILY_OVAL, OVAL_EPROBENOTSUPP, "OVAL object not supported");
		return (NULL);
	}

	pd = oval_pdtbl_get(ctx->pd_table, oval_object_get_subtype(object));

	if (pd == NULL) {
		char   probe_uri[PATH_MAX + 1];
		size_t probe_urilen;
		char  *probe_dir;

		probe_dir    = ctx->p_dir;
		probe_urilen = snprintf(probe_uri, sizeof probe_uri,
					"%s://%s/%s", OVAL_PROBE_SCHEME, probe_dir, pdsc->filename);
                
		_D("URI: %s\n", probe_uri);

		if (oval_pdtbl_add(ctx->pd_table, oval_object_get_subtype(object), -1, probe_uri) != 0) {
                        char errmsg[__ERRBUF_SIZE];

                        snprintf (errmsg, sizeof errmsg, "%s probe not supported", pdsc->subtype_name);
                        oscap_seterr (OSCAP_EFAMILY_OVAL, OVAL_EPROBENOTSUPP, errmsg);
                        
			return (NULL);
		} else
			pd = oval_pdtbl_get(ctx->pd_table, oval_object_get_subtype(object));
	}

	s_obj = oval_object2sexp(pdsc->subtype_name, object, ctx->model);

	if (s_obj == NULL) {
		_D("Can't translate OVAL object to S-exp\n");
                oscap_seterr (OSCAP_EFAMILY_OVAL, OVAL_EPROBEOBJINVAL, "Can't translate OVAL object to S-exp");
		return (NULL);
	}

	s_sysc = oval_probe_comm(ctx->pd_table->ctx, pd, s_obj, ctx->p_flags & OVAL_PCTX_FLAG_NOREPLY);
        
	if (s_sysc == NULL) {
                SEXP_free(s_obj);
                
                if (ctx->p_flags & OVAL_PCTX_FLAG_NOREPLY) {
                        /*
                         * NULL is ok here because the no-reply flag is set and we don't expect
                         * any data to be returned.
                         */
                        return (NULL);
                } else {
                        if (!oscap_err ()) {
                                /*
                                 * oval_probe_comm didn't set an error so we have to do it here.
                                 */
                                oscap_dprintf ("oval_probe_comm failed but didn't set an error!\n");
                                oscap_seterr (OSCAP_EFAMILY_OVAL, OVAL_EPROBENODATA, "No data received");
                        }
                        
                        return (NULL);
                }
	} else {
		if (ctx->p_flags & OVAL_PCTX_FLAG_NOREPLY) {
                        /*
                         * The no-reply flag is set and oval_probe_comm returned some
                         * data. This considered as a non-fatal error.
                         */
                        oscap_dprintf ("obtrusive data from probe!\n");
			
                        SEXP_free(s_sysc);
			SEXP_free(s_obj);
                        
			return (NULL);
		}
	}

	/*
	 * Convert the received S-exp to OVAL system characteristic.
	 */

	struct oval_syschar_model *model = ctx->model;
	o_sysc = oval_sexp2sysch(s_sysc, model, object);

	SEXP_free(s_sysc);
	SEXP_free(s_obj);

	return (o_sysc);
}

static SEXP_t *oval_probe_cmd_obj_eval(SEXP_t * sexp, void *arg)
{
	char *id_str;
	struct oval_definition_model *defs;
	struct oval_object *obj;
	oval_pctx_t *ctx = (oval_pctx_t *) arg;

	if (!SEXP_stringp(sexp)) {
		_D("FAIL: invalid argument: type=%s\n", SEXP_strtype(sexp));
		return (NULL);
	}

	id_str = SEXP_string_cstr(sexp);
	defs   = oval_syschar_model_get_definition_model(ctx->model);
	obj    = oval_definition_model_get_object(defs, id_str);

	_D("get_object: %s\n", id_str);

	if (obj == NULL) {
		_D("FAIL: can't find obj: id=%s\n", id_str);
		oscap_free(id_str);

		return (NULL);
	}

	if (oval_pctx_setflag(ctx, OVAL_PCTX_FLAG_NOREPLY) != 0) {
		_D("FAIL: Can't set NOREPLY flag\n");
		oscap_free(id_str);

		return (NULL);
	}

	oscap_clearerr();
	oval_probe_object_eval(ctx, obj);	/* DO NOT PUT THIS INSIDE ASSERT! */

	if (oval_pctx_unsetflag(ctx, OVAL_PCTX_FLAG_NOREPLY) != 0) {
		_D("FAIL: Can't unset NOREPLY flag\n");
		oscap_free(id_str);

		return (NULL);
	}

	if (oscap_err()) {
		_D("FAIL: obj eval failed: id=%s, err: %d, %d, %s\n",
		   id_str, oscap_err_family(), oscap_err_code(), oscap_err_desc());
		oscap_clearerr();
		oscap_free(id_str);

		return (NULL);
	}

	oscap_free(id_str);

	return (sexp);
}

static SEXP_t *oval_probe_cmd_ste_fetch(SEXP_t * sexp, void *arg)
{
	SEXP_t *id, *ste_list, *ste_sexp;
	char *id_str;
	struct oval_state *ste;
	struct oval_definition_model *definition_model;
	oval_pctx_t *ctx = (oval_pctx_t *) arg;

	ste_list = SEXP_list_new(NULL);

	SEXP_list_foreach(id, sexp) {
		if (SEXP_stringp(id)) {
			id_str = SEXP_string_cstr(id);
			definition_model = oval_syschar_model_get_definition_model(ctx->model);
			ste = oval_definition_model_get_state(definition_model, id_str);

			if (ste == NULL) {
				_D("FAIL: can't find ste: id=%s\n", id_str);
				SEXP_list_free(ste_list);
				oscap_free(id_str);
			}

			ste_sexp = oval_state2sexp(ste);
			SEXP_list_add(ste_list, ste_sexp);

			oscap_free(id_str);
		}
	}

	return (ste_list);
}

struct oval_sysinfo *oval_probe_sysinf_eval(oval_pctx_t * ctx)
{
	struct oval_sysinfo *sysinf;
	struct oval_sysint *ife;
	oval_pd_t *pd;
	SEXP_t *s_obj, *s_sinf, *ent, *r0, *r1, *attrs;
	
	_A(ctx != NULL);
	_A(ctx->model != NULL);

	pd = oval_pdtbl_get(ctx->pd_table, OVAL_INDEPENDENT_SYSCHAR_SUBTYPE);

	if (pd == NULL) {
		char probe_uri[PATH_MAX + 1];
		size_t probe_urilen;
		char *probe_dir;

		probe_dir = ctx->p_dir;
		probe_urilen = snprintf(probe_uri, sizeof probe_uri,
					"%s://%s/probe_system_info", OVAL_PROBE_SCHEME, probe_dir);

		_D("URI: %s\n", probe_uri);

		if (oval_pdtbl_add(ctx->pd_table, OVAL_INDEPENDENT_SYSCHAR_SUBTYPE, -1, probe_uri) != 0) {
                        oscap_seterr (OSCAP_EFAMILY_OVAL, OVAL_EPROBENOTSUPP, "sysinfo probe not supported");
			return (NULL);
		} else
			pd = oval_pdtbl_get(ctx->pd_table, OVAL_INDEPENDENT_SYSCHAR_SUBTYPE);
	}

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
                
                SEXP_vfree (r1, r2, r3, NULL);
                s_obj = SEXP_list_new (r0, NULL);
                SEXP_free (r0);
        }
        
        debug(3) {
                if (ctx->p_flags & OVAL_PCTX_FLAG_NOREPLY)
                        _D("WARN: NOREPLY flag set\n");
        }
        
        r0 = oval_probe_comm(ctx->pd_table->ctx, pd, s_obj, ctx->p_flags & OVAL_PCTX_FLAG_NOREPLY);
        SEXP_free(s_obj);

	if (r0 == NULL)
		return (NULL);

	s_sinf = _probe_cobj_get_items(r0);
	SEXP_free(r0);

	sysinf = oval_sysinfo_new(ctx->model);

	/*
	 * Translate S-exp to sysinfo structure
	 */
#define SYSINF_EXT(obj, name, sysinf, fail)                             \
        do {                                                            \
                SEXP_t *val;                                            \
                char    buf[128+1];                                     \
                                                                        \
                val = probe_obj_getentval (obj, __STRING(name), 1);     \
                                                                        \
                if (val == NULL) {                                      \
                        _D("No entity or value: %s\n", __STRING(name)); \
                        goto fail;                                      \
                }                                                       \
                                                                        \
                if (SEXP_string_cstr_r (val, buf, sizeof buf) >= sizeof buf) { \
                        _D("Value too large: %s\n", __STRING(name));    \
                        SEXP_free (val);                                \
                        goto fail;                                      \
                }                                                       \
                                                                        \
                oval_sysinfo_set_##name (sysinf, buf);                  \
                SEXP_free (val);                                        \
        } while (0)

	if (s_sinf == NULL) {
		oval_sysinfo_free(sysinf);
		return (NULL);
	}

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
			ife = oval_sysint_new(ctx->model);

#define SYSINF_IEXT(ent, name, sysint, fail)                            \
                        do {                                            \
                                SEXP_t *val;                            \
                                char    buf[128+1];                     \
                                                                        \
                                val = probe_ent_getattrval (ent, __STRING(name)); \
                                                                        \
                                if (val == NULL) {                      \
                                        _D("No value: %s\n", __STRING(name)); \
                                        goto fail;                      \
                                }                                       \
                                                                        \
                                if (SEXP_string_cstr_r (val, buf, sizeof buf) >= sizeof buf) { \
                                        _D("Value too large: %s\n", __STRING(name)); \
                                        SEXP_free (val);                \
                                        goto fail;                      \
                                }                                       \
                                                                        \
                                oval_sysint_set_##name (sysint, buf);   \
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

	return (sysinf);
 fail_int:
	SEXP_free(ent);
	oval_sysint_free(ife);
 fail_gen:
	SEXP_free(s_sinf);
	oval_sysinfo_free(sysinf);

	return (NULL);
}

int oval_probe_reset(oval_pctx_t * ctx, oval_subtype_t type)
{
	return 0;		/* TODO */
}

int oval_probe_close(oval_pctx_t * ctx, oval_subtype_t type)
{
	return 0;		/* TODO */
}
