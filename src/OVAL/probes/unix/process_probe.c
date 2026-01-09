/**
 * @file   process_probe.c
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
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
#endif

#include "_seap.h"
#include "probe-api.h"
#include "probe/entcmp.h"
#include "common/debug_priv.h"
#include "process_probe.h"
#include "oscap_helpers.h"

#if defined(OS_FREEBSD)
#include <kvm.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/user.h>
#include <limits.h>
#include <paths.h>

#define SEC_PER_YR  31557600
#define SEC_PER_DAY 86400
#define SEC_PER_HR  3600
#define SEC_PER_MIN 60
#endif

static oval_schema_version_t over;

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
	if (se_ruid) {
		SEXP_free(se_ruid);
	}

        probe_item_collect(ctx, item);
}

#if defined(OS_LINUX)

static unsigned long ticks, boot;

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
	struct tm result;

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
		dI("Have command: %s", cmd);
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
			now = localtime_r(&s_time, &result);
			tyear = now->tm_year;
			tday = now->tm_yday;
			s_time = boot + (start / ticks);
			proc = localtime_r(&s_time, &result);

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

int process_probe_main(probe_ctx *ctx, void *arg)
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
#elif defined (OS_SOLARIS)

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
		dI("Have command: %s", psinfo->pr_fname);
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
			strncpy(r.scheduling_class, (psinfo->pr_lwp).pr_clname, PRCLSZ);

			// Get the start time
			s_time = time(NULL);
			now = localtime_r(&s_time, &result);
			tyear = now->tm_year;
			tday = now->tm_yday;

			// Get current time
			s_time = psinfo->pr_start.tv_sec;
			proc = localtime_r(&s_time, &result);

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

int process_probe_main(probe_ctx *ctx, void *arg)
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

#elif defined(OS_FREEBSD)
static char *convert_time(time_t t, char *tbuf, int tb_size)
{
	unsigned int days = t / SEC_PER_DAY;
	unsigned int hrs = (t - days*SEC_PER_DAY) / SEC_PER_HR;
	unsigned int min = (t - days*SEC_PER_DAY - hrs*SEC_PER_HR) / SEC_PER_MIN;
	unsigned int sec = (t - days*SEC_PER_DAY - hrs*SEC_PER_HR - min*SEC_PER_MIN);

	if (days) {
		snprintf(tbuf, tb_size, "%u-%02u:%02u:%02u", days, hrs, min, sec);
	} else {
		snprintf(tbuf, tb_size,	"%02u:%02u:%02u", hrs, min, sec);
	}

	return tbuf;
}

static int read_process(SEXP_t *cmd_ent, probe_ctx *ctx)
{
	struct kinfo_proc *proclist, *proc;
	struct result_info r;
	char buf[LINE_MAX];
	int i, j, count;
	kvm_t *kd;

	kd = kvm_openfiles(NULL, _PATH_DEVNULL, NULL, O_RDONLY, buf);

	if (!kd) {
		dE("Can't obtain kvm handle");
		return (PROBE_EFATAL);
	}

	proclist = kvm_getprocs(kd, KERN_PROC_PROC, 0, &count);

	if (!proclist) {
		dE("Can't obtain process list");
		kvm_close(kd);
		return (PROBE_EFATAL);
	}

	proc = proclist;

	for (i = 0; i < count; i++, proc++) {
		// Skip kthreads
		if (proc->ki_ppid == 0)
			continue;

		SEXP_t *cmd_sexp;
		char **argbuf = NULL;
		char arg_dest[LINE_MAX] = {0};
		argbuf = kvm_getargv(kd, proc, LINE_MAX);

		if (!argbuf) {
			r.command = "";
		} else {
			for (j = 0; argbuf[j] != NULL; j++) {
				strncat(arg_dest, argbuf[j], LINE_MAX - strlen(arg_dest) - 1);
				strncat(arg_dest, " ", LINE_MAX - strlen(arg_dest) - 1);
			}

			arg_dest[strlen(arg_dest)-1] = '\0';
			r.command = arg_dest;
		}

		dD("Have command: %s", r.command);
		cmd_sexp = SEXP_string_newf("%s", r.command);

		if (probe_entobj_cmp(cmd_ent, cmd_sexp) == OVAL_RESULT_TRUE) {
			const char *fmt;
			char tty_buf[64];
			char exec_buf[64];
			char start_buf[64];

			time_t curr_year;
			time_t curr_day;
			time_t start_year;
			time_t start_day;
			time_t exec_time_diff;

			time_t start_time = proc->ki_start.tv_sec;
			struct timeval current_time;

			gettimeofday(&current_time, NULL);
			exec_time_diff = current_time.tv_sec - proc->ki_start.tv_sec;

			curr_year = current_time.tv_sec / SEC_PER_YR;
			curr_day = current_time.tv_sec / SEC_PER_DAY;
			start_year = start_time / SEC_PER_YR;
			start_day = start_time / SEC_PER_DAY;

			if (curr_year != start_year || curr_day != start_day) {
				fmt = "%b_%d";
			} else {
				fmt = "%H:%M:%S";
			}

			struct tm result;
			struct tm *loc_time = localtime_r(&start_time, &result);
			strftime(start_buf, sizeof(start_buf), fmt, loc_time);

			switch(proc->ki_tdev) {
				case NODEV:
					r.tty = "?";
					break;
				default:
					snprintf(tty_buf, sizeof(tty_buf), "%ld", proc->ki_tdev);
					r.tty = tty_buf;
					break;
			}

			switch (proc->ki_pri.pri_class) {
				case SCHED_OTHER:
					r.scheduling_class = "TS";
					break;
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

			r.exec_time = convert_time(exec_time_diff, exec_buf, sizeof(exec_buf));
			r.pid = proc->ki_pid;
			r.ppid = proc->ki_ppid;
			r.priority = proc->ki_pri.pri_level;
			r.ruid = proc->ki_ruid;
			r.user_id = proc->ki_uid;
			r.start_time = start_buf;

			report_finding(&r, ctx);
		}
		SEXP_free(cmd_sexp);
	}

	if (kvm_close(kd)) {
		dE("Error closing kvm handle");
		return (PROBE_EFAULT);
	}

	return 0;
}

int process_probe_main(probe_ctx *ctx, void *arg)
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
#else
int process_probe_main(probe_ctx *ctx, void *arg)
{
        SEXP_t *item_sexp;

	item_sexp = probe_item_create(OVAL_UNIX_PROCESS, NULL, NULL);
        probe_item_setstatus (item_sexp, SYSCHAR_STATUS_NOT_COLLECTED);
        probe_item_collect(ctx, item_sexp);

	return 0;
}
#endif /* __linux */
