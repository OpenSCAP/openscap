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
 *      Daniel Kopecek <dkopecek@redhat.com>
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdint.h>

#include "debug_priv.h"
#include "memusage.h"
#include "bfind.h"

#if defined(OS_LINUX)
static int read_common_sizet(void *szp, char *strval)
{
	char *end;

	if (szp == NULL || strval == NULL) {
		return -1;
	}

	end = strchr(strval, ' ');

	if (end == NULL)
		return (-1);

	*end = '\0';
	*(size_t *)szp = strtoll(strval, NULL, 10);

	if (errno == EINVAL ||
	    errno == ERANGE)
		return (-1);

	return (0);
}

struct stat_parser {
	char *keyword;
	int (*storval)(void *, char *);
	ptrdiff_t offset;
};

static int cmpkey (const char *a, const struct stat_parser *b)
{
	return strcmp(a, b->keyword);
}

static int read_status(const char *source, void *base, struct stat_parser *spt, size_t spt_size)
{
	FILE *fp;
	size_t processed;

#ifndef NDEBUG
	/* check whether spt is sorted */
	{
		register size_t i;

		for (i = 0; i < spt_size - 1; ++i) {
			if (cmpkey(spt[i].keyword, spt + i+1) > 0) {
				dE("spt not sorted! %s > %s",
				   spt[i].keyword, spt[i+1].keyword);
				abort();
			}
		}
	}
#endif
	fp = fopen(source, "r");

	if (fp == NULL)
		return -1;
	else {
		char linebuf[256];
		char *strval;
		struct stat_parser *sp;

		processed = 0;

		while (fgets(linebuf, sizeof linebuf - 1, fp) != NULL) {
			strval = strchr(linebuf, ':');

			if (strval == NULL) {
				fclose (fp);
				return (-1);
			}

			*strval++ = '\0';

			while(isspace(*strval))
				++strval;

			sp = oscap_bfind(spt, spt_size, sizeof(struct stat_parser),
			                 linebuf, (int(*)(void *, void *))&cmpkey);

			dI("spt: %s", linebuf);

			if (sp == NULL) {
				/* drop end of unread line */
				while (strchr(strval, '\n') == NULL) {
					linebuf[0] = '\n';
					fgets(linebuf, sizeof linebuf - 1, fp);
					strval = linebuf;
				}
				continue;
			}

			/* line is too long, somthing is wrong */
			if (strchr(strval, '\n') == NULL) {
				dE("value for %s key is too long", linebuf);
				fclose(fp);
				return (-1);
			}

			if (sp->storval((void *)((uintptr_t)(base) + sp->offset), strval) != 0) {
				fclose (fp);
				return (-1);
			}

			++processed;
		}

		fclose(fp);
	}

	return processed == spt_size ? 0 : 1;
}

#define stat_sizet_field(name, stype, sfield)                           \
	{ (name), &read_common_sizet, (ptrdiff_t)offsetof(stype, sfield) }

struct stat_parser __sys_stat_ptable[] = {
	stat_sizet_field("Active",    struct sys_memusage, mu_active),
	stat_sizet_field("Buffers",   struct sys_memusage, mu_buffers),
	stat_sizet_field("Cached",    struct sys_memusage, mu_cached),
	stat_sizet_field("Inactive",  struct sys_memusage, mu_inactive),
	stat_sizet_field("MemFree",   struct sys_memusage, mu_free),
	stat_sizet_field("MemTotal",  struct sys_memusage, mu_total)
};

struct stat_parser __proc_stat_ptable[] = {
	stat_sizet_field("VmData", struct proc_memusage, mu_data),
	stat_sizet_field("VmExe",  struct proc_memusage, mu_text),
	stat_sizet_field("VmHWM",  struct proc_memusage, mu_hwm),
	stat_sizet_field("VmLck",  struct proc_memusage, mu_lock),
	stat_sizet_field("VmLib",  struct proc_memusage, mu_lib),
	stat_sizet_field("VmRSS",  struct proc_memusage, mu_rss),
	stat_sizet_field("VmStk",  struct proc_memusage, mu_stack),
};

#endif /* OS_LINUX */

int oscap_sys_memusage(struct sys_memusage *mu)
{
	if (mu == NULL)
		return -1;
#if defined(OS_LINUX)
	if (read_status(MEMUSAGE_LINUX_SYS_STATUS,
	                mu, __sys_stat_ptable,
	                (sizeof __sys_stat_ptable)/sizeof(struct stat_parser)) != 0)
	{
		return -1;
	}

	mu->mu_realfree = mu->mu_free + mu->mu_cached + mu->mu_buffers;
#else
	errno = EOPNOTSUPP;
	return -1;
#endif
	return 0;
}

int oscap_proc_memusage(struct proc_memusage *mu)
{
	if (mu == NULL)
		return -1;
#if defined(OS_LINUX)
	if (read_status(MEMUSAGE_LINUX_PROC_STATUS,
	                mu,  __proc_stat_ptable,
	                (sizeof __proc_stat_ptable)/sizeof(struct stat_parser)) != 0)
	{
		return -1;
	}
#else
	errno = EOPNOTSUPP;
	return -1;
#endif
	return 0;
}
