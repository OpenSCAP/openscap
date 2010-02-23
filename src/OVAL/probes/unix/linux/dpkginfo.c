#ifndef __STUB_PROBE
/*
 * dpkginfo probe:
 *
 *  dpkginfo_object(string name)
 *
 *  dpkginfo_state(string name,
 *                string arch,
 *                string epoch,
 *                string release,
 *                string version,
 *                string evr,
 *                string signature_keyid)
 */
/*
 * Author: Pierre Chifflier <chifflier@edenwall.com>
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

/* SEAP */
#include <seap.h>
#include <probe-api.h>
#include <alloc.h>


#include "dpkginfo-helper.h"


struct dpkginfo_global {
        pthread_mutex_t mutex;
};

static struct dpkginfo_global g_dpkg;


void *probe_init(void)
{
        pthread_mutex_init (&(g_dpkg.mutex), NULL);
        dpkginfo_init();

        return ((void *)&g_dpkg);
}

void probe_fini (void *ptr)
{
        struct dpkginfo_global *d = (struct dpkginfo_global *)ptr;

        pthread_mutex_destroy (&(d->mutex));
        dpkginfo_fini();

        return;
}

SEXP_t *probe_main (SEXP_t *object, int *err, void *arg)
{
        SEXP_t *probe_out, *val, *item_sexp, *r0;
        char *request_st = NULL;
        struct dpkginfo_reply_t *dpkginfo_reply = NULL;
        int errflag;

        val = probe_obj_getentval (object, "name", 1);

        if (val == NULL) {
                _D("%s: no value\n", "name");
                *err = PROBE_ENOVAL;
                return (NULL);
        }

        request_st = SEXP_string_cstr (val);
        SEXP_free (val);

        if (request_st == NULL) {
                switch (errno) {
                case EINVAL:
                        _D("%s: invalid value type\n", "name");
                        *err = PROBE_EINVAL;
                        break;
                case EFAULT:
                        _D("%s: element not found\n", "name");
                        *err = PROBE_ENOELM;
                        break;
                }

                return (NULL);
        }

        probe_out = SEXP_list_new (NULL);

        /* get info from debian apt cache */
        pthread_mutex_lock (&(g_dpkg.mutex));
        dpkginfo_reply = dpkginfo_get_by_name(request_st, &errflag);
        pthread_mutex_unlock (&(g_dpkg.mutex));

        if (dpkginfo_reply == NULL) {
                switch (errflag) {
                        case 0: /* Not found */
                                _D("Package \"%s\" not found.\n", request_st);

                                item_sexp = probe_item_creat ("dpkginfo_item", NULL,
                                                "name", NULL,
                                                r0 = SEXP_string_newf(request_st),
                                                NULL);

                                probe_item_setstatus (item_sexp, OVAL_STATUS_DOESNOTEXIST);
                                probe_itement_setstatus (item_sexp, "name", 1, OVAL_STATUS_DOESNOTEXIST);

                                SEXP_list_add (probe_out, item_sexp);
                                SEXP_free (item_sexp);
                                SEXP_free (r0);

                                break;
                        case -1: /* Error */
                                _D("get_dpkginfo failed\n");

                                item_sexp = probe_item_creat ("dpkginfo_item", NULL,
                                                "name", NULL,
                                                r0 = SEXP_string_newf(request_st),
                                                NULL);

                                probe_item_setstatus (item_sexp, OVAL_STATUS_ERROR);

                                SEXP_list_add (probe_out, item_sexp);
                                SEXP_free (item_sexp);
                                SEXP_free (r0);

                                break;
                }
        } else { /* Ok */
                SEXP_t *r1, *r2, *r3, *r4, *r5;
                int i;
                int num_items = 1; /* FIXME */

                for (i = 0; i < num_items; ++i) {
                        item_sexp = probe_item_creat ("dpkginfo_item", NULL,

                                        "name", NULL,
                                        r0 = SEXP_string_newf (dpkginfo_reply->name),

                                        "arch", NULL,
                                        r1 = SEXP_string_newf (dpkginfo_reply->arch),

                                        "epoch", NULL,
                                        r2 = SEXP_string_newf (dpkginfo_reply->epoch),

                                        "release", NULL,
                                        r3 = SEXP_string_newf (dpkginfo_reply->release),

                                        "version", NULL,
                                        r4 = SEXP_string_newf (dpkginfo_reply->version),

                                        "evr", NULL,
                                        r5 = SEXP_string_newf (dpkginfo_reply->evr),

                                        NULL, NULL,
                                        NULL,

                                        NULL);

                        SEXP_list_add (probe_out, item_sexp);
                        SEXP_free (item_sexp);
                        /* FIXME: this is... stupid */
                        SEXP_free (r0);
                        SEXP_free (r1);
                        SEXP_free (r2);
                        SEXP_free (r3);
                        SEXP_free (r4);
                        SEXP_free (r5);

                        dpkginfo_free_reply(dpkginfo_reply);
                }
        }

        *err = 0;
        return (probe_out);
}

#endif

