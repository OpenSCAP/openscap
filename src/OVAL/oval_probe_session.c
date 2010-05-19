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
#include <config.h>
#include <seap.h>
#include <errno.h>
#include <string.h>
#include "common/_error.h"
#include "common/assume.h"
#include "common/bfind.h"
#include "public/oval_definitions.h"
#include "_oval_probe_session.h"
#include "_oval_probe_handler.h"
#include "oval_probe_ext.h"
#include "oval_probe_impl.h"
#include "oval_sexp.h"

/*
 * oval_probe_session_
 */
oval_probe_session_t *oval_probe_session_new(struct oval_syschar_model *model)
{
        oval_probe_session_t *sess;

        sess = oscap_talloc(oval_probe_session_t);
        sess->ph   = oval_phtbl_new();
        sess->pext = oval_pext_new();
        sess->sys_model = model;
        sess->flg = 0;

        sess->pext->model    = &sess->sys_model;
        sess->pext->sess_ptr = sess;

        oval_probe_handler_set(sess->ph, OVAL_SUBTYPE_SYSINFO,       oval_probe_sys_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_INDEPENDENT_FAMILY,    oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_INDEPENDENT_FILE_MD5,  oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_INDEPENDENT_FILE_HASH, oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_INDEPENDENT_TEXT_FILE_CONTENT_54, oval_probe_ext_handler, sess->pext);       oval_probe_handler_set(sess->ph, OVAL_INDEPENDENT_TEXT_FILE_CONTENT, oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_INDEPENDENT_XML_FILE_CONTENT,  oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_LINUX_INET_LISTENING_SERVERS,  oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_LINUX_DPKG_INFO, oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_LINUX_RPM_INFO,  oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_UNIX_FILE,       oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_UNIX_INTERFACE,  oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_UNIX_PASSWORD,   oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_UNIX_PROCESS,    oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_UNIX_RUNLEVEL,   oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_UNIX_SHADOW,     oval_probe_ext_handler, sess->pext);
        oval_probe_handler_set(sess->ph, OVAL_UNIX_UNAME,      oval_probe_ext_handler, sess->pext);

        return(sess);
}


void oval_probe_session_destroy(oval_probe_session_t *sess)
{
        oval_phtbl_free(sess->ph);
        oval_pext_free(sess->pext);
        oscap_free(sess);
}

int oval_probe_session_close(oval_probe_session_t *sess)
{
        /* send close to all probes */
        return(-1);
}

int oval_probe_session_reset(oval_probe_session_t *sess)
{
        /* send reset to all probes */
        return(-1);
}

int oval_probe_session_sethandler(oval_probe_session_t *sess, oval_subtype_t *type, oval_probe_handler_t handler, void *ptr)
{
        return(-1);
}
