#include <stdlib.h>
#include <string.h>
#include <seap.h>
#include <assert.h>
#include <errno.h>
#if defined(THREAD_SAFE)
# include <pthread.h>
#endif
#include "oval_probe.h"
#include "probes/probe.h"

#ifndef _A
#define _A(x) assert(x)
#endif

/* KEEP THIS LIST SORTED! (by subtype) */
const oval_probe_t __probe_tbl[] = {
        /*  9001 */ { LINUX_DPKG_INFO, "dpkginfo", "probe_dpkginfo" },
        /*  9003 */ { LINUX_RPM_INFO,  "rpminfo",  "probe_rpminfo"  },
        /*  9004 */ { LINUX_SLACKWARE_PKG_INFO_TEST, "slackwarepkginfo", "probe_slackwarepkginfo" },
        /* 13006 */ { UNIX_RUNLEVEL, "runlevel", "probe_runlevel" }
};

#define PROBETBLSIZE (sizeof __probe_tbl / sizeof (oval_probe_t))

#if defined(THREAD_SAFE)
static pthread_once_t __init_once = PTHREAD_ONCE_INIT;
static pthread_key_t  __key;

static void probe_sdtbl_free (void *arg)
{
        /* TODO */
        return;
}

static void probe_sdtbl_init (void)
{
        probe_sdtbl_t *ptbl;
        (void) pthread_key_create (&__key, probe_sdtbl_free);
        
        ptbl = malloc (sizeof (probe_sdtbl_t));
        ptbl->memb = NULL;
        ptbl->count = 0;
        SEAP_CTX_init (&(ptbl->ctx));
        
        (void) pthread_setspecific (key, (void *)ptbl);
        return;
}
#else
static probe_sdtbl_t __probe_sdtbl = PROBE_SDTBL_INITIALIZER;
#endif

const oval_probe_t *search_probe (oval_subtype_enum typenum)
{
        uint32_t w, s;
        
        w = PROBETBLSIZE;
        s = 0;
                
        while (w > 0) {
                if (typenum > __probe_tbl[s + w/2].typenum) {
                        s += w/2 + 1;
                        w  = w - w/2 - 1;
                } else if (typenum < __probe_tbl[s + w/2].typenum) {
                        w = w/2;
                } else {
                        return &(__probe_tbl[s + w/2]);
                }
        }
#undef cmp
        return (NULL);
}

int probe_sd_get (probe_sdtbl_t *tbl, oval_subtype_enum ptype)
{
        size_t w, s;
        
        _A(tbl != NULL);
        
        /* FIXME: duplicated code */
        w = tbl->count;
        s = 0;

        while (w > 0) {
                if (ptype > tbl->memb[s + w/2].typenum) {
                        s += w/2 + 1;
                        w  = w - w/2 - 1;
                } else if (ptype < tbl->memb[s + w/2].typenum) {
                        w = w/2;
                } else {
                        return (tbl->memb[s + w/2].sd);
                }
        }
        /* Not found */
        return (-1);
}

int probe_sd_cmp (const void *a, const void *b)
{
        return (((probe_sd_t *)a)->typenum - ((probe_sd_t *)b)->typenum);
}

int probe_sd_add (probe_sdtbl_t *tbl, oval_subtype_enum type, int sd)
{
        _A(tbl != NULL);
        _A(sd >= 0);

        tbl->memb = realloc (tbl->memb, sizeof (probe_sd_t) * (++tbl->count));
        tbl->memb[tbl->count - 1].typenum = type;
        tbl->memb[tbl->count - 1].sd = sd;

        qsort (tbl->memb, tbl->count, sizeof (probe_sd_t), probe_sd_cmp);
        
        return (0);
}

int probe_sd_del (probe_sdtbl_t *tbl, oval_subtype_enum type)
{
        _A(tbl != NULL);
        /* TODO */
        return (0);
}

struct oval_iterator_syschar *sexp_to_oval_state (SEXP_t *sexp)
{
        _A(sexp != NULL);
        /* TODO */
        return (NULL);
}

struct oval_iterator_syschar *probe_simple_object (struct oval_object *object,
                                                   struct oval_iterator_variable_binding *binding)
{
        probe_sdtbl_t *ptbl = NULL;
        SEXP_t *sexp;
        int psd;
        SEAP_msg_t *msg;
                
        const oval_probe_t *probe;
        struct oval_iterator_syschar *sysch = NULL;
        
        _A(object != NULL);

#if defined(THREAD_SAFE)
        pthread_once (&__init_once, probe_sdtbl_init);
        ptbl = pthread_getspecific (__key);
#else
        ptbl = &__probe_sdtbl;
#endif
        _A(ptbl != NULL);

        probe = search_probe (oval_object_subtype(object));
        if (probe == NULL) {
                errno = EOPNOTSUPP;
                return (NULL);
        }
        
        /* create s-exp */
        sexp  = oval_object_to_sexp (probe->typestr, object);
        
        psd = probe_sd_get (ptbl, oval_object_subtype (object));
        if (psd == -1) {
                char  *uri, *dir;
                size_t len;
                
                len = (strlen (OVAL_PROBE_SCHEME) +
                       strlen (OVAL_PROBE_DIR) + 1 +
                       strlen (probe->filename));
                
                uri = malloc (sizeof (char) * (len + 1));
                if (uri == NULL) {
                        /* ENOMEM */
                        return (NULL);
                }
                
#if defined(PROBEPATH_ENV) /* insecure? */
                dir = getenv ("PROBEPATH");
                if (dir == NULL)
                        dir = OVAL_PROBE_DIR;
#else
                dir = OVAL_PROBE_DIR;
#endif
                snprintf (uri, len + 1, "%s%s/%s",
                          OVAL_PROBE_SCHEME, dir, probe->filename);
                
                psd = SEAP_connect (&(ptbl->ctx), uri, 0);
                if (psd < 0) {
                        /* connect failed */
                        psd = errno;
                        free (uri);
                        errno = psd;
                        return (NULL);
                }

                free (uri);
                probe_sd_add (ptbl, oval_object_subtype (object), psd);
        }
        
        msg = SEAP_msg_new ();
        msg->sexp = sexp;

        if (SEAP_sendmsg (&(ptbl->ctx), psd, msg) != 0) {
                /* error */
                return (NULL);
        }

        /* free sexp, msg */
        
        if (SEAP_recvmsg (&(ptbl->ctx), psd, &msg) != 0) {
                /* error */
                return (NULL);
        }
        
        /* translate the result to oval state */
        sysch = sexp_to_oval_state (msg->sexp);
        
        /* cleanup */

        return (sysch);
}
