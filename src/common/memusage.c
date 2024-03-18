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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
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

#if defined(OS_FREEBSD)
#include <fcntl.h>
#include <kvm.h>
#include <limits.h>
#include <paths.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/user.h>
#include <unistd.h>

#define GET_VM_PAGE_SIZE        "vm.stats.vm.v_page_size"
#define GET_VM_TOTAL_PAGE_COUNT "vm.stats.vm.v_page_count"
#define GET_VM_FREE_PAGE_COUNT  "vm.stats.vm.v_free_count"
#define GET_VM_INACT_PAGE_COUNT "vm.stats.vm.v_inactive_count"
#define GET_VM_ACT_PAGE_COUNT   "vm.stats.vm.v_active_count"

#define BYTES_TO_KIB(x) (x >> 10)
#endif

#include "debug_priv.h"
#include "memusage.h"
#include "bfind.h"

#if defined(OS_LINUX) || defined(__FreeBSD__) || defined(OS_SOLARIS)
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

	errno = 0;
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

			if (sp == NULL) {
				/* drop end of unread line */
				while (strchr(strval, '\n') == NULL) {
					linebuf[0] = '\n';
					if (fgets(linebuf, sizeof linebuf - 1, fp) == NULL) {
						fclose(fp);
						return (-1);
					}
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

#if defined(OS_FREEBSD)
static int freebsd_sys_memusage(struct sys_memusage *mu)
{
	size_t size;
	size_t page_size = 0;
	unsigned int total_page_count = 0;
	unsigned int free_page_count = 0;
	unsigned int active_page_count = 0;
	unsigned int inactive_page_count = 0;

	/* Page size */
	size = sizeof(page_size);
	if (sysctlbyname(GET_VM_PAGE_SIZE, &page_size, &size, NULL, 0) < 0) {
		return -1;
	}

	/* Total pages */
	size = sizeof(total_page_count);
	if (sysctlbyname(GET_VM_TOTAL_PAGE_COUNT, &total_page_count, &size, NULL, 0) < 0) {
		return -1;
	}

	/* Total free pages */
	size = sizeof(free_page_count);
	if (sysctlbyname(GET_VM_FREE_PAGE_COUNT, &free_page_count, &size, NULL, 0) < 0) {
		return -1;
	}

	/* Total active pages */
	size = sizeof(active_page_count);
	if (sysctlbyname(GET_VM_ACT_PAGE_COUNT, &active_page_count , &size, NULL, 0) < 0) {
		return -1;
	}

	/* Total inactive pages */
	size = sizeof(inactive_page_count);
	if (sysctlbyname(GET_VM_INACT_PAGE_COUNT, &inactive_page_count , &size, NULL, 0) < 0) {
		return -1;
	}

	mu->mu_total = BYTES_TO_KIB(total_page_count * page_size);
	mu->mu_free = BYTES_TO_KIB(free_page_count * page_size);
	mu->mu_active = BYTES_TO_KIB(active_page_count * page_size);
	mu->mu_inactive = BYTES_TO_KIB(inactive_page_count * page_size);
	mu->mu_realfree = mu->mu_free + mu->mu_inactive;

	/* FreeBSD does not report these values */
	mu->mu_buffers = 0;
	mu->mu_cached = 0;

	return 0;
}

static int freebsd_proc_memusage(struct proc_memusage *mu)
{
	int count;
	kvm_t *kd;
	pid_t mypid;
	char errbuf[LINE_MAX];
	struct kinfo_proc *procinfo;

	mypid = getpid();
	kd = kvm_openfiles(NULL, _PATH_DEVNULL, NULL, O_RDONLY, errbuf);

	if (!kd)
		return -1;

	procinfo = kvm_getprocs(kd, KERN_PROC_PID, mypid, &count);

	if (!procinfo)
		return -1;

	mu->mu_rss = procinfo->ki_rssize;
	mu->mu_text = procinfo->ki_tsize;
	mu->mu_data = procinfo->ki_dsize;
	mu->mu_stack = procinfo->ki_ssize;

	/* ki_swrss is the resident set size before last swap, this
	 * is the closest approximation to Linux's "VmHWM" which is the
	 * peak resident set size of the process.
	 */
	mu->mu_hwm = procinfo->ki_swrss;

	/* Not exposed on FreeBSD */
	mu->mu_lib = 0;
	mu->mu_lock = 0;

	return 0;
}

#endif /* OS_FREEBSD */

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
#elif defined(OS_FREEBSD)
	if (freebsd_sys_memusage(mu))
		return -1;
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
#elif defined(OS_FREEBSD)
	if (freebsd_proc_memusage(mu))
		return -1;
#else
	errno = EOPNOTSUPP;
	return -1;
#endif
	return 0;
}
