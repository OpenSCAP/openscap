/**
 * @file   rpminfo.c
 * @brief  rpminfo probe
 * @author "Daniel Kopecek" <dkopecek@redhat.com>
 *
 * 2010/06/13 dkopecek@redhat.com
 *  This probe is able to process a rpminfo_object as defined in OVAL 5.4 and 5.5.
 *
 */

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

/*
 * rpminfo probe:
 *
 *  rpminfo_object(string name)
 *
 *  rpminfo_state(string name,
 *                string arch,
 *                string epoch,
 *                string release,
 *                string version,
 *                string evr,
 *                string signature_keyid)
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* RPM headers */
#include <rpm/rpmdb.h>
#include <rpm/rpmlib.h>
#include <rpm/rpmts.h>
#include <rpm/rpmmacro.h>
#include <rpm/rpmlog.h>

/* SEAP */
#include <seap.h>
#include <probe-api.h>
#include "probe-entcmp.h"
#include <alloc.h>
#include <common/assume.h>
#include <config.h>

#ifdef HAVE_LIBRPM44
#include <rpm/header.h>
#define headerFormat(_h, _fmt, _emsg) headerSprintf((_h),( _fmt), rpmTagTable, rpmHeaderFormats, (_emsg))
#define rpmFreeCrypto() while(0)
#define rpmFreeFilesystems() while(0)
#endif

#ifndef _A
#define _A(x) assert(x)
#endif

struct rpminfo_req {
        char *name;
        oval_operation_t op;
};

struct rpminfo_rep {
        char *name;
        char *arch;
        char *epoch;
        char *release;
        char *version;
        char *evr;
        char *signature_keyid;
};

struct rpminfo_global {
        rpmdb           rpmdb;
        pthread_mutex_t mutex;
};

static struct rpminfo_global g_rpm;

static void __rpminfo_rep_free (struct rpminfo_rep *ptr)
{
        oscap_free (ptr->name);
        oscap_free (ptr->arch);
        oscap_free (ptr->epoch);
        oscap_free (ptr->release);
        oscap_free (ptr->version);
        oscap_free (ptr->evr);
        oscap_free (ptr->signature_keyid);
}

static void pkgh2rep (Header h, struct rpminfo_rep *r)
{
        errmsg_t rpmerr;
        char *str, *sid;
        size_t len;

        assume_d (h != NULL, /* void */);
        assume_d (r != NULL, /* void */);

        r->name = headerFormat (h, "%{NAME}", &rpmerr);
        r->arch = headerFormat (h, "%{ARCH}", &rpmerr);
        str     = headerFormat (h, "%{EPOCH}", &rpmerr);

        if (strcmp (str, "(none)") == 0) {
                str    = oscap_realloc (str, sizeof (char) * 2);
                str[0] = '0';
                str[1] = '\0';
        }

        r->epoch   = str;
        r->release = headerFormat (h, "%{RELEASE}", &rpmerr);
        r->version = headerFormat (h, "%{VERSION}", &rpmerr);

        len = (strlen (r->epoch)   +
               strlen (r->release) +
               strlen (r->version) + 2);

        str = oscap_alloc (sizeof (char) * (len + 1));
        snprintf (str, len + 1, "%s:%s-%s",
                  r->epoch,
                  r->version,
                  r->release);

        r->evr = str;

        str = headerFormat (h, "%{SIGGPG:pgpsig}", &rpmerr);
        sid = strrchr (str, ' ');
        r->signature_keyid = (sid != NULL ? strdup (sid+1) : strdup ("0"));
        oscap_free (str);
}

/*
 * req - Structure containing the name of the package.
 * rep - Pointer to rpminfo_rep structure pointer. An
 *       array of rpminfo_rep structures will be allocated
 *       here.
 *
 * The return value on error is -1. Otherwise the number of
 * rpminfo_rep structures allocated in *rep is returned.
 */
static int get_rpminfo (struct rpminfo_req *req, struct rpminfo_rep **rep)
{
	rpmdbMatchIterator match;
	Header pkgh;
	int ret = 0, i;

        pthread_mutex_lock (&(g_rpm.mutex));

        ret = -1;

        switch (req->op) {
        case OVAL_OPERATION_EQUALS:
                match = rpmdbInitIterator (g_rpm.rpmdb, RPMTAG_NAME, (const void *)req->name, 0);

                if (match == NULL) {
                        ret = 0;
                        goto ret;
                }

                ret = rpmdbGetIteratorCount (match);

                break;
        case OVAL_OPERATION_PATTERN_MATCH:
                match = rpmdbInitIterator (g_rpm.rpmdb, RPMDBI_PACKAGES, NULL, 0);

                if (match == NULL) {
                        ret = 0;
                        goto ret;
                }

                if (rpmdbSetIteratorRE (match, RPMTAG_NAME, RPMMIRE_REGEX,
                                        (const char *)req->name) != 0)
                {
                        ret = -1;
                        goto ret;
                }

                break;
        default:
                /* not supported */
                ret = -1;
                goto ret;
        }

        if (ret != -1) {
                /*
                 * We can allocate all memory needed now because we know the number
                 * of results.
                 */
                (*rep) = oscap_realloc (*rep, sizeof (struct rpminfo_rep) * ret);

                for (i = 0; i < ret; ++i) {
                        pkgh = rpmdbNextIterator (match);

                        if (pkgh != NULL)
                                pkgh2rep (pkgh, (*rep) + i);
                        else {
                                /* XXX: emit warning */
                                break;
                        }
                }
        } else {
                ret = 0;

                while ((pkgh = rpmdbNextIterator (match)) != NULL) {
                        (*rep) = oscap_realloc (*rep, sizeof (struct rpminfo_rep) * ++ret);
                        assume_r (*rep != NULL, -1);
                        pkgh2rep (pkgh, (*rep) + (ret - 1));
                }
        }

	match = rpmdbFreeIterator (match);
ret:
        pthread_mutex_unlock (&(g_rpm.mutex));
        return (ret);
}

void *probe_init (void)
{
        if (rpmReadConfigFiles ((const char *)NULL, (const char *)NULL) != 0) {
                _D("rpmReadConfigFiles failed: %u, %s.\n", errno, strerror (errno));
                return (NULL);
        }

        /* XXX: check retval */
        rpmdbOpen (NULL, &g_rpm.rpmdb, O_RDONLY, 0644);

        pthread_mutex_init (&(g_rpm.mutex), NULL);

        return ((void *)&g_rpm);
}

void probe_fini (void *ptr)
{
        struct rpminfo_global *r = (struct rpminfo_global *)ptr;

        rpmdbClose (r->rpmdb);
	rpmFreeCrypto();
        rpmFreeRpmrc();
        rpmFreeMacros(NULL);
        rpmlogClose();
        rpmFreeFilesystems();
        pthread_mutex_destroy (&(r->mutex));

        return;
}

int probe_main (SEXP_t *object, SEXP_t *probe_out, void *arg)
{
        SEXP_t *val, *item_sexp, *r0, *r1, *ent;
	int rpmret, i;

        struct rpminfo_req request_st;
        struct rpminfo_rep *reply_st;

	if (object == NULL || probe_out == NULL) {
		return (PROBE_EINVAL);
	}

        ent = probe_obj_getent (object, "name", 1);

        if (ent == NULL) {
                return (PROBE_ENOENT);
        }

        val = probe_ent_getval (ent);

        if (val == NULL) {
                _D("%s: no value\n", "name");
                SEXP_free (ent);
                return (PROBE_ENOVAL);
        }

        request_st.name = SEXP_string_cstr (val);
        SEXP_free (val);

        val = probe_ent_getattrval (ent, "operation");

        if (val == NULL) {
                request_st.op = OVAL_OPERATION_EQUALS;
        } else {
                request_st.op = (oval_operation_t) SEXP_number_geti_32 (val);

                switch (request_st.op) {
                case OVAL_OPERATION_EQUALS:
                case OVAL_OPERATION_PATTERN_MATCH:
                        break;
                default:
                        SEXP_free (val);
                        SEXP_free (ent);
                        oscap_free (request_st.name);
                        return (PROBE_EOPNOTSUPP);
                }

                SEXP_free (val);
        }

        if (request_st.name == NULL) {
                switch (errno) {
                case EINVAL:
                        _D("%s: invalid value type\n", "name");
			return PROBE_EINVAL;
                        break;
                case EFAULT:
                        _D("%s: element not found\n", "name");
			return PROBE_ENOELM;
                        break;
		default:
			return PROBE_EUNKNOWN;
                }
        }

        reply_st  = NULL;

        /* get info from RPM db */
        switch (rpmret = get_rpminfo (&request_st, &reply_st)) {
        case 0: /* Not found */
                _D("Package \"%s\" not found.\n", request_st.name);
                break;
        case -1: /* Error */
                _D("get_rpminfo failed\n");

                item_sexp = probe_item_creat ("rpminfo_item", NULL,
                                              "name", NULL,
                                              r0 = SEXP_string_newf(request_st.name),
                                              NULL);

                probe_item_setstatus (item_sexp, OVAL_STATUS_ERROR);

		probe_cobj_add_item(probe_out, item_sexp);
                SEXP_free (item_sexp);
                SEXP_free (r0);

                break;
        default: /* Ok */
                _A(rpmret   >= 0);
                _A(reply_st != NULL);
                {
			SEXP_t *r2, *r3, *r4, *r5, *r6;

                        for (i = 0; i < rpmret; ++i) {
				r0 = SEXP_string_newf("%s", reply_st[i].name);
				if (probe_entobj_cmp(ent, r0) != OVAL_RESULT_TRUE) {
					SEXP_free(r0);
					continue;
				}

                                item_sexp = probe_item_creat ("rpminfo_item", NULL,
					"name", NULL, r0,
					"arch", NULL,
					r1 = SEXP_string_newf ("%s", reply_st[i].arch),
					"epoch", NULL,
					r2 = SEXP_string_newf ("%s", reply_st[i].epoch),
					"release", NULL,
					r3 = SEXP_string_newf ("%s", reply_st[i].release),
					"version", NULL,
					r4 = SEXP_string_newf ("%s", reply_st[i].version),
					"evr", NULL,
					r5 = SEXP_string_newf ("%s", reply_st[i].evr),
					"signature_keyid", NULL,
					r6 = SEXP_string_newf ("%s", reply_st[i].signature_keyid),
					NULL);
                                probe_cobj_add_item(probe_out, item_sexp);
				SEXP_vfree(item_sexp, r0, r1, r2, r3, r4, r5, r6, NULL);

                                __rpminfo_rep_free (&(reply_st[i]));
                        }

                        oscap_free (reply_st);
                }
        }

	SEXP_vfree(ent, NULL);
        oscap_free(request_st.name);

        return 0;
}
