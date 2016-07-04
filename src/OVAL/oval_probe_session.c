/**
 * @file oval_probe_session.c
 * @brief OVAL probe session API implementation
 * @author "Daniel Kopecek" <dkopecek@redhat.com>
 *
 * @addtogroup PROBESESSION
 * @{
 */
/*
 * Copyright 2010 Red Hat Inc., Durham, North Carolina.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "common/_error.h"
#include "common/assume.h"
#include "common/bfind.h"
#include "common/debug_priv.h"


#include "public/oval_definitions.h"
#include "_oval_probe_session.h"
#include "_oval_probe_handler.h"
#include "oval_probe_impl.h"
#include "oval_probe_ext.h"
#include "oval_probe_meta.h"

#if defined(OSCAP_THREAD_SAFE)
#include <pthread.h>
static pthread_once_t __oval_probe_session_init_once = PTHREAD_ONCE_INIT;
#else
static volatile int __oval_probe_session_init_once = 0;
#endif /* OSCAP_THREAD_SAFE */

/**
 * Entity name cache exit hook. This hook is registered using
 * atexit(3) during initialization and ensures that the element
 * name cache is freed before exit.
 */
static void ncache_libfree(void)
{
        probe_ncache_free(OSCAP_GSYM(ncache));
}

/**
 * Initialize element name cache and register an exit hook.
 */
static void ncache_libinit(void)
{
        if (OSCAP_GSYM(ncache) == NULL) {
                OSCAP_GSYM(ncache) = probe_ncache_new();
                atexit(ncache_libfree);
        }
}

static void oval_probe_session_libinit(void)
{
	/* HACK: Make sure S-exps are initialized before we initialize anything else
	 * that uses them. This is needed only if the S-exp library is compiled without
	 * atomic builtins. In that case it uses a fallback locking mechanism that uses
	 * a mutex array which needs to be initialized before S-exp are used and freed
	 * at exit(3). The later is done by registering an atexit(3) function.
	 * The ncache_libinit function also registers an atexit(3) function and we need
	 * to make sure that the S-exps are not deinitialized before the ncache atexit
	 * handler is called.
	 * TODO: Implement SEXP_libinit() and call it from oscap_init()?
	 */
	volatile SEXP_t *exp = SEXP_string_new("magic", 5);
	SEXP_free((SEXP_t *)exp);

        ncache_libinit();
        oval_probe_tblinit();
}

/**
 * Initialize the element name cache. This function can be called repeatedly
 * from various probe system entry points to ensure that the cache is initialized.
 * If OSCAP_THREAD_SAFE is defined at compilation time, the pthread_once call
 * is used to ensure that the initialization is done just once. Otherwise a
 * volatile integer flag is used.
 */
static void __init_once(void)
{
#if defined(OSCAP_THREAD_SAFE)
        if (pthread_once(&__oval_probe_session_init_once,
			 &oval_probe_session_libinit) != 0)
                abort();
#else
        if (__oval_probe_session_init_once == 0) {
                oval_probe_session_libinit();
                __oval_probe_session_init_once = 1;
        }
#endif
        return;
}

static void oval_probe_session_init(oval_probe_session_t *sess, struct oval_syschar_model *model)
{
        void *handler_arg;
        register size_t i;

        sess->ph = oval_phtbl_new();
        sess->sys_model = model;
        sess->flg = 0;
        sess->pext = oval_pext_new();
        sess->pext->model    = &sess->sys_model;
        sess->pext->sess_ptr = sess;

        __init_once();

        dI("__probe_meta_count = %zu", OSCAP_GSYM(__probe_meta_count));

        for (i = 0; i < OSCAP_GSYM(__probe_meta_count); ++i) {
                handler_arg = NULL;

                if (OSCAP_GSYM(__probe_meta)[i].flags & OVAL_PROBEMETA_EXTERNAL)
                        handler_arg = sess->pext;

                oval_probe_handler_set(sess->ph,
				       OSCAP_GSYM(__probe_meta)[i].otype,
				       OSCAP_GSYM(__probe_meta)[i].handler, handler_arg);
        }

        oval_probe_handler_set(sess->ph, OVAL_SUBTYPE_ALL, oval_probe_ext_handler, sess->pext); /* special case for reset */
}

oval_probe_session_t *oval_probe_session_new(struct oval_syschar_model *model)
{
        oval_probe_session_t *sess = oscap_talloc(oval_probe_session_t);
        oval_probe_session_init(sess, model);
        return sess;
}

void oval_probe_session_free(oval_probe_session_t *sess)
{
	if (sess == NULL) {
		dE("Invalid session (NULL)");
		return;
	}

	oval_phtbl_free(sess->ph);
	oval_pext_free(sess->pext);
}

void oval_probe_session_reinit(oval_probe_session_t *sess, struct oval_syschar_model *model)
{
	oval_probe_session_free(sess);

	oval_probe_session_init(sess, model);
}

void oval_probe_session_destroy(oval_probe_session_t *sess)
{
	oval_probe_session_free(sess);
	oscap_free(sess);
}

int oval_probe_session_close(oval_probe_session_t *sess)
{
        /* send close to all probes */
        return(-1);
}

int oval_probe_session_reset(oval_probe_session_t *sess, struct oval_syschar_model *sysch)
{
        oval_ph_t *ph;

        if ((ph = oval_probe_handler_get(sess->ph, OVAL_SUBTYPE_ALL)) == NULL) {
		dE("No probe handler for OVAL_SUBTYPE_ALL");
		return (-1);
	}

        if (ph->func(OVAL_SUBTYPE_ALL, ph->uptr, PROBE_HANDLER_ACT_RESET) != 0) {
                return(-1);
        }
        if (sysch != NULL)
                sess->sys_model = sysch;

        return(0);
}

int oval_probe_session_abort(oval_probe_session_t *sess)
{
	oval_ph_t *ph;

	if ((ph = oval_probe_handler_get(sess->ph, OVAL_SUBTYPE_ALL) ) == NULL) {
		dE("No probe handler for OVAL_SUBTYPE_ALL");
		return (-1);
	}

        return ph->func(OVAL_SUBTYPE_ALL, ph->uptr, PROBE_HANDLER_ACT_ABORT);
}

int oval_probe_session_sethandler(oval_probe_session_t *sess, oval_subtype_t type, oval_probe_handler_t handler, void *ptr)
{
	dE("Operation not supported");
        return(-1);
}

struct oval_syschar_model *oval_probe_session_getmodel(oval_probe_session_t *sess)
{
	if (sess == NULL) {
		dE("Invalid session (NULL)");
		return (NULL);
	}

	return (sess->sys_model);
}

/// @}
