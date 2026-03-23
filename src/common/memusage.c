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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdint.h>

#if defined(OS_FREEBSD)
#include <limits.h>
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
#endif /* OS_FREEBSD */

#if defined(OS_APPLE)
#include <mach/mach.h>
#include <sys/sysctl.h>
#define BYTES_TO_KIB(x) ((x) >> 10)
#endif /* OS_APPLE */

#include "debug_priv.h"
#include "memusage.h"
#include "bfind.h"

#if defined(OS_LINUX) || defined(__FreeBSD__) || defined(OS_SOLARIS) || defined(OSCAP_TEST_READ_COMMON_SIZET)
static int read_common_sizet(void *szp, const char *strval)
{
	char *end;
	long long value;

	if (szp == NULL || strval == NULL) {
		return -1;
	}

	errno = 0;
	value = strtoll(strval, &end, 10);

	if (end == strval || !isspace((unsigned char)*end) ||
	    errno == EINVAL || errno == ERANGE || value < 0)
		return -1;

	*(size_t *)szp = (size_t)value;
	return 0;
}

#endif

#if defined(OS_LINUX) || defined(__FreeBSD__) || defined(OS_SOLARIS)

struct stat_parser {
	char *keyword;
	int (*storval)(void *, const char *);
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
	pid_t mypid;
	struct kinfo_proc procinfo;
	size_t size;
	size_t page_size;
	int mib[4];

	mypid = getpid();
	size = sizeof(procinfo);
	memset(&procinfo, 0, sizeof(procinfo));
	mib[0] = CTL_KERN;
	mib[1] = KERN_PROC;
	mib[2] = KERN_PROC_PID;
	mib[3] = mypid;

	if (sysctl(mib, 4, &procinfo, &size, NULL, 0) < 0)
		return -1;

	if (size == 0) {
		errno = ESRCH;
		return -1;
	}

	page_size = (size_t)getpagesize();
	mu->mu_rss = BYTES_TO_KIB((uint64_t)procinfo.ki_rssize * page_size);
	mu->mu_text = BYTES_TO_KIB((uint64_t)procinfo.ki_tsize * page_size);
	mu->mu_data = BYTES_TO_KIB((uint64_t)procinfo.ki_dsize * page_size);
	mu->mu_stack = BYTES_TO_KIB((uint64_t)procinfo.ki_ssize * page_size);

	/* ki_swrss is the resident set size before last swap, this
	 * is the closest approximation to Linux's "VmHWM" which is the
	 * peak resident set size of the process.
	 */
	mu->mu_hwm = BYTES_TO_KIB((uint64_t)procinfo.ki_swrss * page_size);

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
#elif defined(OS_APPLE)
	{
		vm_statistics64_data_t vm_stat;
		mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
		vm_size_t page_size;
		host_page_size(mach_host_self(), &page_size);
		if (host_statistics64(mach_host_self(), HOST_VM_INFO64,
		                      (host_info64_t)&vm_stat, &count) != KERN_SUCCESS) {
			errno = EOPNOTSUPP;
			return -1;
		}
		mu->mu_free     = BYTES_TO_KIB((uint64_t)vm_stat.free_count     * page_size);
		mu->mu_active   = BYTES_TO_KIB((uint64_t)vm_stat.active_count   * page_size);
		mu->mu_inactive = BYTES_TO_KIB((uint64_t)vm_stat.inactive_count * page_size);
		mu->mu_buffers  = 0;
		mu->mu_cached   = 0;
		mu->mu_realfree = mu->mu_free + mu->mu_inactive;
		/* Query total physical RAM via sysctl HW_MEMSIZE */
		int mib[2] = { CTL_HW, HW_MEMSIZE };
		uint64_t memsize = 0;
		size_t len = sizeof(memsize);
		if (sysctl(mib, 2, &memsize, &len, NULL, 0) == 0)
			mu->mu_total = BYTES_TO_KIB(memsize);
		else
			mu->mu_total = 0;
	}
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
#elif defined(OS_APPLE)
	{
		struct task_basic_info info;
		mach_msg_type_number_t count = TASK_BASIC_INFO_COUNT;
		if (task_info(mach_task_self(), TASK_BASIC_INFO,
		              (task_info_t)&info, &count) != KERN_SUCCESS) {
			errno = EOPNOTSUPP;
			return -1;
		}
		mu->mu_rss   = info.resident_size / 1024;
		mu->mu_data  = info.virtual_size  / 1024;
		/* TASK_BASIC_INFO doesn't expose peak RSS; use current as approximation */
		mu->mu_hwm   = mu->mu_rss;
		mu->mu_text  = 0;
		mu->mu_stack = 0;
		mu->mu_lib   = 0;
		mu->mu_lock  = 0;
	}
#else
	errno = EOPNOTSUPP;
	return -1;
#endif
	return 0;
}
