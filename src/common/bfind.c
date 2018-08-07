
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
 *       Lukas Kuklinek <lkuklinek@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stddef.h>
#ifdef OS_WINDOWS
#include <io.h>
#else
#include <unistd.h>
#endif
#include <stdint.h>
#include <assert.h>

#include "bfind.h"

void *oscap_bfind(void *ptr, size_t nmemb, size_t size, void *key, int cmpfn(void *, void *))
{
	size_t w, s;
	int cmp;
	uint8_t *p;

	w = nmemb;
	s = 0;
	p = (uint8_t *) ptr;

	while (w > 0) {
		cmp = cmpfn(key, p + (size * (s + w / 2)));
		if (cmp > 0) {
			s += w / 2 + 1;
			w = w - w / 2 - 1;
		} else if (cmp < 0) {
			w = w / 2;
		} else {
			return ((void *)(p + (size * (s + w / 2))));
		}
	}

	return (NULL);
}

int32_t oscap_bfind_i(void *ptr, size_t nmemb, size_t size, void *key, int cmpfn(void *, void *), size_t *save)
{
	size_t w, s;
	int cmp;
	uint8_t *p;

	w = nmemb;
	s = 0;
	p = (uint8_t *) ptr;

	while (w > 0) {
		cmp = cmpfn(key, p + (size * (s + w / 2)));
		if (cmp > 0) {
			s += w / 2 + 1;
			w = w - w / 2 - 1;
		} else if (cmp < 0) {
			w = w / 2;
		} else {
			assert((s + w / 2) <= INT32_MAX);
			return ((int32_t) (s + w / 2));
		}
	}

        if (save != NULL)
                *save = s;

	return INT32_C(-1);
}
