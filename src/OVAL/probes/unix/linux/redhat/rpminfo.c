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
	int ret = 0;
	char *str, *sid;
	size_t len;
	errmsg_t rpmerr;
	
	match = rpmtsInitIterator (RPMts, RPMTAG_NAME, (const void *)req->name, 0);
	if (NULL == match)
		return (-1);
	
	while ((pkgh = rpmdbNextIterator (match)) != NULL) {
		(*rep) = xrealloc(*rep, sizeof (struct rpminfo_rep) * (++ret));
                
		(*rep)[ret - 1].name    = headerFormat (pkgh, "%{NAME}", &rpmerr);
		(*rep)[ret - 1].arch    = headerFormat (pkgh, "%{ARCH}", &rpmerr);
		str = headerFormat (pkgh, "%{EPOCH}", &rpmerr);
		if (strcmp (str, "(none)") == 0) {
			str    = xrealloc (str, sizeof (char) * 2);
			str[0] = '0';
			str[1] = '\0';
		}
		(*rep)[ret - 1].epoch   = str;
		(*rep)[ret - 1].release = headerFormat (pkgh, "%{RELEASE}", &rpmerr);
		(*rep)[ret - 1].version = headerFormat (pkgh, "%{VERSION}", &rpmerr);
		
		len = (strlen ((*rep)[ret - 1].epoch)   +
		       strlen ((*rep)[ret - 1].release) +
		       strlen ((*rep)[ret - 1].version) + 2);
		
		str = xmalloc (sizeof (char) * (len + 1));
		snprintf (str, len + 1, "%s:%s-%s",
			  (*rep)[ret - 1].epoch,
			  (*rep)[ret - 1].version,
			  (*rep)[ret - 1].release);
		
		(*rep)[ret - 1].evr = str;
		
		str = headerFormat (pkgh, "%{SIGGPG:pgpsig}", &rpmerr);
		sid = strrchr (str, ' ');
		(*rep)[ret - 1].signature_keyid = (sid != NULL ? strdup (sid+1) : strdup ("0"));
		xfree ((void **)&str);
	}
	
	match = rpmdbFreeIterator (match);
	return (ret);
}

int main (void)
{
        SEAP_CTX_t *ctx;
        SEXP_t *state_sexp;
        int sd, rpmret, i, ret = 0;
        SEAP_msg_t *seap_request, *seap_reply;
        struct rpminfo_req request_st;
        struct rpminfo_rep *reply_st = NULL;
        
        /* Initialize SEAP */
        ctx = SEAP_CTX_new ();
        sd  = SEAP_openfd2 (ctx, STDIN_FILENO, STDOUT_FILENO, 0);
        
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
                        /* handle the error */
                        _D("An error ocured while receiving SEAP message. errno=%u, %s.\n",
                           errno, strerror (errno));
                        break;
                }
                
                /* get the desired values from an OVAL object */
                request_st.name = SEXP_OVALobj_getelement_value (seap_request->sexp, "name");
                if (request_st.name == NULL) {
                        /* Element not found */
                        _D("Can't extract package name from the received message.\n");
                        /* FIXME: free the message */
                        continue;
                }

                /* get info from RPM db */
                switch (rpmret = get_rpminfo (&request_st, &reply_st)) {
                case 0: /* Not found */
                        _D("Package \"%s\" not found.\n", request_st.name);

                        seap_reply = SEAP_msg_new ();
                        seap_reply->sexp = SEXP_string_new ("nil", 3); 
                        break;
                case -1: /* Error */
                        _D("get_rpminfo failed\n");
                        ret = 1;
                        goto out;
                default: /* Ok */
                        _A(rpmret >= 0);
                        
                        seap_reply = SEAP_msg_new ();
                        seap_reply->sexp = SEXP_list_new ();
                        
                        for (i = 0; i < rpmret; ++i) {
                                state_sexp = SEXP_OVALobj_create ("rpminfo_state", NULL,
                                                                  "name",    NULL, reply_st[i].name,
                                                                  "arch",    NULL, reply_st[i].arch,
                                                                  "epoch",   NULL, reply_st[i].epoch,
                                                                  "release", NULL, reply_st[i].release,
                                                                  "version", NULL, reply_st[i].version,
                                                                  "evr",     NULL, reply_st[i].evr,
                                                                  "signature_keyid", NULL, reply_st[i].signature_keyid,
                                                                  NULL);
                                
                                SEXP_list_add (seap_reply->sexp, state_sexp);
                        }
                }
                
                if (SEAP_reply (ctx, sd, seap_reply, seap_request) == -1) {
                        /* Handle the error */
                        _D("An error ocured while sending SEAP message. errno=%u, %s.\n",
                           errno, strerror (errno));
                        break;
                }
        }
out:
        /* Close SEAP descriptor */
        SEAP_close (ctx, sd);
        
        /* Free RPM related stuff */
        rpmtsFree (RPMts);
        
        return (ret);
}
