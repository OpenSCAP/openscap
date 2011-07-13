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
#include "public/oval_definitions.h"
#include "_oval_probe_session.h"
#include "_oval_probe_handler.h"
#include "oval_probe_impl.h"
#if defined(ENABLE_PROBES)
# include "oval_probe_ext.h"
# include "oval_probe_int.h"

#if defined(OSCAP_THREAD_SAFE)
# include <pthread.h>
static pthread_once_t __encache_once = PTHREAD_ONCE_INIT;
#else
static volatile int __encache_once = 0;
#endif

/**
 * Entity name cache exit hook. This hook is registered using
 * atexit(3) during initialization and ensures that the element
 * name cache is freed before exit.
 */
static void encache_libfree(void)
{
        probe_ncache_free(OSCAP_GSYM(encache));
}

/**
 * Initialize element name cache and register an exit hook.
 */
static void encache_libinit(void)
{
        if (OSCAP_GSYM(encache) == NULL) {
                OSCAP_GSYM(encache) = probe_ncache_new();
                atexit(encache_libfree);
        }
}

/**
 * Initialize the element name cache. This function can be called repeatedly
 * from various probe system entry points to ensure that the cache is initialized.
 * If OSCAP_THREAD_SAFE is defined at compilation time, the pthread_once call
 * is used to ensure that the initialization is done just once. Otherwise a
 * volatile integer flag is used.
 */
static void encache_once(void)
{
#if defined(OSCAP_THREAD_SAFE)
        if (pthread_once(&__encache_once, &encache_libinit) != 0)
                abort();
#else
        if (__encache_once == 0) {
                encache_libinit();
                __encache_once = 1;
        }
#endif
        return;
}
#endif /* ENABLE_PROBES */

oval_probe_session_t *oval_probe_session_new(struct oval_syschar_model *model)
{
        oval_probe_session_t *sess;

        sess = oscap_talloc(oval_probe_session_t);
        sess->ph = oval_phtbl_new();
        sess->sys_model = model;
        sess->flg = 0;

#if defined(ENABLE_PROBES)
        sess->pext = oval_pext_new();
        sess->pext->model    = &sess->sys_model;
        sess->pext->sess_ptr = sess;

        encache_once();

        oval_probe_handler_set(sess->ph, OVAL_SUBTYPE_SYSINFO,       oval_probe_sys_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_INDEPENDENT_FAMILY,    oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_INDEPENDENT_FILE_MD5,  oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_INDEPENDENT_FILE_HASH, oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_INDEPENDENT_TEXT_FILE_CONTENT_54, oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_INDEPENDENT_TEXT_FILE_CONTENT, oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_INDEPENDENT_XML_FILE_CONTENT,  oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_INDEPENDENT_LDAP57,  oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_INDEPENDENT_SQL,  oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_INDEPENDENT_SQL57,  oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_LINUX_INET_LISTENING_SERVERS,  oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_LINUX_DPKG_INFO, oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_LINUX_RPM_INFO,  oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_LINUX_PARTITION, oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_SOLARIS_ISAINFO, oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_UNIX_FILE,       oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_UNIX_INTERFACE,  oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_UNIX_PASSWORD,   oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_UNIX_PROCESS,    oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_UNIX_RUNLEVEL,   oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_UNIX_SHADOW,     oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_UNIX_UNAME,      oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_UNIX_DNSCACHE,   oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_UNIX_SYSCTL,     oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_SUBTYPE_ALL,     oval_probe_ext_handler, sess->pext); /* special case for reset */

        oval_probe_handler_set(sess->ph, OVAL_INDEPENDENT_ENVIRONMENT_VARIABLE,  oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_INDEPENDENT_ENVIRONMENT_VARIABLE58,  oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_INDEPENDENT_VARIABLE,              oval_probe_var_handler,    sess);
#endif
        return(sess);
}

void oval_probe_session_destroy(oval_probe_session_t *sess)
{
        oval_phtbl_free(sess->ph);
#if defined(ENABLE_PROBES)
        oval_pext_free(sess->pext);
#endif
        oscap_free(sess);
}

int oval_probe_session_close(oval_probe_session_t *sess)
{
        /* send close to all probes */
        return(-1);
}

int oval_probe_session_reset(oval_probe_session_t *sess, struct oval_syschar_model *sysch)
{
#if defined(ENABLE_PROBES)
        oval_ph_t *ph;

        ph = oval_probe_handler_get(sess->ph, OVAL_SUBTYPE_ALL);

        if (ph->func(OVAL_SUBTYPE_ALL, ph->uptr, PROBE_HANDLER_ACT_RESET) != 0) {
                return(-1);
        }
#endif
        if (sysch != NULL)
                sess->sys_model = sysch;

        return(0);
}

int oval_probe_session_abort(oval_probe_session_t *sess)
{
#if defined(ENABLE_PROBES)
	oval_ph_t *ph = oval_probe_handler_get(sess->ph, OVAL_SUBTYPE_ALL);
        return ph->func(OVAL_SUBTYPE_ALL, ph->uptr, PROBE_HANDLER_ACT_ABORT);
#else
	return (-1);
#endif
}

int oval_probe_session_sethandler(oval_probe_session_t *sess, oval_subtype_t type, oval_probe_handler_t handler, void *ptr)
{
        return(-1);
}

struct oval_syschar_model *oval_probe_session_getmodel(oval_probe_session_t *sess)
{
        return (sess->sys_model);
}
