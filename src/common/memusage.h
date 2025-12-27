#ifndef MEMUSAGE_H
#define MEMUSAGE_H

#include "oscap_platforms.h"

#if defined(OS_LINUX)
# define MEMUSAGE_LINUX_PROC_STATUS "/proc/self/status"
# define MEMUSAGE_LINUX_PROC_ENV    "MEMUSAGE_PROC_STATUS"
# define MEMUSAGE_LINUX_SYS_STATUS "/proc/meminfo"
# define MEMUSAGE_LINUX_SYS_ENV "MEMUSAGE_SYS_STATUS"
# define MEMUSAGE_LINUX_SYS_CGROUP_USAGE "/sys/fs/cgroup/memory/memory.usage_in_bytes"
# define MEMUSAGE_LINUX_SYS_CGROUP_LIMIT "/sys/fs/cgroup/memory/memory.limit_in_bytes"
# define MEMUSAGE_LINUX_SYS_CGROUP2_CURRENT "/sys/fs/cgroup/memory.current"
# define MEMUSAGE_LINUX_SYS_CGROUP2_MAX "/sys/fs/cgroup/memory.max"

#endif /* OS_LINUX */

struct proc_memusage {
	size_t mu_rss;
	size_t mu_hwm;
	size_t mu_lib;
	size_t mu_text;
	size_t mu_data;
	size_t mu_stack;
	size_t mu_lock;
};

struct sys_memusage {
	size_t mu_total;
	size_t mu_free;
	size_t mu_realfree;
	size_t mu_buffers;
	size_t mu_cached;
	size_t mu_active;
	size_t mu_inactive;
};

int oscap_proc_memusage(struct proc_memusage *mu);
int oscap_sys_memusage(struct sys_memusage *mu);

#endif /* MEMUSAGE_H */
