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

/* SEAP */
#include <seap.h>
#include <probe.h>
#include "xmalloc.h"

#ifndef _A
#define _A(x) assert(x)
#endif

static rpmts RPMts = NULL;

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

void __rpminfo_rep_free (struct rpminfo_rep *ptr)
{
        xfree ((void **)&(ptr->name));
        xfree ((void **)&(ptr->arch));
        xfree ((void **)&(ptr->epoch));
        xfree ((void **)&(ptr->release));
        xfree ((void **)&(ptr->version));
        xfree ((void **)&(ptr->evr));
        xfree ((void **)&(ptr->signature_keyid));
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
	
	match = rpmdbInitIterator (RPMts, RPMTAG_NAME, (const void *)req->name, 0);
	if (NULL == match)
		return (0);
	
        ret = rpmdbGetIteratorCount (match);
        
        if (ret > 0) {
                (*rep) = xrealloc (*rep, sizeof (struct rpminfo_rep) * ret);
                
                for (i = 0; ((pkgh = rpmdbNextIterator (match)) != NULL) && i < ret; ++i) {
                        (*rep)[i].name    = headerFormat (pkgh, "%{NAME}", &rpmerr);
                        (*rep)[i].arch    = headerFormat (pkgh, "%{ARCH}", &rpmerr);
                        str = headerFormat (pkgh, "%{EPOCH}", &rpmerr);
                        if (strcmp (str, "(none)") == 0) {
                                str    = xrealloc (str, sizeof (char) * 2);
                                str[0] = '0';
                                str[1] = '\0';
                        }
                        (*rep)[i].epoch   = str;
                        (*rep)[i].release = headerFormat (pkgh, "%{RELEASE}", &rpmerr);
                        (*rep)[i].version = headerFormat (pkgh, "%{VERSION}", &rpmerr);
                        
                        len = (strlen ((*rep)[i].epoch)   +
                               strlen ((*rep)[i].release) +
                               strlen ((*rep)[i].version) + 2);
                        
                        str = xmalloc (sizeof (char) * (len + 1));
                        snprintf (str, len + 1, "%s:%s-%s",
                                  (*rep)[i].epoch,
                                  (*rep)[i].version,
                                  (*rep)[i].release);
                        
                        (*rep)[i].evr = str;
                        
                        str = headerFormat (pkgh, "%{SIGGPG:pgpsig}", &rpmerr);
                        sid = strrchr (str, ' ');
                        (*rep)[i].signature_keyid = (sid != NULL ? strdup (sid+1) : strdup ("0"));
                        xfree ((void **)&str);
                }

                if (ret != i) {
                        _D("Something bad happened...\n");
                        
                        while (i > 0)
                                __rpminfo_rep_free (&((*rep)[--i]));
                        
                        xfree ((void **)&(*rep));
                        ret = -1;
                }
        }
	
	match = rpmdbFreeIterator (match);
	return (ret);
}

int main (void)
{
        SEAP_CTX_t *ctx;
        SEAP_msg_t *seap_request, *seap_reply;
        
        SEXP_t *state_sexp, *val, *obj;
        int sd;
        
        int rpmret, i, ret = 0;
        struct rpminfo_req request_st;
        struct rpminfo_rep *reply_st = NULL;
        
        /* Initialize SEAP */
        ctx = SEAP_CTX_new ();
        sd  = SEAP_openfd2 (ctx, STDIN_FILENO, STDOUT_FILENO, 0);

        if (sd < 0) {
                _D("Can't create SEAP descriptor: errno=%u, %s.\n",
                   errno, strerror (errno));
                exit (1);
        }
        
        /* Initialize RPM db */
        if (rpmReadConfigFiles ((const char *)NULL, (const char *)NULL) != 0)
        {
                _D("rpmReadConfigFiles failed: %u, %s.\n", errno, strerror (errno));
                exit (errno);
        }
        
        /* Initialize transaction... stuff. */
        RPMts = rpmtsCreate ();
        
        /* Main loop */
        for (;;) {
                /* receive S-exp */
                if (SEAP_recvmsg (ctx, sd, &seap_request) == -1) {
                        ret = errno;
                        
                        _D("An error ocured while receiving SEAP message. errno=%u, %s.\n",
                           errno, strerror (errno));
                        
                        break;
                }
                
                obj = SEAP_msg_get (seap_request);
                
                /* get the desired values from an OVAL object */
                val = SEXP_OVALobj_getelmval (obj, "name", 1);
                request_st.name = SEXP_string_cstr (val);
                
                if (request_st.name == NULL) {
                        int err;
                        
                        switch (errno) {
                        case EINVAL:
                                _D("%s: invalid value type\n", "name");
                                err = PROBE_ERR_INVALIDOBJ;
                                break;
                        case EFAULT:
                                _D("%s: element not found\n", "name");
                                err = PROBE_ERR_MISSINGVAL;
                                break;
                        }
                        
                        if (SEAP_senderr (ctx, sd, err, seap_request) == -1) {
                                _D("An error ocured while sending error status. errno=%u, %s.\n",
                                   errno, strerror (errno));
                                
                                SEAP_msg_free (seap_request);
                                break;
                        }
                        
                        SEAP_msg_free (seap_request);
                        continue;
                }
                
                seap_reply = SEAP_msg_new ();
                
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
                        SEAP_msg_set (seap_reply, item_sexp);
                        
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
                        SEAP_msg_set (seap_reply, item_sexp);
                        
                        break;

                default: /* Ok */
                        _A(rpmret >= 0);
                        
                        if (rpmret == 1) {
                                item_sexp = SEXP_OVALobj_create ("rpminfo_item", NULL,
                                                                 
                                                                 "name", NULL,
                                                                 SEXP_string_newf (reply_st[0].name),

                                                                 "arch", NULL,
                                                                 SEXP_string_newf (reply_st[0].arch),

                                                                 "epoch", NULL,
                                                                 SEXP_string_newf (reply_st[0].epoch),

                                                                 "release", NULL,
                                                                 SEXP_string_newf (reply_st[0].release),

                                                                 "version", NULL,
                                                                 SEXP_string_newf (reply_st[0].version),

                                                                 "evr", NULL,
                                                                 SEXP_string_newf (reply_st[0].evr),
                                                                 
                                                                 "signature_keyid", NULL,
                                                                 SEXP_string_newf (reply_st[0].signature_keyid),

                                                                 NULL);
                                
                                __rpminfo_rep_free (&reply_st[0]);
                                xfree ((void **)&reply_st);
                                
                                SEAP_msg_set (seap_reply, item_sexp);
                        } else {
                                /* TODO */
                                for (i = 0; i < rpmret; ++i)
                                        __rpminfo_rep_free (&reply_st[i]);
                                xfree ((void **)&reply_st);
                        }
                }
                
                if (SEAP_reply (ctx, sd, seap_reply, seap_request) == -1) {
                        ret = errno;
                        
                        _D("An error ocured while sending SEAP message. errno=%u, %s.\n",
                           errno, strerror (errno));
                        
                        SEAP_msg_free (seap_reply);
                        SEAP_msg_free (seap_request);
                        
                        break;
                }

                SEAP_msg_free (seap_reply);
                SEAP_msg_free (seap_request);
        }
out:
        /* Close SEAP descriptor */
        SEAP_close (ctx, sd);
        
        /* Free RPM related stuff */
        rpmtsFree (RPMts);
        
        return (ret);
}
