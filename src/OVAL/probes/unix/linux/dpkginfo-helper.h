/*
 * Copyright 2009 Red Hat Inc., Durham, North Carolina.
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
 *	"Pierre Chifflier <chifflier@edenwall.com>"
 */
#ifndef __DPKGINFO_HELPER__
#define __DPKGINFO_HELPER__

#ifdef __cplusplus
extern "C" {
#endif

struct dpkginfo_reply_t {
        char *name;
        char *arch;
        char *epoch;
        char *release;
        char *version;
        char *evr;
};

int dpkginfo_init();
int dpkginfo_fini();

struct dpkginfo_reply_t * dpkginfo_get_by_name(const char *name, int *err);

void * dpkginfo_free_reply(struct dpkginfo_reply_t *reply);

#ifdef __cplusplus
}
#endif

#endif /* __DPKGINFO_HELPER__ */
