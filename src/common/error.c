
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

#include <pthread.h>
#include <string.h>

#include "alloc.h"
#include "util.h"
#include "_error.h"
#include "debug_priv.h"

static pthread_key_t __key;
static pthread_once_t __once = PTHREAD_ONCE_INIT;

static void oscap_errkey_init(void)
{
	(void)pthread_key_create(&__key, NULL);
}

static struct oscap_err_t *oscap_err_new(oscap_errfamily_t family, const char *desc,
					 const char *func, uint32_t line, const char *file)
{
	struct oscap_err_t *err;

	err = oscap_talloc(struct oscap_err_t);
	err->family = family;
	err->desc = oscap_sprintf("%s [%s:%d]", desc, file, line);
	err->func = func;
	err->line = line;
	err->file = file;

	dE("\(%s:%d:%s\()) %s\n", file, line, func, desc);

	return (err);
}

static void oscap_err_free(struct oscap_err_t *err)
{
	if (err->desc != NULL)
		oscap_free(err->desc);
	oscap_free(err);
}

void __oscap_setxmlerr(const char *file, uint32_t line, const char *func, xmlErrorPtr error)
{

	if (error == NULL)
		return;

	struct oscap_err_t *err;
	(void)pthread_once(&__once, oscap_errkey_init);

	oscap_clearerr();
	/* get rid of a newline that was pass by xmlGetLastError() */
	int len = strlen(error->message);
	if( error->message[len-1] == '\n' )
		error->message[len-1] = 0;
	err = oscap_err_new(OSCAP_EFAMILY_XML, error->message, func, line, file);
	(void)pthread_setspecific(__key, err);

	return;

}

void __oscap_seterr(const char *file, uint32_t line, const char *func, oscap_errfamily_t family, ...)
{
	struct oscap_err_t *err;
	char *msg;
	va_list ap;
	const char *fmt;

	(void)pthread_once(&__once, oscap_errkey_init);

	oscap_clearerr();

	va_start(ap, family);
	fmt = va_arg(ap, const char *);
	msg = oscap_vsprintf(fmt, ap);
	va_end(ap);

	err = oscap_err_new(family, msg, func, line, file);

	oscap_free(msg);
	(void)pthread_setspecific(__key, err);

	return;
}

void oscap_clearerr(void)
{
	struct oscap_err_t *err;

	(void)pthread_once(&__once, oscap_errkey_init);

	err = pthread_getspecific(__key);
	(void)pthread_setspecific(__key, NULL);

	if (err != NULL)
		oscap_err_free(err);

	return;
}

bool oscap_err(void)
{
	(void)pthread_once(&__once, oscap_errkey_init);
	return (pthread_getspecific(__key) != NULL);
}

oscap_errfamily_t oscap_err_family(void)
{
	struct oscap_err_t *err;

	(void)pthread_once(&__once, oscap_errkey_init);
	err = pthread_getspecific(__key);

	return (err == NULL ? 0 : err->family);
}

const char *oscap_err_desc(void)
{
	struct oscap_err_t *err;

	(void)pthread_once(&__once, oscap_errkey_init);
	err = pthread_getspecific(__key);

	return (err == NULL ? 0 : (const char *)err->desc);
}
