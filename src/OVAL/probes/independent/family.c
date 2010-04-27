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
 *      "Tomas Heinrich" <theinric@redhat.com>
 *      "Daniel Kopecek" <dkopecek@redhat.com>
 */

/*
 * family probe:
 *
 *  family_object
 *
 *  family_item
 *    attrs
 *      id
 *      status_enum status
 *    [0..1] string family
 */

#include <config.h>
#include <seap.h>
#include <string.h>
#include <probe-api.h>

SEXP_t *probe_main(SEXP_t *probe_in, int *err, void *arg)
{
	SEXP_t *list, *item, *v_fm;

        (void)arg;
        
	const char *family =
#if defined(_WIN32)
        "windows";
#elif defined(Macintosh) || defined(macintosh) || (defined(__APPLE__) && defined(__MACH__))
        "macos";
#elif defined(__unix__) || defined(__unix)
        "unix";
#elif defined(CISCO_IOS) /* XXX: how to detect IOS? */
        "ios";
#else
        "error";
#endif
        
	if (probe_in == NULL) {
		*err = PROBE_EINVAL;
		return NULL;
	}

        item  = probe_item_creat ("family_item", NULL,
                                  /* entities */
                                  "family", NULL, v_fm = SEXP_string_new (family, strlen (family)),
                                  NULL);
        
	list = SEXP_list_new (item, NULL);
        
        SEXP_free (item);
        SEXP_free (v_fm);
        
	*err = 0;
	return (list);
}

