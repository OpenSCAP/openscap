#ifndef __STUB_PROBE

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <seap.h>
#include <assert.h>
#include <errno.h>
#include <common/bfind.h>
#if defined(OSCAP_THREAD_SAFE)
# include <pthread.h>
#endif

#include "oval_sexp.h"
#include "oval_probe.h"
#include "oval_system_characteristics_impl.h"
#include "probes/public/probe-api.h"

/* KEEP THIS LIST SORTED! (by subtype) */
static const oval_probe_t __ovalp_ltable[] = {
        /*  7001 */ { OVAL_INDEPENDENT_FAMILY,               "family",            "probe_family"            },
        /*  7006 */ { OVAL_INDEPENDENT_TEXT_FILE_CONTENT_54, "textfilecontent54", "probe_textfilecontent54" },
        /*  7010 */ { OVAL_INDEPENDENT_XML_FILE_CONTENT,     "xmlfilecontent",    "probe_xmlfilecontent"    },
        /*  7999 */ { 7999,                                  "system_info",       "probe_system_info"       },
        /*  9001 */ { OVAL_LINUX_DPKG_INFO,                  "dpkginfo",          "probe_dpkginfo"          },
        /*  9003 */ { OVAL_LINUX_RPM_INFO,                   "rpminfo",           "probe_rpminfo"           },
        /*  9004 */ { OVAL_LINUX_SLACKWARE_PKG_INFO_TEST,    "slackwarepkginfo",  "probe_slackwarepkginfo"  },
        /* 13001 */ { OVAL_UNIX_FILE,                        "file",              "probe_file"              },
        /* 13006 */ { OVAL_UNIX_RUNLEVEL,                    "runlevel",          "probe_runlevel"          }
};

#define OVALP_LTBL_SIZE (sizeof __ovalp_ltable / sizeof (oval_probe_t))

static SEXP_t *ovalp_cmd_obj_eval  (SEXP_t *sexp, void *arg);
static SEXP_t *ovalp_cmd_ste_fetch (SEXP_t *sexp, void *arg);
static int     ovalp_cmd_init (SEAP_CTX_t *ctx, struct oval_definition_model *model);

#if defined(OSCAP_THREAD_SAFE)
static pthread_once_t __ovalp_init_once = PTHREAD_ONCE_INIT;
static pthread_key_t  __ovalp_init_key;
#else
static ovalp_sdtbl_t *__ovalp_table = NULL;
#endif

static void ovalp_sdtbl_init (void);
static void ovalp_sdtbl_free (void);

static int  ovalp_subtype_cmp (oval_subtype_t *a, oval_probe_t *b);

static int         ovalp_sd_add (ovalp_sdtbl_t *table, oval_subtype_t type, int sd, const char *uri);
static ovalp_sd_t *ovalp_sd_get (ovalp_sdtbl_t *table, oval_subtype_t type);
static int         ovalp_sd_cmp (const ovalp_sd_t *a, const ovalp_sd_t *b);

static void ovalp_sdtbl_free (void)
{
        return;
}

static void ovalp_sdtbl_init (void)
{
        ovalp_sdtbl_t *p_tbl;

#if defined(OSCAP_THREAD_SAFE)        
        (void) pthread_key_create (&__ovalp_init_key, (void (*)(void *)) ovalp_sdtbl_free);
#endif
        p_tbl = oscap_talloc (ovalp_sdtbl_t);
        
        p_tbl->memb  = NULL;
        p_tbl->count = 0;
        p_tbl->ctx   = SEAP_CTX_new ();
        p_tbl->flags = 0;
      
#if defined(OSCAP_THREAD_SAFE)          
        (void) pthread_setspecific (__ovalp_init_key, (void *)p_tbl);
#else
        __ovalp_table = p_tbl;
#endif  
        return;
}

static int ovalp_cmd_init (SEAP_CTX_t *ctx, struct oval_definition_model *model)
{
        _A(ctx != NULL);
        
        if (SEAP_cmd_register (ctx, PROBECMD_OBJ_EVAL, SEAP_CMDREG_USEARG,
                               &ovalp_cmd_obj_eval, (void *)model) != 0)
        {
                _D("FAIL: can't register command: %s: errno=%u, %s.\n",
                   "obj_eval", errno, strerror (errno));
                return (-1);
        }
        
        if (SEAP_cmd_register (ctx, PROBECMD_STE_FETCH, SEAP_CMDREG_USEARG,
                               &ovalp_cmd_ste_fetch, (void *)model) != 0)
        {
                _D("FAIL: can't register command: %s: errno=%u, %s.\n",
                   "ste_fetch", errno, strerror (errno));
                
                /* FIXME: unregister the first command */
                
                return (-1);
        }
        
        return (0);
}

static int  ovalp_subtype_cmp (oval_subtype_t *a, oval_probe_t *b)
{
        return (*a - b->typenum);
}

const oval_probe_t *ovalp_lookup (oval_subtype_t typenum)
{
        return (oscap_bfind ((void *)__ovalp_ltable, OVALP_LTBL_SIZE, sizeof __ovalp_ltable[0],
                             &typenum, (int (*) (void *, void *)) ovalp_subtype_cmp));
}

oval_subtype_t ovalp_lookup_type (const char *name)
{
        _A(name != NULL);
        for (unsigned int i = 0; i < OVALP_LTBL_SIZE; ++i)
                if (strcmp(__ovalp_ltable[i].typestr, name) == 0)
                        return __ovalp_ltable[i].typenum;
        return (0);
}

static ovalp_sd_t *ovalp_sd_get (ovalp_sdtbl_t *tbl, oval_subtype_t subtype)
{
        _A(tbl != NULL);
        return oscap_bfind ((void *)(tbl->memb), tbl->count, sizeof (ovalp_sd_t),
                            &subtype, (int (*) (void *, void *)) ovalp_subtype_cmp);
}

static int ovalp_sd_cmp (const ovalp_sd_t *a, const ovalp_sd_t *b)
{
        return (a->typenum - b->typenum);
}

static int ovalp_sd_add (ovalp_sdtbl_t *tbl, oval_subtype_t type, int sd, const char *uri)
{
        _A(tbl != NULL);
        _A(uri != NULL);
        
        tbl->memb = realloc (tbl->memb, sizeof (ovalp_sd_t) * (++tbl->count));
        tbl->memb[tbl->count - 1].typenum = type;
        tbl->memb[tbl->count - 1].sd      = sd;
        tbl->memb[tbl->count - 1].uri     = strdup (uri);
        
        qsort (tbl->memb, tbl->count, sizeof (ovalp_sd_t), (int (*) (const void *, const void *))ovalp_sd_cmp);

        return (0);
}

static int ovalp_sd_del (ovalp_sdtbl_t *tbl, oval_subtype_t type)
{
        _A(tbl != NULL);
        /* TODO */
        return (0);
}

struct oval_sysinfo *oval_sysinfo_probe (void)
{
        struct oval_sysinfo *sysinf;
        SEXP_t *obj;

        int retry;
        SEAP_CTX_t *s_ctx;
        int         sd;
        SEAP_msg_t *s_imsg, *s_omsg;
        
        char   probe_uri[PATH_MAX+1];
        size_t probe_urilen;
        char  *probe_dir;

#if defined(OVAL_PROBEDIR_ENV)
        probe_dir = getenv ("OVAL_PROBE_DIR");
        
        if (probe_dir == NULL)
                probe_dir = OVAL_PROBE_DIR;
#else
        probe_dir = OVAL_PROBE_DIR;
#endif
        _A(probe_dir != NULL);
        
        probe_urilen = snprintf (probe_uri, sizeof probe_uri, "%s://%s/probe_system_info",
                                 OVAL_PROBE_SCHEME, probe_dir);

        _A(probe_urilen < sizeof probe_uri);        
        _D("URI: %s\n", probe_uri);

        s_ctx = SEAP_CTX_new ();
        
        if (s_ctx == NULL)
                return (NULL);
        
        {
                SEXP_t *attrs, *r0;

                obj = probe_obj_creat ("sysinfo_object",
                                       attrs = probe_attr_creat ("id", r0 = SEXP_string_newf ("sysinfo:0"),
                                                                 NULL),
                                       NULL);
                SEXP_vfree (attrs, r0, NULL);
        }
        
        for (sd = -1, retry = 0;;) {
                /*
                 * connect
                 */
                if (sd == -1) {
                        sd = SEAP_connect (s_ctx, probe_uri, 0);
                        
                        if (sd < 0) {
                                _D("Can't connect: %u, %s.\n", errno, strerror (errno));
                                
                                if (++retry <= OVAL_PROBE_MAXRETRY) {
                                        _D("connect: retry %u/%u\n", retry, OVAL_PROBE_MAXRETRY);
                                        continue;
                                } else {
                                        _D("connect: retry limit (%u) reached.\n", OVAL_PROBE_MAXRETRY);
                                        SEXP_free (obj);
                                        return (NULL);
                                }
                        }
                }
                
                /*
                 * send
                 */
                s_omsg = SEAP_msg_new ();
                SEAP_msg_set (s_omsg, obj);
                
                if (SEAP_sendmsg (s_ctx, sd, s_omsg) != 0) {
                        _D("Can't send message: %u, %s\n", errno, strerror (errno));
                        
                        if (SEAP_close (s_ctx, sd) != 0) {
                                _D("Can't close sd: %u, %s\n", errno, strerror (errno));
                                
                                SEAP_msg_free (s_omsg);
                                SEXP_free (obj);
                                
                                return (NULL);
                        }
                        
                        sd = -1;
                        
                        if (++retry <= OVAL_PROBE_MAXRETRY) {
                                _D("send: retry %u/%u\n", retry, OVAL_PROBE_MAXRETRY);
                                continue;
                        } else {
                                _D("send: retry limit (%u) reached.\n", OVAL_PROBE_MAXRETRY);
                                SEAP_msg_free (s_omsg);
                                SEXP_free (obj);

                                return (NULL);
                        }
                }
                
                /*
                 * receive
                 */
        recv_retry:
                
                if (SEAP_recvmsg (s_ctx, sd, &s_imsg) != 0) {
                        _D("Can't receive message: %u, %s\n", errno, strerror (errno));
                        
                        switch (errno) {
                        case ECANCELED:
                        {
                                SEAP_err_t *err = NULL;
                                
                                if (SEAP_recverr_byid (s_ctx, sd, &err, SEAP_msg_id (s_omsg)) != 0)
                                        goto recv_retry;
                                
                                /* 
                                 * decide what to do based on the error code/type
                                 */
                        } break;
                        }
                        
                        if (SEAP_close (s_ctx, sd) != 0) {
                                _D("Can't close sd: %u, %s\n", errno, strerror (errno));
                                
                                SEAP_msg_free (s_imsg);
                                SEAP_msg_free (s_omsg);
                                SEXP_free (obj);

                                return (NULL);
                        }
                        
                        sd = -1;

                        if (++retry <= OVAL_PROBE_MAXRETRY) {
                                _D("recv: retry %u/%u\n", retry, OVAL_PROBE_MAXRETRY);
                                continue;
                        } else {
                                _D("recv: retry limit (%u) reached.\n", OVAL_PROBE_MAXRETRY);
                                
                                SEAP_msg_free (s_imsg);
                                SEAP_msg_free (s_omsg);
                                SEXP_free (obj);
                                                        
                                return (NULL);
                        }
                }
                
                /*
                 * close
                 */
                
                SEAP_close (s_ctx, sd);
                break;
        }
        
        SEXP_free (obj);
        SEAP_msg_free (s_omsg);
        sysinf = oval_sysinfo_new ();
        
        /*
         * Translate S-exp to sysinfo structure
         */
#include <sys/cdefs.h>
        
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
        
        
        obj = SEAP_msg_get (s_imsg);
        
        if (obj == NULL) {
                oval_sysinfo_free (sysinf);
                return (NULL);
        }
        
        SYSINF_EXT(obj, os_name, sysinf, fail);
        SYSINF_EXT(obj, os_version, sysinf, fail);
        SYSINF_EXT(obj, os_architecture, sysinf, fail);
        SYSINF_EXT(obj, primary_host_name, sysinf, fail);
        
        SEXP_free (obj);
        SEAP_msg_free (s_imsg);
        
        return (sysinf);
fail:
        oval_sysinfo_free (sysinf);
        SEXP_free (obj);
        SEAP_msg_free (s_imsg);
        
        return (NULL);
}

struct oval_syschar *oval_object_probe (struct oval_object *object, struct oval_definition_model *model)
{
        const  oval_probe_t *probe;
        struct oval_syschar *sysch;
        int retry;
        
        ovalp_sdtbl_t *p_tbl;
        
        SEAP_msg_t *s_omsg, *s_imsg;
        SEXP_t     *s_exp, *r0;
        ovalp_sd_t *psd;
        
        _A(object != NULL);
        _A(model  != NULL);

        probe = NULL;
        sysch = NULL;
        
        psd    = NULL;
        s_exp  = NULL;
        s_omsg = NULL;
        s_imsg = NULL;

#if defined(OSCAP_THREAD_SAFE)
        pthread_once (&__ovalp_init_once, ovalp_sdtbl_init);
        p_tbl = pthread_getspecific (__ovalp_init_key);
#else
        if (__ovalp_table == NULL)
                ovalp_sbtbl_init ();
        
        p_tbl = __ovalp_table;
#endif
        
        _A(p_tbl != NULL);
        
        if (!(p_tbl->flags & OVALP_SDTBL_CMDDONE)) {
                if (ovalp_cmd_init (p_tbl->ctx, model) != 0) {
                        _D("FAIL: SEAP cmd init failed\n");
                        return (NULL);
                }

                p_tbl->flags |= OVALP_SDTBL_CMDDONE;
        }
        
        probe = ovalp_lookup (oval_object_get_subtype(object));
        
        if (probe == NULL) {
                errno = EOPNOTSUPP;
                return (NULL);
        }
        
        _D("oval -> sexp\n");
        
        s_exp = oval_object_to_sexp (probe->typestr, object);
        
        if (s_exp == NULL) {
                _D("Can't translate OVAL object to S-exp\n");
                return (NULL);
        }

#if !defined(NDEBUG)
        fprintf (stderr,   "--- msg out ---\n");
        SEXP_fprintfa (stderr, s_exp);
        fprintf (stderr, "\n---------------\n");
#endif
        
        psd = ovalp_sd_get (p_tbl, oval_object_get_subtype (object));
        
        if (psd == NULL) {
                char   probe_uri[PATH_MAX+1];
                size_t probe_urilen;
                char  *probe_dir;
                
#if defined(OVAL_PROBEDIR_ENV)
                probe_dir = getenv ("OVAL_PROBE_DIR");
                
                if (probe_dir == NULL)
                        probe_dir = OVAL_PROBE_DIR;
#else
                probe_dir = OVAL_PROBE_DIR;
#endif
                _A(probe_dir != NULL);
                
                probe_urilen = snprintf (probe_uri, sizeof probe_uri,
                                         "%s://%s/%s",
                                         OVAL_PROBE_SCHEME, probe_dir, probe->filename);
                
                _D("URI: %s\n", probe_uri);
                
                if (ovalp_sd_add (p_tbl, oval_object_get_subtype (object), -1, probe_uri) != 0) {
                        return (NULL);
                } else
                        psd = ovalp_sd_get (p_tbl, oval_object_get_subtype (object));
        }
        
        _A(psd != NULL);
        
        for (retry = 0;;) {
                
                if (psd->sd == -1) {
                        psd->sd = SEAP_connect (p_tbl->ctx, psd->uri, 0);
                        
                        if (psd->sd < 0) {
                                _D("Can't connect: %u, %s.\n", errno, strerror (errno));
                                
                                if (++retry <= OVAL_PROBE_MAXRETRY) {
                                        _D("connect: retry %u/%u\n", retry, OVAL_PROBE_MAXRETRY);
                                        continue;
                                } else {
                                        _D("connect: retry limit (%u) reached.\n", OVAL_PROBE_MAXRETRY);
                                        
                                        SEXP_free (s_exp);
                                        return (NULL);
                                }
                        }
                }
                
                s_omsg = SEAP_msg_new ();
                SEAP_msg_set (s_omsg, s_exp);
                
                _D("Sending message...\n");
                
                if (SEAP_sendmsg (p_tbl->ctx, psd->sd, s_omsg) != 0) {
                        _D("Can't send message: %u, %s\n", errno, strerror (errno));
                        
                        if (SEAP_close (p_tbl->ctx, psd->sd) != 0) {
                                _D("Can't close sd: %u, %s\n", errno, strerror (errno));
                                
                                SEAP_msg_free (s_omsg);
                                SEXP_free (s_exp);
                                
                                return (NULL);
                        }
                                        
                        psd->sd = -1;
                        
                        if (++retry <= OVAL_PROBE_MAXRETRY) {
                                _D("send: retry %u/%u\n", retry, OVAL_PROBE_MAXRETRY);
                                continue;
                        } else {
                                _D("send: retry limit (%u) reached.\n", OVAL_PROBE_MAXRETRY);
                                SEAP_msg_free (s_omsg);
                                SEXP_free (s_exp);
                                                                
                                return (NULL);
                        }
                }
                
                _D("Waiting for reply...\n");
                
        recv_retry:
                if (SEAP_recvmsg (p_tbl->ctx, psd->sd, &s_imsg) != 0) {
                        _D("Can't receive message: %u, %s\n", errno, strerror (errno));
                        
                        switch (errno) {
                        case ECANCELED:
                        {
                                SEAP_err_t *err = NULL;
                                
                                if (SEAP_recverr_byid (p_tbl->ctx, psd->sd, &err, SEAP_msg_id (s_omsg)) != 0)
                                        goto recv_retry;
                                
                                /* 
                                 * decide what to do based on the error code/type
                                 */
                        } break;
                        }
                        // end
                        if (SEAP_close (p_tbl->ctx, psd->sd) != 0) {
                                _D("Can't close sd: %u, %s\n", errno, strerror (errno));
                                
                                SEAP_msg_free (s_imsg);
                                SEAP_msg_free (s_omsg);
                                SEXP_free (s_exp);
                                
                                return (NULL);
                        }
                        
                        psd->sd = -1;

                        if (++retry <= OVAL_PROBE_MAXRETRY) {
                                _D("recv: retry %u/%u\n", retry, OVAL_PROBE_MAXRETRY);
                                continue;
                        } else {
                                _D("recv: retry limit (%u) reached.\n", OVAL_PROBE_MAXRETRY);
                                
                                SEAP_msg_free (s_imsg);
                                SEAP_msg_free (s_omsg);
                                SEXP_free (s_exp);
                                
                                return (NULL);
                        }
                }
                
#if !defined(NDEBUG)
                fprintf (stderr,   "--- msg in ---\n");
                SEXP_fprintfa (stderr, r0 = SEAP_msg_get (s_imsg));
                SEXP_free (r0);
                fprintf (stderr, "\n--------------\n");
#endif
                
                _D("Message received.\n");
                break;
        }
        
        sysch = sexp_to_oval_state (r0 = SEAP_msg_get(s_imsg), object);
        SEXP_free (r0);
        
        SEAP_msg_free (s_omsg);
        SEAP_msg_free (s_imsg);
        SEXP_free (s_exp);
        
        return (sysch);
}

static SEXP_t *ovalp_cmd_obj_eval (SEXP_t *sexp, void *arg)
{
        char   *id_str;
        struct oval_object *obj;
        struct oval_definition_model *model = (struct oval_definition_model *)arg;

        if (SEXP_stringp (sexp)) {
                id_str = SEXP_string_cstr (sexp);
                obj    = oval_definition_model_get_object (model, id_str);
                
                _D("get_object: %s\n", id_str);
                
                if (obj == NULL) {
                        _D("FAIL: can't find obj: id=%s\n", id_str);
                        oscap_free (id_str);
                        return (NULL);
                }

                if (oval_object_probe (obj, model) == NULL) {
                        _D("FAIL: obj eval failed: id=%s\n", id_str);
                        oscap_free (id_str);
                        return (NULL);
                }

                oscap_free (id_str);

                return (sexp);
        } else {
                _D("FAIL: invalid argument: type=%s\n", SEXP_strtype (sexp));
                return (NULL);
        }
}

static SEXP_t *ovalp_cmd_ste_fetch (SEXP_t *sexp, void *arg)
{
        SEXP_t *id, *ste_list, *ste_sexp;
        char   *id_str;
        struct oval_state *ste;
        struct oval_definition_model *model = (struct oval_definition_model *)arg;

        ste_list = SEXP_list_new (NULL);

        SEXP_list_foreach (id, sexp) {
                if (SEXP_stringp (id)) {
                        id_str = SEXP_string_cstr (id);
                        ste    = oval_definition_model_get_state (model, id_str);

                        if (ste == NULL) {
                                _D("FAIL: can't find ste: id=%s\n", id_str);
                                SEXP_list_free (ste_list);
                                oscap_free (id_str);
                        }

                        ste_sexp = oval_state_to_sexp (ste);
                        SEXP_list_add (ste_list, ste_sexp);

                        oscap_free (id_str);
                }
        }

        return (ste_list);
}
#endif
