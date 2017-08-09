
/*
 * Copyright 2009-2010 Red Hat Inc., Durham, North Carolina.
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

#include <stdlib.h>
#include <string.h>

#include "alloc.h"
#include "_error.h"
#include "assume.h"

static void __oscap_err_check(void *m);

static void __oscap_err_check(void *m)
{

	if (m == NULL)
		oscap_seterr(OSCAP_EFAMILY_GLIBC, strerror(errno));
}

#if defined(NDEBUG)
/*
 * Normal
 */

void *__oscap_alloc(size_t s)
{
	void *m;
	m = malloc(s);
	__oscap_err_check(m);
#if defined(OSCAP_ALLOC_EXIT)
	if (m == NULL)
		exit(ENOMEM);
#endif
	return (m);
}

void *__oscap_calloc(size_t n, size_t s)
{
	void *m;
#if defined(OSCAP_ALLOC_STRICT)
	assume_d (n > 0, NULL);
	assume_d (s > 0, NULL);
#endif
	m = calloc(n, s);
	__oscap_err_check(m);
#if defined(OSCAP_ALLOC_EXIT)
	if (m == NULL)
		exit(ENOMEM);
#endif
	return (m);
}

void *__oscap_realloc(void *p, size_t s)
{
	void *m;

	m = realloc(p, s);
	__oscap_err_check(m);
#if defined(OSCAP_ALLOC_EXIT)
	if (m == NULL && s > 0)
		exit(ENOMEM);
#endif
	return (m);
}

void *__oscap_reallocf(void *p, size_t s)
{
	void *m;

	m = realloc(p, s);
	__oscap_err_check(m);
	if (m == NULL && s > 0) {
		free(p);
#if defined(OSCAP_ALLOC_EXIT)
		exit(ENOMEM);
#endif
	}
	return (m);
}

void __oscap_free(void *p)
{
#if defined(OSCAP_ALLOC_STRICT)
	assume_d (p != NULL, /* void */);
#endif
	if (p != NULL)
		free(p);
	return;
}

#endif
