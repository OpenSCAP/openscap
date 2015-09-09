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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

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

#ifdef HAVE_PROC_DEVNAME_H
 #include <proc/devname.h>
#else
 #include "process58-devname.h"
#endif

#include "seap.h"
#include "probe-api.h"
#include "probe/entcmp.h"
#include "alloc.h"
#include "common/debug_priv.h"

oval_schema_version_t over;

/* Convenience structure for the results being reported */
struct result_info {
        const char *command;
        const char *exec_time;
        unsigned pid;
        unsigned ppid;
        long priority;
	int ruid;
        const char *scheduling_class;
        const char *start_time;
	const char *tty;
	int user_id;
};

static void report_finding(struct result_info *res, probe_ctx *ctx)
{
        SEXP_t *item;
	SEXP_t *se_ruid;

	if (oval_schema_version_cmp(over, OVAL_SCHEMA_VERSION(5.8)) < 0) {
		se_ruid = NULL;
	} else {
		se_ruid = SEXP_number_newu_64(res->ruid);
	}

        item = probe_item_create(OVAL_UNIX_PROCESS, NULL,
                                 "command",   OVAL_DATATYPE_STRING, res->command,
                                 "exec_time", OVAL_DATATYPE_STRING, res->exec_time,
                                 "pid",       OVAL_DATATYPE_INTEGER, (int64_t)res->pid,
                                 "ppid",      OVAL_DATATYPE_INTEGER, (int64_t)res->ppid,
                                 "priority",  OVAL_DATATYPE_INTEGER, (int64_t)res->priority,
				 "ruid",      OVAL_DATATYPE_SEXP, se_ruid,
                                 "scheduling_class", OVAL_DATATYPE_STRING, res->scheduling_class,
                                 "start_time", OVAL_DATATYPE_STRING, res->start_time,
                                 "tty",          OVAL_DATATYPE_STRING, res->tty,
                                 "user_id",    OVAL_DATATYPE_INTEGER, (int64_t)res->user_id,
                                 NULL);

        probe_item_collect(ctx, item);
}

#if defined(__linux__)

unsigned long ticks, boot;

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

static int get_uids(int pid, struct result_info *r)
{
	char buf[100];
	FILE *sf;

	r->ruid = -1;
	r->user_id = -1;

	snprintf(buf, sizeof(buf), "/proc/%d/status", pid);
	sf = fopen(buf, "rt");
	if (sf) {
		int line = 0;
		__fsetlocking(sf, FSETLOCKING_BYCALLER);
		while (fgets(buf, sizeof(buf), sf)) {
			if (line == 0) {
				line++;
				continue;
			}
			if (memcmp(buf, "Uid:", 4) == 0) {
				sscanf(buf, "Uid: %d %d", &r->ruid, &r->user_id);
				break;
			}
		}
		fclose(sf);
	}

	return 0;
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

static int read_process(SEXP_t *cmd_ent, probe_ctx *ctx)
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
		char *tmp, cmd[16], state, tty_dev[128];
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
		dI("Have command: %s\n", cmd);
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
			//
			// FROM THE SPEC:
			// "This is the time of day the process started formatted in HH:MM:SS if
			// the same day the process started or formatted as MMM_DD (Ex.: Feb_5)
			// if process started the previous day or further in the past."
			//
			if (tday != proc->tm_yday || tyear != proc->tm_year)
				fmt = "%b_%d";
			else
				fmt = "%H:%M:%S";
			strftime(sbuf, sizeof(sbuf), fmt, proc);

			r.command = cmd;
			r.exec_time = convert_time(t, tbuf, sizeof(tbuf));
			r.pid = pid;
			r.ppid = ppid;
			r.priority = priority;
			r.start_time = sbuf;

                        dev_to_tty(tty_dev, sizeof(tty_dev), (dev_t) tty_nr, pid, ABBREV_DEV);
                        r.tty = tty_dev;

			get_uids(pid, &r);
			report_finding(&r, ctx);
		}
		SEXP_free(cmd_sexp);
	}
        closedir(d);

	return err;
}

int probe_main(probe_ctx *ctx, void *arg)
{
	SEXP_t *ent;

	ent = probe_obj_getent(probe_ctx_getobject(ctx), "command", 1);
	if (ent == NULL) {
		return PROBE_ENOVAL;
	}

	if (read_process(ent, ctx)) {
		SEXP_free(ent);
		return PROBE_EACCESS;
	}

	SEXP_free(ent);

	return 0;
}
#elif defined (__SVR4) && defined (__sun)

#include <procfs.h>
#include <unistd.h>

static char *convert_time(time_t t, char *tbuf, int tb_size)
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

static int read_process(SEXP_t *cmd_ent, probe_ctx *ctx)
{
	int err = 1;
	DIR *d;
	struct dirent *ent;

	d = opendir("/proc");
	if (d == NULL)
		return err;

	psinfo_t *psinfo;

	// Scan the directories
	while (( ent = readdir(d) )) {
		int fd, len;
		char buf[336];
		int pid;
		unsigned sched_policy;
		SEXP_t *cmd_sexp;


		// Skip non-process dir entries
		if(*ent->d_name<'0' || *ent->d_name>'9')
			continue;
		errno = 0;
		pid = strtol(ent->d_name, NULL, 10);
		if (errno || pid == 2) // skip err & kthreads
			continue;

		// Parse up the stat file for the proc
		snprintf(buf, 32, "/proc/%d/psinfo", pid);
		fd = open(buf, O_RDONLY, 0);
		if (fd < 0)
			continue;
		len = read(fd, buf, sizeof buf);
		close(fd);
		if (len < 336)
			continue;

		// The psinfo file contains a psinfo struct; this typecast gets us the struct directly
		psinfo = (psinfo_t *) buf;


		err = 0; // If we get this far, no permission problems
		dI("Have command: %s\n", psinfo->pr_fname);
		cmd_sexp = SEXP_string_newf("%s", psinfo->pr_fname);
		if (probe_entobj_cmp(cmd_ent, cmd_sexp) == OVAL_RESULT_TRUE) {
			struct result_info r;
			char tbuf[32], sbuf[32];
			int tday,tyear;
			time_t s_time;
			struct tm *proc, *now;
			const char *fmt;
			int fixfmt_year;

			r.scheduling_class = malloc(PRCLSZ);
			strncpy(r.scheduling_class, (psinfo->pr_lwp).pr_clname, sizeof(r.scheduling_class));

			// Get the start time
			s_time = time(NULL);
			now = localtime(&s_time);
			tyear = now->tm_year;
			tday = now->tm_yday;

			// Get current time
			s_time = psinfo->pr_start.tv_sec;
			proc = localtime(&s_time);

			// Select format based on how long we've been running
			//
			// FROM THE SPEC:
			// "This is the time of day the process started formatted in HH:MM:SS if
			// the same day the process started or formatted as MMM_DD (Ex.: Feb_5)
			// if process started the previous day or further in the past."
			//
			if (tday != proc->tm_yday || tyear != proc->tm_year)
				fmt = "%b_%d";
			else
				fmt = "%H:%M:%S";
			strftime(sbuf, sizeof(sbuf), fmt, proc);

			r.command = psinfo->pr_fname;
			r.exec_time = convert_time(psinfo->pr_time.tv_sec, tbuf, sizeof(tbuf));
			r.pid = psinfo->pr_pid;
			r.ppid = psinfo->pr_ppid;
			r.priority = (psinfo->pr_lwp).pr_pri;
			r.ruid = psinfo->pr_uid;
			r.start_time = sbuf;
			r.tty = oscap_sprintf("%s", psinfo->pr_ttydev);
			r.user_id = psinfo->pr_euid;
			report_finding(&r, ctx);
		}
		SEXP_free(cmd_sexp);
	}
        closedir(d);

	return err;
}

int probe_main(probe_ctx *ctx, void *arg)
{
	SEXP_t *obj, *ent;

	obj = probe_ctx_getobject(ctx);
	over = probe_obj_get_platform_schema_version(obj);
	ent = probe_obj_getent(obj, "command", 1);
	if (ent == NULL) {
		return PROBE_ENOVAL;
	}

	if (read_process(ent, ctx)) {
		SEXP_free(ent);
		return PROBE_EACCESS;
	}

	SEXP_free(ent);

	return 0;
}

#else
int probe_main(probe_ctx *ctx, void *arg)
{
        SEXP_t *item_sexp;

	item_sexp = probe_item_creat ("process_item", NULL, NULL);
        probe_item_setstatus (item_sexp, SYSCHAR_STATUS_NOT_COLLECTED);
        probe_item_collect(ctx, item_sexp);

	return 0;
}
#endif /* __linux */
