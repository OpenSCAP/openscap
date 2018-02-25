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
#ifndef OVAL_PROBE_EXT_H
#define OVAL_PROBE_EXT_H

#include <seap.h>
#include <pthread.h>
#include <stdbool.h>
#include "oval_probe_impl.h"
#include "oval_system_characteristics_impl.h"
#include "common/util.h"

typedef struct {
	oval_subtype_t subtype;
	int sd;
	char *uri;
} oval_pd_t;

typedef struct {
	oval_pd_t **memb;
	size_t      count;
	SEAP_CTX_t *ctx;
} oval_pdtbl_t;

struct oval_pext {
        pthread_mutex_t lock;
        bool            do_init;

        SEAP_CTX_t   *sctx;
        oval_pdtbl_t *pdtbl;
        char         *probe_dir;

        void *sess_ptr;
        struct oval_syschar_model **model;
};

typedef struct oval_pext oval_pext_t;

oval_pext_t *oval_pext_new(void);
void oval_pext_free(oval_pext_t *pext);
int oval_probe_ext_init(oval_pext_t *pext);
int oval_probe_ext_eval(SEAP_CTX_t *ctx, oval_pd_t *pd, oval_pext_t *pext, struct oval_syschar *syschar, int flags);
int oval_probe_ext_reset(SEAP_CTX_t *ctx, oval_pd_t *pd, oval_pext_t *pext);
int oval_probe_ext_abort(SEAP_CTX_t *ctx, oval_pd_t *pd, oval_pext_t *pext);

int oval_probe_ext_handler(oval_subtype_t type, void *ptr, int act, ...);
int oval_probe_sys_handler(oval_subtype_t type, void *ptr, int act, ...);

#endif /* OVAL_PROBE_EXT_H */
