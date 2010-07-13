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
#include <config.h>
#include <seap.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include "common/_error.h"
#include "common/public/alloc.h"
#include "common/assume.h"
#include "common/util.h"
#include "common/bfind.h"
#include "common/debug_priv.h"
#include "probes/public/probe-api.h"
#include "oval_probe_ext.h"
#include "oval_sexp.h"

#define __ERRBUF_SIZE 128

/* KEEP THIS LIST SORTED! (by subtype) */
const oval_pdsc_t OSCAP_GSYM(default_pdsc)[] = {
        /*     2 */ {OVAL_SUBTYPE_SYSINFO,                  "system_info",       "probe_system_info"},
	/*  7001 */ {OVAL_INDEPENDENT_FAMILY,               "family",            "probe_family"},
        /*  7002 */ {OVAL_INDEPENDENT_FILE_MD5,             "filemd5",           "probe_filemd5"},
        /*  7003 */ {OVAL_INDEPENDENT_FILE_HASH,            "filehash",          "probe_filehash"},
	/*  7006 */ {OVAL_INDEPENDENT_TEXT_FILE_CONTENT_54, "textfilecontent54", "probe_textfilecontent54"},
	/*  7007 */ {OVAL_INDEPENDENT_TEXT_FILE_CONTENT,    "textfilecontent",   "probe_textfilecontent"},
	/*  7010 */ {OVAL_INDEPENDENT_XML_FILE_CONTENT,     "xmlfilecontent",    "probe_xmlfilecontent"},
	/*  9001 */ {OVAL_LINUX_DPKG_INFO,                  "dpkginfo",          "probe_dpkginfo"},
	/*  9002 */ {OVAL_LINUX_INET_LISTENING_SERVERS,     "inetlisteningservers", "probe_inetlisteningservers"},
	/*  9003 */ {OVAL_LINUX_RPM_INFO,                   "rpminfo",           "probe_rpminfo"},
	/* 13001 */ {OVAL_UNIX_FILE,                        "file",              "probe_file"},
	/* 13003 */ {OVAL_UNIX_INTERFACE,                   "interface",         "probe_interface"},
	/* 13004 */ {OVAL_UNIX_PASSWORD,                    "password",          "probe_password"},
	/* 13005 */ {OVAL_UNIX_PROCESS,                     "process",           "probe_process"},
	/* 13006 */ {OVAL_UNIX_RUNLEVEL,                    "runlevel",          "probe_runlevel"},
	/* 13008 */ {OVAL_UNIX_SHADOW,                      "shadow",            "probe_shadow"},
	/* 13009 */ {OVAL_UNIX_UNAME,                       "uname",             "probe_uname"}
};

#define DEFAULT_PDSC_COUNT (sizeof OSCAP_GSYM(default_pdsc) / sizeof (oval_pdsc_t))

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
                oval_pdtbl_free(pext->pdtbl);
        }

        pthread_mutex_destroy(&pext->lock);
        oscap_free(pext);
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
                SEAP_close(tbl->ctx, tbl->memb[i].sd);
                oscap_free(tbl->memb[i].uri);
        }

        oscap_free(tbl->memb);
        SEAP_CTX_free(tbl->ctx);
        oscap_free(tbl);

	return;
}

static int oval_pdtbl_pdcmp(const oval_pd_t *a, const oval_pd_t *b)
{
	return (a->subtype - b->subtype);
}

static int oval_pdtbl_typecmp(oval_subtype_t *a, oval_pd_t *b)
{
        return (*a - b->subtype);
}

static int oval_pdtbl_add(oval_pdtbl_t * tbl, oval_subtype_t type, int sd, const char *uri)
{
	assume_d (tbl != NULL, -1);
	assume_d (uri != NULL, -1);

	tbl->memb = oscap_realloc(tbl->memb, sizeof(oval_pd_t) * (++tbl->count));

	tbl->memb[tbl->count - 1].subtype = type;
	tbl->memb[tbl->count - 1].sd      = sd;
	tbl->memb[tbl->count - 1].uri     = strdup(uri);

	qsort(tbl->memb, tbl->count, sizeof(oval_pd_t),
              (int (*)(const void *, const void *))oval_pdtbl_pdcmp);

	return (0);
}

static oval_pd_t *oval_pdtbl_get(oval_pdtbl_t * tbl, oval_subtype_t type)
{
	return oscap_bfind(tbl->memb, tbl->count, sizeof(oval_pd_t),
			   &type, (int (*)(void *, void *))oval_pdtbl_typecmp);
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
		oscap_dprintf("FAIL: can't register command: %s: errno=%u, %s.\n", "obj_eval", errno, strerror(errno));
		return (-1);
	}

	if (SEAP_cmd_register(pext->pdtbl->ctx, PROBECMD_STE_FETCH, SEAP_CMDREG_USEARG,
			      &oval_probe_cmd_ste_fetch, (void *)pext) != 0) {
		oscap_dprintf("FAIL: can't register command: %s: errno=%u, %s.\n", "ste_fetch", errno, strerror(errno));

		/* FIXME: unregister the first command */

		return (-1);
	}

	return (0);
}

static SEXP_t *oval_probe_cmd_obj_eval(SEXP_t *sexp, void *arg)
{
	char *id_str;
	struct oval_definition_model *defs;
	struct oval_object *obj;
	oval_pext_t *pext = (oval_pext_t *) arg;

        assume_d (sexp != NULL, NULL);
        assume_d (arg  != NULL, NULL);

	if (!SEXP_stringp(sexp)) {
		oscap_dprintf("FAIL: invalid argument: type=%s\n", SEXP_strtype(sexp));
		return (NULL);
	}

	id_str = SEXP_string_cstr(sexp);
	defs   = oval_syschar_model_get_definition_model(*(pext->model));
	obj    = oval_definition_model_get_object(defs, id_str);

	oscap_dprintf("get_object: %s\n", id_str);

	if (obj == NULL) {
		oscap_dprintf("FAIL: can't find obj: id=%s\n", id_str);
		oscap_free(id_str);

		return (NULL);
	}

	oscap_clearerr();
	oval_probe_object_query(pext->sess_ptr, obj, OVAL_PDFLAG_NOREPLY); /* DO NOT PUT THIS INSIDE ASSERT! */

	if (oscap_err()) {
		oscap_dprintf("FAIL: obj eval failed: id=%s, err: %d, %d, %s\n",
		   id_str, oscap_err_family(), oscap_err_code(), oscap_err_desc());
		oscap_clearerr();
		oscap_free(id_str);

		return (NULL);
	}

	oscap_free(id_str);

	return SEXP_ref(sexp);
}

static SEXP_t *oval_probe_cmd_ste_fetch(SEXP_t *sexp, void *arg)
{
	SEXP_t *id, *ste_list, *ste_sexp;
	char *id_str;
	struct oval_state *ste;
	struct oval_definition_model *definition_model;
	oval_pext_t *pext = (oval_pext_t *)arg;

        assume_d (sexp != NULL, NULL);
        assume_d (arg  != NULL, NULL);

	ste_list = SEXP_list_new(NULL);

	SEXP_list_foreach(id, sexp) {
		if (SEXP_stringp(id)) {
			id_str = SEXP_string_cstr(id);
			definition_model = oval_syschar_model_get_definition_model(*(pext->model));
			ste = oval_definition_model_get_state(definition_model, id_str);

			if (ste == NULL) {
				oscap_dprintf("FAIL: can't find ste: id=%s\n", id_str);
				SEXP_list_free(ste_list);
				oscap_free(id_str);
			}

			ste_sexp = oval_state2sexp(ste, pext->sess_ptr);
			SEXP_list_add(ste_list, ste_sexp);
                        SEXP_free(ste_sexp);

			oscap_free(id_str);
		}
	}

	return (ste_list);
}

static SEXP_t *oval_probe_comm(SEAP_CTX_t *ctx, oval_pd_t *pd, const SEXP_t *s_iobj, int noreply)
{
	int retry;

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
                                        oscap_dprintf("Can't connect: %u, %s.\n", errno, strerror(errno));
                                }

				if (++retry <= OVAL_PROBE_MAXRETRY) {
					oscap_dprintf("connect: retry %u/%u\n", retry, OVAL_PROBE_MAXRETRY);
					continue;
				} else {
                                        char errbuf[__ERRBUF_SIZE];

                                        protect_errno {
                                                oscap_dprintf("connect: retry limit (%u) reached.\n", OVAL_PROBE_MAXRETRY);
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
                                        oscap_dprintf("Can't set no-reply attribute\n");
                                }

                                SEAP_msg_free(s_omsg);
                                oscap_seterr (OSCAP_EFAMILY_OVAL, OVAL_EPROBEUNKNOWN, NULL);

				return (NULL);
			}
		}

		oscap_dprintf("Sending message...\n");

		if (SEAP_sendmsg(ctx, pd->sd, s_omsg) != 0) {
                        protect_errno {
                                oscap_dprintf("Can't send message: %u, %s\n", errno, strerror(errno));
                        }

			if (SEAP_close(ctx, pd->sd) != 0) {
                                char errbuf[__ERRBUF_SIZE];

                                protect_errno {
                                        oscap_dprintf("Can't close sd: %u, %s\n", errno, strerror(errno));
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
				oscap_dprintf("send: retry %u/%u\n", retry, OVAL_PROBE_MAXRETRY);
				continue;
			} else {
                                char errbuf[__ERRBUF_SIZE];

                                protect_errno {
                                        oscap_dprintf("send: retry limit (%u) reached.\n", OVAL_PROBE_MAXRETRY);
                                        SEAP_msg_free(s_omsg);
                                }

                                if (strerror_r (errno, errbuf, sizeof errbuf - 1) != 0)
                                        oscap_seterr (OSCAP_EFAMILY_OVAL, OVAL_EPROBESEND, "Unable to send a message to probe");
                                else
                                        oscap_seterr (OSCAP_EFAMILY_OVAL, OVAL_EPROBESEND, errbuf);

				return (NULL);
			}
		}

		oscap_dprintf("Waiting for reply...\n");

 recv_retry:
		s_imsg = NULL;

		if (SEAP_recvmsg(ctx, pd->sd, &s_imsg) != 0) {
                        protect_errno {
                                oscap_dprintf("Can't receive message: %u, %s\n", errno, strerror(errno));
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
                                        oscap_dprintf("Can't close sd: %u, %s\n", errno, strerror(errno));
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
				oscap_dprintf("recv: retry %u/%u\n", retry, OVAL_PROBE_MAXRETRY);
				continue;
			} else {
                                char errbuf[__ERRBUF_SIZE];

                                protect_errno {
                                        oscap_dprintf("recv: retry limit (%u) reached.\n", OVAL_PROBE_MAXRETRY);
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

		oscap_dprintf("Message received.\n");
		break;
	}

	s_oobj = SEAP_msg_get(s_imsg);

	SEAP_msg_free(s_imsg);
	SEAP_msg_free(s_omsg);

	return (s_oobj);
}

static int oval_pdsc_typecmp(oval_subtype_t *a, oval_pdsc_t *b)
{
        return (*a - b->type);
}

static oval_pdsc_t *oval_pdsc_lookup(oval_pdsc_t pdsc[], int count, oval_subtype_t type)
{
	return oscap_bfind(pdsc, count, sizeof(oval_pdsc_t), &type,
                           (int (*)(void *, void *))oval_pdsc_typecmp);
}

static struct oval_sysinfo *oval_probe_sys_eval(SEAP_CTX_t *ctx, oval_pd_t *pd, struct oval_syschar_model *model)
{
	struct oval_sysinfo *sysinf;
	struct oval_sysint *ife;
	SEXP_t *s_obj, *s_sinf, *ent, *r0, *r1;

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

        r0 = oval_probe_comm(ctx, pd, s_obj, 0);
        SEXP_free(s_obj);

	if (r0 == NULL)
		return (NULL);

	s_sinf = _probe_cobj_get_items(r0);
	SEXP_free(r0);
	sysinf = oval_sysinfo_new(model);

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
			ife = oval_sysint_new(model);

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

	return (sysinf);
 fail_int:
	SEXP_free(ent);
	oval_sysint_free(ife);
 fail_gen:
	SEXP_free(s_sinf);
	oval_sysinfo_free(sysinf);

	return (NULL);
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
                struct oval_object   *obj;
                struct oval_sysinfo **inf;
                /* int flags = va_arg(ap, int); */

                obj = va_arg(ap, struct oval_object *);
                inf = va_arg(ap, struct oval_sysinfo **);
                pd  = oval_pdtbl_get(pext->pdtbl, type);

                if (pd == NULL) {
                        if (oval_probe_sys_handler(type, ptr, PROBE_HANDLER_ACT_OPEN) != 0)
                                return(-1);

                        pd = oval_pdtbl_get(pext->pdtbl, type);
                }

                assume_r(pd != NULL, -1);
                *inf = oval_probe_sys_eval(pext->pdtbl->ctx, pd, *(pext->model));
                ret  = (*inf == NULL ? -1 : 0);
                break;
        }
        case PROBE_HANDLER_ACT_OPEN:
        {
                char         probe_uri[PATH_MAX + 1];
                size_t       probe_urilen;
                char        *probe_dir;
                oval_pdsc_t *probe_dsc;

                probe_dir    = pext->probe_dir;
                probe_dsc    = oval_pdsc_lookup(pext->pdsc, pext->pdsc_cnt, type);
                probe_urilen = snprintf(probe_uri, sizeof probe_uri,
                                        "%s://%s/%s", OVAL_PROBE_SCHEME, probe_dir, probe_dsc->file);

                oscap_dprintf("URI: %s\n", probe_uri);

                if (oval_pdtbl_add(pext->pdtbl, type, -1, probe_uri) != 0) {
                        char errmsg[__ERRBUF_SIZE];

                        snprintf (errmsg, sizeof errmsg, "%s probe not supported", probe_dsc->name);
                        oscap_seterr (OSCAP_EFAMILY_OVAL, OVAL_EPROBENOTSUPP, errmsg);

                        ret = -1;
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
                struct oval_object   *obj = va_arg(ap, struct oval_object *);
                struct oval_syschar **sys = va_arg(ap, struct oval_syschar **);
                int flags = va_arg(ap, int);

                pd = oval_pdtbl_get(pext->pdtbl, oval_object_get_subtype(obj));

                if (pd == NULL) {
                        char         probe_uri[PATH_MAX + 1];
                        size_t       probe_urilen;
                        char        *probe_dir;
                        oval_pdsc_t *probe_dsc;

                        probe_dir    = pext->probe_dir;
                        probe_dsc    = oval_pdsc_lookup(pext->pdsc, pext->pdsc_cnt, oval_object_get_subtype(obj));
                        probe_urilen = snprintf(probe_uri, sizeof probe_uri,
                                                "%s://%s/%s", OVAL_PROBE_SCHEME, probe_dir, probe_dsc->file);

                        oscap_dprintf("URI: %s\n", probe_uri);

                        if (oval_pdtbl_add(pext->pdtbl, oval_object_get_subtype(obj), -1, probe_uri) != 0) {
                                char errmsg[__ERRBUF_SIZE];

                                snprintf (errmsg, sizeof errmsg, "%s probe not supported", probe_dsc->name);
                                oscap_seterr (OSCAP_EFAMILY_OVAL, OVAL_EPROBENOTSUPP, errmsg);

                                return (-1);
                        } else
                                pd = oval_pdtbl_get(pext->pdtbl, oval_object_get_subtype(obj));
                }

                *sys = oval_probe_ext_eval(pext->pdtbl->ctx, pd, pext, obj, flags);
                ret  = (*sys == NULL ? -1 : 0);
                break;
        }
        case PROBE_HANDLER_ACT_OPEN:
                break;
        case PROBE_HANDLER_ACT_INIT:
                ret = oval_probe_ext_init(pext);
                break;
        case PROBE_HANDLER_ACT_RESET:
        {
                size_t i;

                if (type == OVAL_SUBTYPE_ALL) {
                        /*
                         * Iterate trhu probe descriptor table and execute the reset operation
                         * for each probe descriptor.
                         */
                        for (i = 0; i < pext->pdtbl->count; ++i) {
                                pd  = pext->pdtbl->memb + i;

                                //fprintf(stderr, "Sending reset to %p(%s)...\n", pd, oval_subtype2str(pd->subtype));

                                ret = oval_probe_ext_reset(pext->pdtbl->ctx, pd, pext);

                                if (ret != 0)
                                        return(ret);
                        }

                        return(0);
                } else {
                        /*
                         * Reset only the probe of specified subtype.
                         */
                        pd = oval_pdtbl_get(pext->pdtbl, type);

                        if (pd == NULL)
                                return(0);

                        return oval_probe_ext_reset(pext->pdtbl->ctx, pd, pext);
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
                pext->pdsc     = (oval_pdsc_t *)OSCAP_GSYM(default_pdsc);
                pext->pdsc_cnt = DEFAULT_PDSC_COUNT;
                pext->pdtbl    = oval_pdtbl_new();

                if (oval_probe_cmd_init(pext) != 0)
                        ret = -1;
                else
                        pext->do_init = false;
        }
        pthread_mutex_unlock(&pext->lock);

        return(ret);
}

struct oval_syschar *oval_probe_ext_eval(SEAP_CTX_t *ctx, oval_pd_t *pd, oval_pext_t *pext, struct oval_object *object, int flags)
{
        SEXP_t *s_obj, *s_sys;
        struct oval_syschar *o_sys;
        struct oval_syschar_model *model = *(pext->model);

        s_obj = oval_object2sexp(oval_subtype2str(oval_object_get_subtype(object)), object, model, pext->sess_ptr);

        if (s_obj == NULL) {
                oscap_dprintf("Can't translate OVAL object to S-exp\n");
                oscap_seterr(OSCAP_EFAMILY_OVAL, OVAL_EPROBEOBJINVAL, "Can't translate OVAL object to S-exp");

                return(NULL);
        }

        s_sys = oval_probe_comm(ctx, pd, s_obj, flags & OVAL_PDFLAG_NOREPLY);

        if (s_sys == NULL) {
                SEXP_free(s_obj);

                if (flags & OVAL_PDFLAG_NOREPLY) {
                        /*
                         * NULL is ok here because the no-reply flag is set and we don't expect
                         * any data to be returned.
                         */
                        return(NULL);
                } else {
                        if (!oscap_err()) {
                                /*
                                 * oval_probe_comm didn't set an error so we have to do it here.
                                 */
                                oscap_dprintf("oval_probe_comm failed but didn't set an error!\n");
                                oscap_seterr(OSCAP_EFAMILY_OVAL, OVAL_EPROBENODATA, "No data received");
                        }

                        return(NULL);
                }
        } else {
                if (flags & OVAL_PDFLAG_NOREPLY) {
                        /*
                         * The no-reply flag is set and oval_probe_comm returned some
                         * data. This considered as a non-fatal error.
                         */
                        oscap_dprintf ("obtrusive data from probe!\n");

                        SEXP_free(s_sys);
			SEXP_free(s_obj);

			return (NULL);
                }
        }

        /*
	 * Convert the received S-exp to OVAL system characteristic.
	 */
	o_sys = oval_sexp2sysch(s_sys, model, object);

	SEXP_free(s_sys);
	SEXP_free(s_obj);

	return (o_sys);
}

int oval_probe_ext_reset(SEAP_CTX_t *ctx, oval_pd_t *pd, oval_pext_t *pext)
{
        void *res;

        res = SEAP_cmd_exec(ctx, pd->sd, SEAP_EXEC_RECV, PROBECMD_RESET, NULL, SEAP_CMDTYPE_SYNC, NULL, NULL);

        return (0);
}
