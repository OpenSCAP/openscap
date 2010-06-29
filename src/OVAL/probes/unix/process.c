/**
 * @file   process.c
 * @brief  process probe
 * @author "Steve Grubb" <sgrubb@redhat.com>
 *
 * 2010/06/13 dkopecek@redhat.com
 *  This probe is able to process an process_object as defined in OVAL 5.4 and 5.5.
 *
 */

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
 *   Steve Grubb <sgrubb@redhat.com>
 */

/*
 * process probe:
 *
 * command
 * exec_time
 * pid
 * ppid
 * priority
 * scheduling_class
 * start_time
 * tty
 * user_id
 */

#include "config.h"
#include "seap.h"
#include "probe-api.h"
#include "probe-entcmp.h"
#include "alloc.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#ifdef HAVE_STDIO_EXT_H
# include <stdio_ext.h>
#endif
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sched.h>
#include <time.h>

#if defined(__linux)
/* Convenience structure for the results being reported */
struct result_info {
        const char *command;
        const char *exec_time;
        unsigned pid;
        unsigned ppid;
        long priority;
        const char *scheduling_class;
        const char *start_time;
        int tty;
        unsigned user_id;
};

unsigned long ticks, boot;

static void report_finding(struct result_info *res, SEXP_t *probe_out)
{
	SEXP_t *r0, *r1, *r2, *r3, *r4, *r5, *r6, *r7, *r8, *item_sexp;

	item_sexp = probe_item_creat("process_item", NULL,
		/* entities */
		"command", NULL, r0 = SEXP_string_newf("%s", res->command),
		"exec_time", NULL, r1 = SEXP_string_newf("%s", res->exec_time),
		"pid", NULL, r2 = SEXP_string_newf("%u", res->pid),
		"ppid", NULL, r3 = SEXP_string_newf("%u", res->ppid),
		"priority", NULL, r4 = SEXP_string_newf("%ld", res->priority),
		"scheduling_class", NULL, r5 = SEXP_string_newf("%s", res->scheduling_class),
		"start_time", NULL, r6 = SEXP_string_newf("%s",res->start_time),
		"tty", NULL, r7 = SEXP_string_newf("%d", res->tty),
		"user_id", NULL, r8 = SEXP_string_newf("%u", res->user_id),
		NULL);
	SEXP_vfree(r0, r1, r2, r3, r4, r5, r6, r7, r8, NULL);
	SEXP_list_add(probe_out, item_sexp);
	SEXP_free(item_sexp);
}

static void get_boot_time(void)
{
	char buf[100];
	FILE *sf;
	int line;

	boot = 0;
	sf = fopen("/proc/stat", "rt");
	if (sf == NULL)
		return;

	line = 0;
	__fsetlocking(sf, FSETLOCKING_BYCALLER);
	while (fgets(buf, sizeof(buf), sf)) {
		if (line == 0) {
			line++;
			continue;
		}
		if (memcmp(buf, "btime", 5) == 0) {
			sscanf(buf, "btime %lu", &boot);
			break;
		}
	}
	fclose(sf);
}

static unsigned get_effective_id(int pid)
{
	char buf[100];
	unsigned euid;
	FILE *sf;

	snprintf(buf, sizeof(buf), "/proc/%d/status", pid);
	sf = fopen(buf, "rt");
	if (sf == NULL)
		euid = 0;
	else {
		int line = 0;
		__fsetlocking(sf, FSETLOCKING_BYCALLER);
		while (fgets(buf, sizeof(buf), sf)) {
			if (line == 0) {
				line++;
				continue;
			}
			if (memcmp(buf, "Uid:", 4) == 0) {
				int id;
				sscanf(buf, "Uid: %d %u", &id, &euid);
				break;
			}
		}
		fclose(sf);
	}
	return euid;
}

static char *convert_time(unsigned long long t, char *tbuf, int tb_size)
{
	unsigned d,h,m,s;

	s = t%60;
	t /= 60;
	m = t%60;
	t /=60;
	h = t%24;
	t /= 24;
	d = t;
	if (d)
		snprintf(tbuf, tb_size, "%u-%02u:%02u:%02u", d, h, m, s);
	else
		snprintf(tbuf, tb_size,	"%02u:%02u:%02u", h, m, s);
	return tbuf;
}

static int read_process(SEXP_t *cmd_ent, SEXP_t *probe_out)
{
	int err = 1;
	DIR *d;
	struct dirent *ent;

	d = opendir("/proc");
	if (d == NULL)
		return err;

	// Get the time tick hertz
	ticks = (unsigned long)sysconf(_SC_CLK_TCK);
	get_boot_time();

	// Scan the directories
	while (( ent = readdir(d) )) {
		int fd, len;
		char buf[256];
		char *tmp, cmd[16], state;
		int pid, ppid, pgrp, session, tty_nr, tpgid;
		unsigned flags, sched_policy;
		unsigned long minflt, cminflt, majflt, cmajflt, uutime, ustime;
		long cutime, cstime, priority, cnice, nthreads, itrealvalue;
		unsigned long long start;
		SEXP_t *cmd_sexp;

		// Skip non-process dir entries
		if(*ent->d_name<'0' || *ent->d_name>'9')
			continue;
		errno = 0;
		pid = strtol(ent->d_name, NULL, 10);
		if (errno || pid == 2) // skip err & kthreads
			continue;

		// Parse up the stat file for the proc
		snprintf(buf, 32, "/proc/%d/stat", pid);
		fd = open(buf, O_RDONLY, 0);
		if (fd < 0)
			continue;
		len = read(fd, buf, sizeof buf - 1);
		close(fd);
		if (len < 40)
			continue;
		buf[len] = 0;
		tmp = strrchr(buf, ')');
		if (tmp)
			*tmp = 0;
		else
			continue;
		memset(cmd, 0, sizeof(cmd));
		sscanf(buf, "%d (%15c", &ppid, cmd);
		sscanf(tmp+2,	"%c %d %d %d %d %d "
				"%u %lu %lu %lu %lu "
				"%lu %lu %lu %ld %ld "
				"%ld %ld %ld %llu",
			&state, &ppid, &pgrp, &session, &tty_nr, &tpgid,
			&flags, &minflt, &cminflt, &majflt, &cmajflt,
			&uutime, &ustime, &cutime, &cstime, &priority,
			&cnice, &nthreads, &itrealvalue, &start
		);

		// Skip kthreads
		if (ppid == 2)
			continue;

		err = 0; // If we get this far, no permission problems
		_D("Have command: %s\n", cmd);
		cmd_sexp = SEXP_string_newf("%s", cmd);
		if (probe_entobj_cmp(cmd_ent, cmd_sexp) == OVAL_RESULT_TRUE) {
			struct result_info r;
			unsigned long t = uutime/ticks + ustime/ticks;
			char tbuf[32], sbuf[32];
			int tday,tyear;
			time_t s_time;
			struct tm *proc, *now;
			const char *fmt;

			// Now get scheduler policy
			sched_policy = sched_getscheduler(pid);
			switch (sched_policy) {
				case SCHED_OTHER:
					r.scheduling_class = "TS";
					break;
				case SCHED_BATCH:
					r.scheduling_class = "B";
					break;
#ifdef SCHED_IDLE
				case SCHED_IDLE:
					r.scheduling_class = "#5";
					break;
#endif
				case SCHED_FIFO:
					r.scheduling_class = "FF";
					break;
				case SCHED_RR:
					r.scheduling_class = "RR";
					break;
				default:
					r.scheduling_class = "?";
					break;
			}

			// Calculate the start time
			s_time = time(NULL);
			now = localtime(&s_time);
			tyear = now->tm_year;
			tday = now->tm_yday;
			s_time = boot + (start / ticks);
			proc = localtime(&s_time);

			// Select format based on how long we've been running
			fmt = "%H:%M";
			if (tday != proc->tm_yday)
				fmt = "%b%d";
			if (tyear != proc->tm_year)
				fmt = "%Y";
			strftime(sbuf, sizeof(sbuf), fmt, proc);

			r.command = cmd;
			r.exec_time = convert_time(t, tbuf, sizeof(tbuf));
			r.pid = pid;
			r.ppid = ppid;
			r.priority = priority;
			r.start_time = sbuf;
			r.tty = tty_nr;
			r.user_id = get_effective_id(pid);
			report_finding(&r, probe_out);
		}
		SEXP_free(cmd_sexp);
	}
        closedir(d);

	return err;
}

SEXP_t *probe_main(SEXP_t *object, int *err, void *arg)
{
	SEXP_t *probe_out, *ent;

	ent = probe_obj_getent(object, "command", 1);
	if (ent == NULL) {
		*err = PROBE_ENOVAL;
		return NULL;
	}
	probe_out = SEXP_list_new(NULL);

	if (read_process(ent, probe_out)) {
		*err = PROBE_EACCES;
		SEXP_free(probe_out);
		probe_out = NULL;
	} else
		*err = 0;

	SEXP_free(ent);
	return probe_out;
}
#else
SEXP_t *probe_main(SEXP_t *object, int *err, void *arg)
{
        SEXP_t *item_sexp, *probe_out;

	item_sexp = probe_obj_creat ("process_item", NULL, NULL);
        probe_item_setstatus (item_sexp, OVAL_STATUS_NOTCOLLECTED);
        probe_out = SEXP_list_new (item_sexp, NULL);
        SEXP_free (item_sexp);

        return (probe_out);
}
#endif /* __linux */
