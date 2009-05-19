/*
 * runlevel probe:
 *
 * runlevel_object(string service_name,
 *                 string runlevel)
 *
 * runlevel_state(string service_name,
 *                string runlevel,
 *                bool   start,
 *                bool   kill)
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include <seap.h>
#include <probe.h>
#include "xmalloc.h"

#ifndef _A
#define _A(x) assert(x)
#endif

struct runlevel_req {
        char *service_name;
        char *runlevel;
};

struct runlevel_rep {
        char *service_name;
        char *runlevel;
        char *start;
        char *kill;
};

static int get_runlevel (struct runlevel_req *req, struct runlevel_rep *rep);

void __runlevel_rep_free (struct runlevel_rep *ptr)
{
        xfree ((void **)&(ptr->service_name));
        xfree ((void **)&(ptr->runlevel));
}

#if defined(__linux__)
static int get_runlevel_redhat (struct runlevel_req *req, struct runlevel_rep *rep)
{
        char pathbuf[PATH_MAX];
        unsigned long runlevel;
        
        DIR *rcdir;
        struct dirent *dp;
        struct stat st, rc_st;
        
        _A(req != NULL);
        _A(rep != NULL);
        
        rep.service_name = req.service_name;
        rep.runlevel     = req.runlevel;
        rep.start        = "0";
        rep.kill         = "0";
        
        snprintf (pathbuf, PATH_MAX, "/etc/init.d/%s", req->service_name);
        
        if (stat (pathbuf, &st) != 0)
                return (0);
        
        /* TODO: check mode/owner? */
        
        runlevel = strtoul (req->runlevel, NULL, 10);
        
        switch (errno) {
        case EINVAL:
        case ERANGE:
                _D("Can't convert req.runlevel to a number\n");
                return (-1);
        }
        
        snprintf (pathbuf, PATH_MAX, "/etc/rc%u.d", runlevel);
        
        rcdir = opendir (pathbuf);
        while ((dp = readdir (rcdir)) != NULL) {
                if (stat (dp->d_name, &rc_st) != 0) {
                        _D("Can't stat file %s/%s: errno=%d, %s.",
                           pathbuf, dp->d_name, errno, strerror (errno));
                        continue;
                }
                
                if (rc_st.st_ino == st.st_ino) {
                        switch(dp->d_name[0]) {
                        case 'S':
                                rep->start = "1";
                                goto out;
                        case 'K':
                                rep->kill  = "1";
                                goto out;
                        default:
                                _D("Unexpected character in filename: %c, %s/%s.\n",
                                   dp->d_name[0], pathbuf, dp->d_name);
                        }
                }
        }
out:
        closedir (rcdir);
        
        return (0);
}

static int get_runlevel_debian (struct runlevel_req *req, struct runlevel_rep *rep)
{
        return (-1);
}

static int get_runlevel_slack (struct runlevel_req *req, struct runlevel_rep *rep)
{
        return (-1);
}

static int get_runlevel_gentoo (struct runlevel_req *req, struct runlevel_rep *rep)
{
        return (-1);
}

static int get_runlevel_arch (struct runlevel_req *req, struct runlevel_rep *rep)
{
        return (-1);
}

static int get_runlevel_mandriva (struct runlevel_req *req, struct runlevel_rep *rep)
{
        return (-1);
}

static int get_runlevel_suse (struct runlevel_req *req, struct runlevel_rep *rep)
{
        return (-1);
}

static int get_runlevel_common (struct runlevel_req *req, struct runlevel_rep *rep)
{
        return (-1);
}

#if !defined(LINUX_DISTRO)
# define LINUX_DISTRO "generic"
static int is_redhat (void)
{
        return (eaccess ("/etc/redhat-release", F_OK) == 0);
}

static int is_debian (void)
{
        return (eaccess ("/etc/debian_version", F_OK) == 0 ||
                eaccess ("/etc/debian_release", F_OK) == 0);
}

static int is_slack (void)
{
        return (eaccess ("/etc/slackware-release", F_OK) == 0);
}

static int is_gentoo (void)
{
        return (eaccess ("/etc/gentoo-release", F_OK) == 0);
}

static int is_arch (void)
{
        return (eaccess ("/etc/arch-release", F_OK) == 0);
}

static int is_mandriva (void)
{
        return (eaccess ("/etc/mandriva-release", F_OK) == 0);
}

static int is_suse (void)
{
        return (eaccess ("/etc/SuSE-release", F_OK)   == 0 ||
                eaccess ("/etc/sles-release", F_OK)   == 0 ||
                eaccess ("/etc/novell-release", F_OK) == 0);
}

static int is_common (void)
{
        return (1);
}

typedef struct {
        int (*distrop)(void);
        int (*get_runlevel)(struct runlevel_req *, struct runlevel_rep *);
} distro_tbl_t;

const distro_tbl_t distro_tbl[] = {
        { &is_debian,   &get_runlevel_debian   },
        { &is_redhat,   &get_runlevel_redhat   },
        { &is_slack,    &get_runlevel_slack    },
        { &is_gentoo,   &get_runlevel_gentoo   },
        { &is_arch,     &get_runlevel_arch     },
        { &is_mandriva, &get_runlevel_mandriva },
        { &is_suse,     &get_runlevel_suse     },
        { &is_common,   &get_runlevel_common   },
};

#define DISTRO_TBL_SIZE ((sizeof distro_tbl)/sizeof (distro_tbl_t));

static int get_runlevel_generic (struct runlevel_req *req, struct runlevel_rep *rep)
{
        uint16_t i;
        
        _A(req != NULL);
        _A(rep != NULL);
        
        for (i = 0; i < DISTRO_TBL_SIZE; ++i)
                if (distro_tbl[i].distrop ())
                        return distro_tbl[i].get_runlevel (req, rep);
        
        abort ();
        
        /* NOTREACHED */
        return (-1);
}
#endif

static int get_runlevel (struct runlevel_req *req, struct runlevel_rep *rep)
{
        _A(req != NULL);
        _A(rep != NULL);
        return get_runlevel_##LINUX_DISTRO (req, rep);
}
#elif defined(__FreeBSD__)
static int get_runlevel (struct runlevel_req *req, struct runlevel_rep *rep)
{
        _A(req != NULL);
        _A(rep != NULL);
        return (-1);
}
#else
# error "Sorry, your OS isn't supported."
#endif

int main (void)
{
        int ret = 0;

        SEAP_CTX_t *ctx;
        SEAP_msg_t *seap_request, *seap_reply;
        int sd;

        struct runlevel_req request_st;
        struct runlevel_rep reply_st;
        

        /* Initialize SEAP */
        ctx = SEAP_CTX_new ();
        sd  = SEAP_openfd2 (ctx, STDIN_FILENO, STDOUT_FILENO, 0);

        if (sd < 0) {
                _D("Can't create SEAP descriptor: errno=%u, %s.\n",
                   errno, strerror (errno));
                exit (errno);
        }

        /* Main loop */
        for (;;) {
                if (SEAP_recvmsg (ctx, sd, &seap_request) == -1) {
                        _D("An error ocured while receiving SEAP message. errno=%u, %s.\n",
                           errno, strerror (errno));
                        break;
                }

                request_st.service_name = SEXP_OVALobj_getelm_val (seap_request->sexp, "service_name");
                request_st.runlevel     = SEXP_OVALobj_getelm_val (seap_request->sexp, "runlevel");
                
                if (get_runlevel (&request_st, &reply_st) == -1) {
                        _D("get_runlevel failed\n");
                        ret = 1;
                        break;
                }
                
                seap_reply = SEAP_msg_new ();
                seap_reply->sexp = SEXP_OVALobj_create ("runlevel_state", NULL,
                                                        "service_name", NULL, reply_st.service_name,
                                                        "runlevel",     NULL, reply_st.runlevel,
                                                        "start",        NULL, reply_st.start,
                                                        "kill",         NULL, reply_st.kill);
                
                if (SEAP_reply (ctx, sd, seap_reply, seap_request) == -1) {
                        _D("An error ocured while sending SEAP message. errno=%u, %s.\n",
                           errno, strerror (errno));
                        break;
                }
        }
        
        /* Close SEAP descriptor */
        SEAP_close (ctx, sd);
        SEAP_CTX_free (ctx);
        
        return (ret);
}
