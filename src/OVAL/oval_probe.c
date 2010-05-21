/*
 * Copyright 2009-2010 Red Hat Inc., Durham, North Carolina.
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
#include <errno.h>
#if defined(OSCAP_THREAD_SAFE)
# include <pthread.h>
#endif

#include "config.h"
#include "oval_probe.h"
#include "common/_error.h"
#include "common/assume.h"

#include "seap.h"
#include "oval_sexp.h"
#include "oval_probe_impl.h"
#include "oval_system_characteristics_impl.h"
#include "probes/public/probe-api.h"
#include "common/util.h"
#include "common/bfind.h"
#include "common/debug_priv.h"

#include "_oval_probe_session.h"
#include "_oval_probe_handler.h"

/* Sorted by subtype (first column) */
oval_subtypedsc_t __s2n_tbl[] = {
        /*     2 */ {OVAL_SUBTYPE_SYSINFO,                  "system_info"         },
	/*  7001 */ {OVAL_INDEPENDENT_FAMILY,               "family"              },
        /*  7002 */ {OVAL_INDEPENDENT_FILE_MD5,             "filemd5"             },
        /*  7003 */ {OVAL_INDEPENDENT_FILE_HASH,            "filehash"            },
        /*  7004 */ {OVAL_INDEPENDENT_ENVIRONMENT_VARIABLE, "environmentvariable" },
	/*  7006 */ {OVAL_INDEPENDENT_TEXT_FILE_CONTENT_54, "textfilecontent54"   },
	/*  7007 */ {OVAL_INDEPENDENT_TEXT_FILE_CONTENT,    "textfilecontent"     },
        /*  7009 */ {OVAL_INDEPENDENT_VARIABLE,             "variable"            },
	/*  7010 */ {OVAL_INDEPENDENT_XML_FILE_CONTENT,     "xmlfilecontent"      },
	/*  9001 */ {OVAL_LINUX_DPKG_INFO,                  "dpkginfo"            },
	/*  9002 */ {OVAL_LINUX_INET_LISTENING_SERVERS,     "inetlisteningservers"},
	/*  9003 */ {OVAL_LINUX_RPM_INFO,                   "rpminfo"             },
	/* 13001 */ {OVAL_UNIX_FILE,                        "file"                },
	/* 13003 */ {OVAL_UNIX_INTERFACE,                   "interface"           },
	/* 13004 */ {OVAL_UNIX_PASSWORD,                    "password"            },
	/* 13005 */ {OVAL_UNIX_PROCESS,                     "process"             },
	/* 13006 */ {OVAL_UNIX_RUNLEVEL,                    "runlevel"            },
	/* 13008 */ {OVAL_UNIX_SHADOW,                      "shadow"              },
	/* 13009 */ {OVAL_UNIX_UNAME,                       "uname"               }
};

#define __s2n_tbl_count (sizeof __s2n_tbl / sizeof(oval_subtypedsc_t))

static int __s2n_tbl_cmp(oval_subtype_t *type, oval_subtypedsc_t *dsc)
{
        return (*type - dsc->type);
}

/* Sorted by name (second column) */
oval_subtypedsc_t __n2s_tbl[] = {
	/*  9001 */ {OVAL_LINUX_DPKG_INFO,                  "dpkginfo"            },
        /*  7004 */ {OVAL_INDEPENDENT_ENVIRONMENT_VARIABLE, "environmentvariable" },
	/*  7001 */ {OVAL_INDEPENDENT_FAMILY,               "family"              },
	/* 13001 */ {OVAL_UNIX_FILE,                        "file"                },
        /*  7003 */ {OVAL_INDEPENDENT_FILE_HASH,            "filehash"            },
        /*  7002 */ {OVAL_INDEPENDENT_FILE_MD5,             "filemd5"             },
	/*  9002 */ {OVAL_LINUX_INET_LISTENING_SERVERS,     "inetlisteningservers"},
	/* 13003 */ {OVAL_UNIX_INTERFACE,                   "interface"           },
	/* 13004 */ {OVAL_UNIX_PASSWORD,                    "password"            },
	/* 13005 */ {OVAL_UNIX_PROCESS,                     "process"             },
	/*  9003 */ {OVAL_LINUX_RPM_INFO,                   "rpminfo"             },
	/* 13006 */ {OVAL_UNIX_RUNLEVEL,                    "runlevel"            },
	/* 13008 */ {OVAL_UNIX_SHADOW,                      "shadow"              },
        /*     2 */ {OVAL_SUBTYPE_SYSINFO,                  "system_info"         },
	/*  7007 */ {OVAL_INDEPENDENT_TEXT_FILE_CONTENT,    "textfilecontent"     },
	/*  7006 */ {OVAL_INDEPENDENT_TEXT_FILE_CONTENT_54, "textfilecontent54"   },
	/* 13009 */ {OVAL_UNIX_UNAME,                       "uname"               },
        /*  7009 */ {OVAL_INDEPENDENT_VARIABLE,             "variable"            },
	/*  7010 */ {OVAL_INDEPENDENT_XML_FILE_CONTENT,     "xmlfilecontent"      }
};

#define __n2s_tbl_count (sizeof __n2s_tbl / sizeof(oval_subtypedsc_t))

static int __n2s_tbl_cmp(const char *name, oval_subtypedsc_t *dsc)
{
        return strcmp(name, dsc->name);
}

encache_t *OSCAP_GSYM(encache) = NULL;
struct id_desc_t OSCAP_GSYM(id_desc);

#define __ERRBUF_SIZE 128

const char *oval_subtype2str(oval_subtype_t subtype)
{
        oval_subtypedsc_t *d;

        d = oscap_bfind(__s2n_tbl, __s2n_tbl_count, sizeof(oval_subtypedsc_t), &subtype,
                        (int(*)(void *, void *))__s2n_tbl_cmp);

        return (d == NULL ? NULL : d->name);
}

oval_subtype_t oval_str2subtype(const char *str)
{
        oval_subtypedsc_t *d;

        d = oscap_bfind(__n2s_tbl, __n2s_tbl_count, sizeof(oval_subtypedsc_t), str,
                        (int(*)(void *, void *))__n2s_tbl_cmp);

        return (d == NULL ? OVAL_SUBTYPE_UNKNOWN : d->type);
}

struct oval_syschar *oval_probe_object_eval(oval_probe_session_t *psess, struct oval_object *object, int flags)
{
        struct oval_syschar *o_sys;
        oval_subtype_t type;
        oval_ph_t *ph;

        type = oval_object_get_subtype(object);
        ph   = oval_probe_handler_get(psess->ph, type);

        if (ph == NULL) {
                oscap_seterr (OSCAP_EFAMILY_OVAL, OVAL_EPROBENOTSUPP, "OVAL object not supported");
                return(NULL);
        }

        o_sys = NULL;

        if (ph->func(type, ph->uptr, PROBE_HANDLER_ACT_EVAL, object, &o_sys, flags) != 0) {
                return(NULL);
        }

        return(o_sys);
#if 0
	const oval_pdsc_t *pdsc;
	oval_pd_t *pd;

	SEXP_t *s_obj, *s_sysc;
	struct oval_syschar *o_sysc;

	assume_d (ctx != NULL, NULL);
	assume_d (object != NULL, NULL);
	assume_d (ctx->pdsc_table != NULL, NULL);

	/* XXX: lock ctx? */

	oscap_clearerr();

	if (ctx->model == NULL) {
                oscap_seterr (OSCAP_EFAMILY_OVAL, OVAL_EPROBECONTEXT, "Invalid probe context; model not set");
                return (NULL);
	}

	pdsc = oval_pdsc_lookup(oval_object_get_subtype(object));

	if (pdsc == NULL) {
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
#endif
}

struct oval_sysinfo *oval_probe_sysinf_eval(oval_probe_session_t *sess)
{
	struct oval_sysinfo *sysinf;
        oval_ph_t *ph;

        ph = oval_probe_handler_get(sess->ph, OVAL_SUBTYPE_SYSINFO);

        if (ph == NULL) {
                oscap_seterr (OSCAP_EFAMILY_OVAL, OVAL_EPROBENOTSUPP, "OVAL object not supported");
                return(NULL);
        }

        sysinf = NULL;

        if (ph->func(OVAL_SUBTYPE_SYSINFO, ph->uptr, PROBE_HANDLER_ACT_EVAL, NULL, &sysinf, 0) != 0) {
                return(NULL);
        }

        return(sysinf);
}
