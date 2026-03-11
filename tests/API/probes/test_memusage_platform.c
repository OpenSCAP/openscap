// SPDX-License-Identifier: LGPL-2.1-or-later

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <errno.h>

#include "memusage.h"
#include "memusage.c"

int main(void)
{
	struct sys_memusage sys_mu = {0};
	struct proc_memusage proc_mu = {0};
	int ret_sys = oscap_sys_memusage(&sys_mu);
	int ret_proc = oscap_proc_memusage(&proc_mu);

#if defined(OS_LINUX) || defined(OS_FREEBSD) || defined(OS_APPLE)
	if (ret_sys != 0) {
		fprintf(stderr, "oscap_sys_memusage failed with errno=%d\n", errno);
		return 1;
	}
	if (ret_proc != 0) {
		fprintf(stderr, "oscap_proc_memusage failed with errno=%d\n", errno);
		return 1;
	}
#else
	if (ret_sys == 0 || ret_proc == 0) {
		fprintf(stderr, "memusage unexpectedly supported on this platform\n");
		return 1;
	}
#endif

	return 0;
}
