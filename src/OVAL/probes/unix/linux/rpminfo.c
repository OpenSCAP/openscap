#ifndef __STUB_PROBE
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
/* RPM headers */
#include <rpm/rpmdb.h>
#include <rpm/rpmlib.h>
#include <rpm/rpmts.h>

/* SEAP */
#include <seap.h>
#include <probe.h>
#include <common/alloc.h>

#ifndef _A
#define _A(x) assert(x)
#endif

struct rpminfo_req {
        char *name;
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
        rpmts           rpmts;
        pthread_mutex_t mutex;
};

static struct rpminfo_global g_rpm;

void __rpminfo_rep_free (struct rpminfo_rep *ptr)
{
        oscap_free (ptr->name);
        oscap_free (ptr->arch);
        oscap_free (ptr->epoch);
        oscap_free (ptr->release);
        oscap_free (ptr->version);
        oscap_free (ptr->evr);
        oscap_free (ptr->signature_keyid);
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
	char *str, *sid;
	size_t len;
	errmsg_t rpmerr;
	
        
        pthread_mutex_lock (&(g_rpm.mutex));
        match = rpmtsInitIterator (g_rpm.rpmts, RPMTAG_NAME, (const void *)req->name, 0);
	
        if (NULL == match) {
                ret = 0;
                goto ret;
	}
        
        ret = rpmdbGetIteratorCount (match);
        
        if (ret > 0) {
                (*rep) = oscap_realloc (*rep, sizeof (struct rpminfo_rep) * ret);
                
                for (i = 0; ((pkgh = rpmdbNextIterator (match)) != NULL) && i < ret; ++i) {
                        (*rep)[i].name    = headerFormat (pkgh, "%{NAME}", &rpmerr);
                        (*rep)[i].arch    = headerFormat (pkgh, "%{ARCH}", &rpmerr);
                        str = headerFormat (pkgh, "%{EPOCH}", &rpmerr);
                        if (strcmp (str, "(none)") == 0) {
                                str    = oscap_realloc (str, sizeof (char) * 2);
                                str[0] = '0';
                                str[1] = '\0';
                        }
                        (*rep)[i].epoch   = str;
                        (*rep)[i].release = headerFormat (pkgh, "%{RELEASE}", &rpmerr);
                        (*rep)[i].version = headerFormat (pkgh, "%{VERSION}", &rpmerr);
                        
                        len = (strlen ((*rep)[i].epoch)   +
                               strlen ((*rep)[i].release) +
                               strlen ((*rep)[i].version) + 2);
                        
                        str = oscap_alloc (sizeof (char) * (len + 1));
                        snprintf (str, len + 1, "%s:%s-%s",
                                  (*rep)[i].epoch,
                                  (*rep)[i].version,
                                  (*rep)[i].release);
                        
                        (*rep)[i].evr = str;
                        
                        str = headerFormat (pkgh, "%{SIGGPG:pgpsig}", &rpmerr);
                        sid = strrchr (str, ' ');
                        (*rep)[i].signature_keyid = (sid != NULL ? strdup (sid+1) : strdup ("0"));
                        oscap_free (str);
                }

                if (ret != i) {
                        _D("Something bad happened...\n");
                        
                        if (i > 0) {
                                do {
                                        __rpminfo_rep_free (&((*rep)[--i]));
                                } while (i > 0);
                                
                                oscap_free (*rep);
                        }
                        
                        ret = -1;
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
        
        g_rpm.rpmts = rpmtsCreate ();
        pthread_mutex_init (&(g_rpm.mutex), NULL);
        
        return ((void *)&g_rpm);
}

void probe_fini (void *ptr)
{
        struct rpminfo_global *r = (struct rpminfo_global *)ptr;
        
        rpmtsFree (r->rpmts);
        pthread_mutex_destroy (&(r->mutex));
        
        return;
}

SEXP_t *probe_main (SEXP_t *object, int *err, void *arg)
{
        int i;
        SEXP_t *probe_out, *val, *item_sexp;
	int rpmret;

        struct rpminfo_req request_st;
        struct rpminfo_rep *reply_st;

        val = SEXP_OVALobj_getelmval (object, "name", 1, 1);
        request_st.name = SEXP_string_cstr (val);
        
        if (request_st.name == NULL) {
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
                
        probe_out = SEXP_list_new ();
        
        /* get info from RPM db */
        switch (rpmret = get_rpminfo (&request_st, &reply_st)) {
        case 0: /* Not found */
                _D("Package \"%s\" not found.\n", request_st.name);
                        
                item_sexp = SEXP_OVALobj_create ("rpminfo_item", NULL,
                                                         
                                                 "name", NULL,
                                                 SEXP_string_newf(request_st.name),
                                                         
                                                 "arch",    NULL, NULL,
                                                 "epoch",   NULL, NULL,
                                                 "release", NULL, NULL,
                                                 "version", NULL, NULL,
                                                 "evr",     NULL, NULL,
                                                 "signature_keyid", NULL, NULL,
                                                 NULL);
                        
                SEXP_OVALobj_setstatus (item_sexp, OVAL_STATUS_DOESNOTEXIST);
                SEXP_list_add (probe_out, item_sexp);
                        
                break;
        case -1: /* Error */
                _D("get_rpminfo failed\n");
                        
                item_sexp = SEXP_OVALobj_create ("rpminfo_item", NULL,
                                                         
                                                 "name", NULL,
                                                 SEXP_string_newf(request_st.name),
                                                         
                                                 "arch",    NULL, NULL,
                                                 "epoch",   NULL, NULL,
                                                 "release", NULL, NULL,
                                                 "version", NULL, NULL,
                                                 "evr",     NULL, NULL,
                                                 "signature_keyid", NULL, NULL,
                                                 NULL);
                        
                SEXP_OVALobj_setstatus (item_sexp, OVAL_STATUS_ERROR);
                SEXP_list_add (probe_out, item_sexp);
                
                break;
        default: /* Ok */
                _A(rpmret   >= 0);
                _A(reply_st != NULL);
                
                for (i = 0; i < rpmret; ++i) {
                        item_sexp = SEXP_OVALobj_create ("rpminfo_item", NULL,
                                                         
                                                         "name", NULL,
                                                         SEXP_string_newf (reply_st[i].name),
                                                         
                                                         "arch", NULL,
                                                         SEXP_string_newf (reply_st[i].arch),

                                                         "epoch", NULL,
                                                         SEXP_string_newf (reply_st[i].epoch),

                                                         "release", NULL,
                                                         SEXP_string_newf (reply_st[i].release),

                                                         "version", NULL,
                                                         SEXP_string_newf (reply_st[i].version),

                                                         "evr", NULL,
                                                         SEXP_string_newf (reply_st[i].evr),
                                                                 
                                                         "signature_keyid", NULL,
                                                         SEXP_string_newf (reply_st[i].signature_keyid),

                                                         NULL);
                        
                        SEXP_list_add (probe_out, item_sexp);
                        __rpminfo_rep_free (&(reply_st[i]));
                }
                
                oscap_free (reply_st);
        }
        
        if (reply_st != NULL)
                oscap_free (reply_st);
        
        *err = 0;

        return (probe_out);
}
#endif
