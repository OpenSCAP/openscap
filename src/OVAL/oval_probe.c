#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/cdefs.h>
#include <seap.h>
#include <assert.h>
#include <errno.h>
#include <common/bfind.h>
#include "common/public/debug.h"
#if defined(OSCAP_THREAD_SAFE)
# include <pthread.h>
#endif

#include "oval_sexp.h"
#include "oval_probe_impl.h"
#include "oval_system_characteristics_impl.h"
#include "probes/public/probe-api.h"

/* KEEP THIS LIST SORTED! (by subtype) */
static const oval_pdsc_t __ovalp_ltable[] = {
        /*  7001 */ { OVAL_INDEPENDENT_FAMILY,               "family",            "probe_family"            },
        /*  7006 */ { OVAL_INDEPENDENT_TEXT_FILE_CONTENT_54, "textfilecontent54", "probe_textfilecontent54" },
        /*  7010 */ { OVAL_INDEPENDENT_XML_FILE_CONTENT,     "xmlfilecontent",    "probe_xmlfilecontent"    },
        /*  7999 */ { OVAL_INDEPENDENT_SYSCHAR_SUBTYPE,      "system_info",       "probe_system_info"       },
//      /*  9001 */ { OVAL_LINUX_DPKG_INFO,                  "dpkginfo",          "probe_dpkginfo"          },
        /*  9003 */ { OVAL_LINUX_RPM_INFO,                   "rpminfo",           "probe_rpminfo"           },
//      /*  9004 */ { OVAL_LINUX_SLACKWARE_PKG_INFO_TEST,    "slackwarepkginfo",  "probe_slackwarepkginfo"  },
        /* 13001 */ { OVAL_UNIX_FILE,                        "file",              "probe_file"              },
        /* 13006 */ { OVAL_UNIX_RUNLEVEL,                    "runlevel",          "probe_runlevel"          }
};

#define OVALP_LTBL_SIZE (sizeof __ovalp_ltable / sizeof (oval_pdsc_t))

static SEXP_t *oval_probe_cmd_obj_eval  (SEXP_t *sexp, void *arg);
static SEXP_t *oval_probe_cmd_ste_fetch (SEXP_t *sexp, void *arg);
static int     oval_probe_cmd_init (SEAP_CTX_t *ctx, oval_pctx_t *);

static oval_pdtbl_t *oval_pdtbl_new (void);
static void          oval_pdtbl_free (oval_pdtbl_t *);
static int           oval_pdtbl_add (oval_pdtbl_t *table, oval_subtype_t type, int sd, const char *uri);
static oval_pd_t    *oval_pdtbl_get (oval_pdtbl_t *table, oval_subtype_t type);
static int           oval_pd_cmp (const oval_pd_t *a, const oval_pd_t *b);
static int           oval_pdsc_subtype_cmp (oval_subtype_t *a, oval_pdsc_t *b);

static oval_pdtbl_t *oval_pdtbl_new (void)
{
        oval_pdtbl_t *p_tbl;
        
        p_tbl = oscap_talloc (oval_pdtbl_t);
        
        p_tbl->memb  = NULL;
        p_tbl->count = 0;
        p_tbl->ctx   = SEAP_CTX_new ();
                
        return (p_tbl);
}

static void oval_pdtbl_free (oval_pdtbl_t *tbl)
{
        return;
}

static int oval_probe_cmd_init (SEAP_CTX_t *ctx, oval_pctx_t *pctx)
{
        _A(ctx != NULL);
        
        if (SEAP_cmd_register (ctx, PROBECMD_OBJ_EVAL, SEAP_CMDREG_USEARG,
                               &oval_probe_cmd_obj_eval, (void *)pctx) != 0)
        {
                _D("FAIL: can't register command: %s: errno=%u, %s.\n",
                   "obj_eval", errno, strerror (errno));
                return (-1);
        }
        
        if (SEAP_cmd_register (ctx, PROBECMD_STE_FETCH, SEAP_CMDREG_USEARG,
                               &oval_probe_cmd_ste_fetch, (void *)pctx) != 0)
        {
                _D("FAIL: can't register command: %s: errno=%u, %s.\n",
                   "ste_fetch", errno, strerror (errno));
                
                /* FIXME: unregister the first command */
                
                return (-1);
        }
        
        return (0);
}

static int oval_pdsc_subtype_cmp (oval_subtype_t *a, oval_pdsc_t *b)
{
        return (*a - b->subtype);
}

const oval_pdsc_t *oval_pdsc_lookup (oval_subtype_t typenum)
{
        return (oscap_bfind ((void *)__ovalp_ltable, OVALP_LTBL_SIZE, sizeof __ovalp_ltable[0],
                             &typenum, (int (*) (void *, void *)) oval_pdsc_subtype_cmp));
}

oval_subtype_t oval_pdsc_lookup_type (const char *name)
{
        unsigned int i;
       
        _A(name != NULL);
        
        for (i = 0; i < OVALP_LTBL_SIZE; ++i)
                if (strcmp(__ovalp_ltable[i].subtype_name, name) == 0)
                        return __ovalp_ltable[i].subtype;
        
        return (0);
}

static oval_pd_t *oval_pdtbl_get (oval_pdtbl_t *tbl, oval_subtype_t subtype)
{
        _A(tbl != NULL);
        return oscap_bfind ((void *)(tbl->memb), tbl->count, sizeof (oval_pd_t),
                            &subtype, (int (*) (void *, void *)) oval_pdsc_subtype_cmp);
}

static int oval_pd_cmp (const oval_pd_t *a, const oval_pd_t *b)
{
        return (a->subtype - b->subtype);
}

static int oval_pdtbl_add (oval_pdtbl_t *tbl, oval_subtype_t type, int sd, const char *uri)
{
        _A(tbl != NULL);
        _A(uri != NULL);
        
        tbl->memb = oscap_realloc (tbl->memb, sizeof (oval_pd_t) * (++tbl->count));
        tbl->memb[tbl->count - 1].subtype = type;
        tbl->memb[tbl->count - 1].sd      = sd;
        tbl->memb[tbl->count - 1].uri     = strdup (uri);
        
        qsort (tbl->memb, tbl->count, sizeof (oval_pd_t), (int (*) (const void *, const void *))oval_pd_cmp);

        return (0);
}

static int oval_pdtbl_del (oval_pdtbl_t *tbl, oval_subtype_t type)
{
        _A(tbl != NULL);
        /* TODO */
        return (0);
}

oval_pctx_t *oval_pctx_new (struct oval_syschar_model *model)
{
        oval_pctx_t *ctx;

        ctx = oscap_talloc (oval_pctx_t);
        
        ctx->pdsc_table = (oval_pdsc_t *) __ovalp_ltable;

#if defined(OVAL_PROBEDIR_ENV)
        ctx->p_dir = getenv ("OVAL_PROBE_DIR");
#else
        ctx->p_dir = NULL;
#endif
        
        ctx->p_dir = ctx->p_dir == NULL ? strdup (OVAL_PROBE_DIR) : strdup (ctx->p_dir);
        ctx->pd_table = oval_pdtbl_new ();
        ctx->model    = model;
        
        if (model != NULL) {
                if (oval_probe_cmd_init (ctx->pd_table->ctx, ctx) != 0) {
                        _D("FAIL: Can't initialize SEAP commands\n");
                        oval_pctx_free (ctx);
                        
                        return (NULL);
                }
        }
        
        return (ctx);
}

int oval_pctx_setflag (oval_pctx_t *ctx, uint32_t flags)
{
        return (-1);
}

int oval_pctx_unsetflag (oval_pctx_t *ctx, uint32_t flags)
{
        return (-1);
}

int oval_pctx_setattr (oval_pctx_t *ctx, uint32_t attr, ...)
{
        va_list ap;
        
        _A(ctx != NULL);
        
        va_start (ap, attr);
        
        switch (attr) {
        case OVAL_PCTX_ATTR_DIR:
                if (ctx->p_dir != NULL)
                        oscap_free (ctx->p_dir);
                
                ctx->p_dir = strdup (va_arg (ap, const char *));
        }
        
        return (-1);
}

void oval_pctx_free (oval_pctx_t *ctx)
{
        oval_pdtbl_free (ctx->pd_table);
        oscap_free (ctx->p_dir);
        oscap_free (ctx);
        
        return;
}

static SEXP_t *oval_probe_comm (SEAP_CTX_t *ctx, oval_pd_t *pd, const SEXP_t *s_iobj)
{
        int retry;
        
        SEAP_msg_t *s_imsg, *s_omsg;
        SEXP_t     *s_oobj;
        
        _A(pd     != NULL);
        _A(s_iobj != NULL);
        
#if !defined(NDEBUG)
        fprintf (stderr,   "--- msg out ---\n");
        SEXP_fprintfa (stderr, s_iobj);
        fprintf (stderr, "\n---------------\n");
#endif
        
        oscap_dprintf ("%s: ctx=%p, pd=%p, s_iobj=%p\n", ctx, pd, s_iobj);
        
        for (retry = 0;;) {
                /*
                 * Establish connection to probe. The connection may be
                 * already set up by previous calls to this function or
                 * by the probe context handling functions.
                 */
                if (pd->sd == -1) {
                        pd->sd = SEAP_connect (ctx, pd->uri, 0);
                        
                        if (pd->sd < 0) {
                                _D("Can't connect: %u, %s.\n", errno, strerror (errno));
                                
                                if (++retry <= OVAL_PROBE_MAXRETRY) {
                                        _D("connect: retry %u/%u\n", retry, OVAL_PROBE_MAXRETRY);
                                        continue;
                                } else {
                                        _D("connect: retry limit (%u) reached.\n", OVAL_PROBE_MAXRETRY);
                                        return (NULL);
                                }
                        }
                }
                
                s_omsg = SEAP_msg_new ();
                SEAP_msg_set (s_omsg, (SEXP_t *)s_iobj);
                
                _D("Sending message...\n");
                
                if (SEAP_sendmsg (ctx, pd->sd, s_omsg) != 0) {
                        _D("Can't send message: %u, %s\n", errno, strerror (errno));
                        
                        if (SEAP_close (ctx, pd->sd) != 0) {
                                _D("Can't close sd: %u, %s\n", errno, strerror (errno));
                                SEAP_msg_free (s_omsg);
                                
                                return (NULL);
                        }
                        
                        pd->sd = -1;
                        
                        if (++retry <= OVAL_PROBE_MAXRETRY) {
                                _D("send: retry %u/%u\n", retry, OVAL_PROBE_MAXRETRY);
                                continue;
                        } else {
                                _D("send: retry limit (%u) reached.\n", OVAL_PROBE_MAXRETRY);
                                SEAP_msg_free (s_omsg);
                                                                                                
                                return (NULL);
                        }
                }
                
                _D("Waiting for reply...\n");
                
        recv_retry:
                if (SEAP_recvmsg (ctx, pd->sd, &s_imsg) != 0) {
                        _D("Can't receive message: %u, %s\n", errno, strerror (errno));
                        
                        switch (errno) {
                        case ECANCELED:
                        {
                                SEAP_err_t *err = NULL;
                                
                                if (SEAP_recverr_byid (ctx, pd->sd, &err, SEAP_msg_id (s_omsg)) != 0)
                                        goto recv_retry;
                                
                                /* 
                                 * decide what to do based on the error code/type
                                 */
                        } break;
                        }
                        
                        if (SEAP_close (ctx, pd->sd) != 0) {
                                _D("Can't close sd: %u, %s\n", errno, strerror (errno));
                                SEAP_msg_free (s_imsg);
                                SEAP_msg_free (s_omsg);
                                
                                return (NULL);
                        }
                        
                        pd->sd = -1;

                        if (++retry <= OVAL_PROBE_MAXRETRY) {
                                _D("recv: retry %u/%u\n", retry, OVAL_PROBE_MAXRETRY);
                                continue;
                        } else {
                                _D("recv: retry limit (%u) reached.\n", OVAL_PROBE_MAXRETRY);
                                
                                SEAP_msg_free (s_imsg);
                                SEAP_msg_free (s_omsg);
                                
                                return (NULL);
                        }
                }
                
                _D("Message received.\n");
                break;
        }
        
        s_oobj = SEAP_msg_get (s_imsg);
        
        SEAP_msg_free (s_imsg);
        SEAP_msg_free (s_omsg);
        
#if !defined(NDEBUG)
        fprintf (stderr,   "--- msg in ---\n");
        SEXP_fprintfa (stderr, s_oobj);
        fprintf (stderr, "\n--------------\n");
#endif        
        return (s_oobj);
}

struct oval_syschar *oval_probe_object_eval (oval_pctx_t *ctx, struct oval_object *object)
{
        const oval_pdsc_t *pdsc;        
        oval_pd_t    *pd;
        
        SEXP_t *s_obj, *s_sysc;
        struct oval_syschar *o_sysc;
                
        _A(ctx    != NULL);
        _A(object != NULL);
        _A(ctx->pdsc_table != NULL);
        
        /* XXX: lock ctx? */
        
        if (ctx->model == NULL) {
                errno = EINVAL;
                return (NULL);
        }
        
        pdsc = oval_pdsc_lookup (oval_object_get_subtype(object));
        
        if (pdsc == NULL) {
                errno = EOPNOTSUPP;
                return (NULL);
        }
                
        pd = oval_pdtbl_get (ctx->pd_table, oval_object_get_subtype (object));
        
        if (pd == NULL) {
                char   probe_uri[PATH_MAX+1];
                size_t probe_urilen;
                char  *probe_dir;
                
                probe_dir    = ctx->p_dir;
                probe_urilen = snprintf (probe_uri, sizeof probe_uri,
                                         "%s://%s/%s",
                                         OVAL_PROBE_SCHEME, probe_dir, pdsc->filename);
                
                _D("URI: %s\n", probe_uri);
                
                if (oval_pdtbl_add (ctx->pd_table, oval_object_get_subtype (object), -1, probe_uri) != 0) {
                        return (NULL);
                } else
                        pd = oval_pdtbl_get (ctx->pd_table, oval_object_get_subtype (object));
        }
        
        s_obj = oval_object2sexp (pdsc->subtype_name, object, ctx->model);
        
        if (s_obj == NULL) {
                _D("Can't translate OVAL object to S-exp\n");
                return (NULL);
        }
                
        s_sysc = oval_probe_comm (ctx->pd_table->ctx, pd, s_obj);
        
        if (s_sysc == NULL) {
                SEXP_free (s_obj);
                return (NULL);
        }
        
        /*
         * Convert the received S-exp to OVAL system characteristic.
         */
        
        o_sysc = oval_sexp2sysch (s_sysc, object);
        
        SEXP_free (s_sysc);
        SEXP_free (s_obj);
        
        return (o_sysc);
}

static SEXP_t *oval_probe_cmd_obj_eval (SEXP_t *sexp, void *arg)
{
        char   *id_str;
        struct oval_object *obj;
	struct oval_definition_model *definition_model;
        oval_pctx_t *ctx = (oval_pctx_t *)arg;

        if (SEXP_stringp (sexp)) {
                id_str = SEXP_string_cstr (sexp);
		definition_model = oval_syschar_model_get_definition_model(ctx->model);
                obj    = oval_definition_model_get_object (definition_model, id_str);
                
                _D("get_object: %s\n", id_str);
                
                if (obj == NULL) {
                        _D("FAIL: can't find obj: id=%s\n", id_str);
                        oscap_free (id_str);
                        return (NULL);
                }
                
                if (oval_probe_object_eval (ctx, obj) == NULL) {
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

static SEXP_t *oval_probe_cmd_ste_fetch (SEXP_t *sexp, void *arg)
{
        SEXP_t *id, *ste_list, *ste_sexp;
        char   *id_str;
        struct oval_state *ste;
	struct oval_definition_model *definition_model;
        oval_pctx_t *ctx = (oval_pctx_t *)arg;
        
        ste_list = SEXP_list_new (NULL);

        SEXP_list_foreach (id, sexp) {
                if (SEXP_stringp (id)) {
                        id_str = SEXP_string_cstr (id);
			definition_model = oval_syschar_model_get_definition_model(ctx->model);
                        ste    = oval_definition_model_get_state (definition_model, id_str);

                        if (ste == NULL) {
                                _D("FAIL: can't find ste: id=%s\n", id_str);
                                SEXP_list_free (ste_list);
                                oscap_free (id_str);
                        }
                        
                        ste_sexp = oval_state2sexp (ste);
                        SEXP_list_add (ste_list, ste_sexp);
                        
                        oscap_free (id_str);
                }
        }

        return (ste_list);
}

struct oval_sysinfo *oval_probe_sysinf_eval (oval_pctx_t *ctx)
{
        struct oval_sysinfo *sysinf;
        struct oval_sysint  *ife;
        oval_pd_t *pd;
        
        SEXP_t *s_obj, *s_sinf, *ent;
        
        pd = oval_pdtbl_get (ctx->pd_table, OVAL_INDEPENDENT_SYSCHAR_SUBTYPE);
        
        if (pd == NULL) {
                char   probe_uri[PATH_MAX+1];
                size_t probe_urilen;
                char  *probe_dir;
                
                probe_dir    = ctx->p_dir;
                probe_urilen = snprintf (probe_uri, sizeof probe_uri,
                                         "%s://%s/probe_system_info",
                                         OVAL_PROBE_SCHEME, probe_dir);
                
                _D("URI: %s\n", probe_uri);
                
                if (oval_pdtbl_add (ctx->pd_table, OVAL_INDEPENDENT_SYSCHAR_SUBTYPE, -1, probe_uri) != 0) {
                        return (NULL);
                } else
                        pd = oval_pdtbl_get (ctx->pd_table, OVAL_INDEPENDENT_SYSCHAR_SUBTYPE);
        }
        
        /*
         * Prepare a dummy object. We can't simply send an empty object
         * because the preprocessing machinery in probes need an id that
         * is used as key in cache lookups.
         */
        {
                SEXP_t *attrs, *r0;

                s_obj = probe_obj_creat ("sysinfo_object",
                                         attrs = probe_attr_creat ("id", r0 = SEXP_string_newf ("sysinfo:0"),
                                                                   NULL),
                                         NULL);
                SEXP_vfree (attrs, r0, NULL);
        }
        
        s_sinf = oval_probe_comm (ctx->pd_table->ctx, pd, s_obj);
        SEXP_free (s_obj);
        
        if (s_sinf == NULL)
                return (NULL);
        
        sysinf = oval_sysinfo_new ();
        
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
                oval_sysinfo_free (sysinf);
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
                
                for (n = 1; (ent = probe_obj_getent (s_sinf, "interface", n)) != NULL; ++n) {
                        ife = oval_sysint_new ();
                        
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
                                                                        \
                        } while (0)
                        
                        SYSINF_IEXT(ent, ip_address, ife, fail_int);
                        SYSINF_IEXT(ent, mac_address, ife, fail_int);
                        SYSINF_IEXT(ent, name, ife, fail_int);
                        
                        oval_sysinfo_add_interface (sysinf, ife);
                        oval_sysint_free (ife);
                        SEXP_free (ent);
                }
        }
        
        SEXP_free (s_sinf);
        
        return (sysinf);
fail_int:
        SEXP_free (ent);
        oval_sysint_free (ife);
fail_gen:
        SEXP_free (s_sinf);
        oval_sysinfo_free (sysinf);
        
        return (NULL);
}
