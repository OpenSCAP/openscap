/**
 * @file   process58.c
 * @brief  process58 probe
 * @author "Steve Grubb" <sgrubb@redhat.com>
 *
 * 2010/06/13 dkopecek@redhat.com
 *  This probe is able to process58 an process58_object as defined in OVAL 5.4 and 5.5.
 *
 */

/*
 * Copyright 2009-2011 Red Hat Inc., Durham, North Carolina.
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
 *   Petr Lautrbach <plautrba@redhat.com>
 */

/*
 * process58 probe:
 *
 * command_line
 * exec_time
 * pid
 * ppid
 * priority
 * ruid
 * scheduling_class
 * start_time
 * tty
 * user_id
 * exec_shield
 * loginuid
 * posix_capability
 * selinux_domain_label
 * session_id
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

#ifdef HAVE_SELINUX_SELINUX_H
#include <selinux/selinux.h>
#include <selinux/context.h>
#endif
#ifdef HAVE_SYS_CAPABILITY_H
#include <ctype.h>
#include <sys/types.h>

#ifndef HAVE_CAP_GET_PID // we don't have cap_get_pid => assume libcap version == 1
#undef _POSIX_SOURCE
#define LIBCAP_VERSION 1
#include <sys/capability.h>
#define CAP_LAST_CAP CAP_AUDIT_CONTROL
extern char const *_cap_names[];
#else
#define LIBCAP_VERSION 2
#include <sys/capability.h>
#endif

#include "util.h"
#include "process58-capability.h"
#endif /* HAVE_SYS_CAPABILITY_H */

#include "seap.h"
#include "probe-api.h"
#include "probe/entcmp.h"
#include "alloc.h"
#include "common/debug_priv.h"
#include <ctype.h>
#include "common/oscap_buffer.h"

/* Convenience structure for the results being reported */
struct result_info {
        const char *command_line;
        const char *exec_time;
        unsigned pid;
        unsigned ppid;
        long priority;
	int ruid;
        const char *scheduling_class;
        const char *start_time;
        const char *tty;
	int user_id;
	int exec_shield;
	unsigned loginuid;
	char **posix_capability;
	const char *selinux_domain_label;
	int session_id;
};

static void report_finding(struct result_info *res, probe_ctx *ctx)
{
        SEXP_t *item;

        item = probe_item_create(OVAL_UNIX_PROCESS58, NULL,
                                 "command_line", OVAL_DATATYPE_STRING, res->command_line,
                                 "exec_time",    OVAL_DATATYPE_STRING, res->exec_time,
                                 "pid",          OVAL_DATATYPE_INTEGER, (int64_t)res->pid,
                                 "ppid",         OVAL_DATATYPE_INTEGER, (int64_t)res->ppid,
                                 "priority",     OVAL_DATATYPE_INTEGER, (int64_t)res->priority,
				 "ruid",         OVAL_DATATYPE_INTEGER, (int64_t) res->ruid,
                                 "scheduling_class", OVAL_DATATYPE_STRING, res->scheduling_class,
                                 "start_time",   OVAL_DATATYPE_STRING, res->start_time,
                                 "tty",          OVAL_DATATYPE_STRING, res->tty,
                                 "user_id",      OVAL_DATATYPE_INTEGER, (int64_t)res->user_id,
                                 "exec_shield",  OVAL_DATATYPE_BOOLEAN, (bool)res->exec_shield,
                                 "loginuid",     OVAL_DATATYPE_INTEGER, (int64_t)res->loginuid,
				 "posix_capability", OVAL_DATATYPE_STRING_M, res->posix_capability,
				 "selinux_domain_label", OVAL_DATATYPE_STRING, res->selinux_domain_label,
                                 "session_id", OVAL_DATATYPE_INTEGER, (int64_t)res->session_id,
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
	r->loginuid = -1;

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

	snprintf(buf, sizeof(buf), "/proc/%d/loginuid", pid);
	sf = fopen(buf, "rt");
	if (sf) {
		if (fscanf(sf, "%u", &r->loginuid) < 1) {
			dW("fscanf failed from %s", buf);
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

static char *get_selinux_label(int pid) {
#ifdef HAVE_SELINUX_SELINUX_H
	char *selinux_label;
	security_context_t pid_context;
	context_t context;

	if (getpidcon(pid, &pid_context) == -1) {
		/* error getting pid selinux context */
		dW("Can't get selinux context for process %d", pid);
		return NULL;
	}
	context = context_new(pid_context);
	selinux_label = strdup(context_type_get(context));
	context_free(context);
	freecon(pid_context);
	return selinux_label;

#else
	return NULL;
#endif /* HAVE_SELINUX_SELINUX_H */
}

static char **get_posix_capability(int pid, int max_cap_id) {
#ifdef HAVE_SYS_CAPABILITY_H
	cap_t pid_caps;
	char *cap_name, **ret = NULL;
	unsigned cap_value, ret_index = 0;
	cap_flag_value_t cap_flag;
	int cap_id;

#if LIBCAP_VERSION == 1
	pid_caps = cap_init();
	if (capgetp(pid, pid_caps) == -1) {
		dW("Can't get capabilities for process %d", pid);
		cap_free(pid_caps);
		return NULL;
	}
#elif LIBCAP_VERSION == 2
	pid_caps = cap_get_pid(pid);
#else
	dW("Can't detect libcap version");
	return NULL;
#endif

	if (pid_caps == NULL) {
		dW("Can't get capabilities for process %d", pid);
		return NULL;
	}

	for (cap_value = 0; cap_value < CAP_LAST_CAP; cap_value++) {
		if (cap_get_flag(pid_caps, cap_value, CAP_EFFECTIVE, &cap_flag) == -1)
			continue;
		if (cap_flag == CAP_SET) {
#if LIBCAP_VERSION == 2
			cap_name = cap_to_name(cap_value);
#else
			cap_name = strdup(_cap_names[cap_value]);
#endif
			if (cap_name != NULL) {
				char *cap_name_p = cap_name;
				while (*cap_name_p) {
					*cap_name_p = toupper(*cap_name_p);
					cap_name_p++;
				}

				cap_id = oscap_string_to_enum(CapabilityType, cap_name);
				if (cap_id > -1 && cap_id <= max_cap_id) {
					ret = realloc(ret, (ret_index + 1) * sizeof(char *));
					ret[ret_index] = strdup(cap_name);
					ret_index++;
				}
				cap_free(cap_name);
			}
		}
	}
	ret = realloc(ret, (ret_index + 1) * sizeof(char *));
	ret[ret_index] = NULL;

	cap_free(pid_caps);
	return ret;
#else
	return NULL;
#endif
}

/* get exec shield status according to http://people.redhat.com/sgrubb/files/lsexec
 * return value: -1 - not detected, 0 - disabled, 1 - enabled */
static int get_exec_shield_status(int pid) {
	char buf[501];
	FILE *sf;
	long unsigned low, high, inode;
	long long unsigned offset;
	int dev_min, dev_maj;
	char perm[3], trim;
	int ret = -1, read_items;

	snprintf(buf, sizeof(buf), "/proc/%d/maps", pid);
	sf = fopen(buf, "rt");
	if (sf) {
		while (fgets(buf, 500, sf)) {
			read_items = sscanf(
				buf, "%lx-%lx rw%s %llx %x:%x %lu %c\n",
				&low, &high, perm, &offset, &dev_min,
				&dev_maj, &inode, &trim
			);
			if (read_items == 7) {
				if (perm[0] == 'x' && offset != 0) {
					ret = 0;
				}
				else {
					ret = 1;
				}
			}
		}
		fclose(sf);
	}

	return ret;
}

/**
 * Parse /proc/%d/cmdline file
 * @param filepath Path to file ~ use preallocated buffer for the path
 * @param buffer output buffer with non-zero size
 * @return ps-like command info or NULL
 */
static inline bool get_process_cmdline(const char* filepath, struct oscap_buffer* const buffer){

	int fd = open(filepath, O_RDONLY, 0);

	if (fd < 0) {
		return false;
	}

	oscap_buffer_clear(buffer);



	for(;;) {
		static const int chunk_size = 1024;
		char chunk[chunk_size];
		// Read data, store to buffer
		ssize_t read_size = read(fd, chunk, chunk_size );
		if (read_size < 0) {
			close(fd);
			return false;
		}
		oscap_buffer_append_binary_data(buffer, chunk, read_size);

		// If reach end of file, then end the loop
		if (chunk_size != read_size) {
			break;
		}
	}

	close(fd);

	int length = oscap_buffer_get_length(buffer);
	char* buffer_mem = oscap_buffer_get_raw(buffer);

	if ( length == 0 ) { // empty file
		return false;
	} else {

		// Skip multiple trailing zeros
		int i = length - 1;
		while ( (i > 0) && (buffer_mem[i] == '\0') ) {
			--i;
		}

		// Program and args are separated by '\0'
		// Replace them with spaces ' '
		while( i >= 0 ){
			char chr = buffer_mem[i];
			if ( ( chr == '\0') || ( chr == '\n' ) ) {
				buffer_mem[i] = ' ';
			} else if ( !isprint(chr) ) { // "ps" replace non-printable characters with '.' (LC_ALL=C)
				buffer_mem[i] = '.';
			}
			--i;
		}
	}
	return true;
}

/**
 * Make "[%s] <defunct>" from cmd string - inplace
 * @param cmd_buffer @see read_process() > cmd_buffer
 * @return pointer to start of string
 */
static inline char *make_defunc_str(char* const cmd_buffer){
	static const char DEFUNC_STR[] = "] <defunct>";

	size_t len = strlen(cmd_buffer);
	memcpy(cmd_buffer + len, DEFUNC_STR, sizeof(DEFUNC_STR));
	return cmd_buffer;
}

static int read_process(SEXP_t *cmd_ent, SEXP_t *pid_ent, probe_ctx *ctx)
{
	int err = 1, max_cap_id;
	DIR *d;
	struct dirent *ent;
	oval_schema_version_t oval_version;

	d = opendir("/proc");
	if (d == NULL)
		return err;

	// Get the time tick hertz
	ticks = (unsigned long)sysconf(_SC_CLK_TCK);
	get_boot_time();

	oval_version = probe_obj_get_platform_schema_version(probe_ctx_getobject(ctx));
	if (oval_schema_version_cmp(oval_version, OVAL_SCHEMA_VERSION(5.11)) < 0) {
		max_cap_id = OVAL_5_8_MAX_CAP_ID;
	} else {
		max_cap_id = OVAL_5_11_MAX_CAP_ID;
	}

	struct oscap_buffer *cmdline_buffer = oscap_buffer_new();
	
	char cmd_buffer[1 + 15 + 11 + 1]; // Format:" [ cmd:15 ] <defunc>"
	cmd_buffer[0] = '[';

	// Scan the directories
	while (( ent = readdir(d) )) {
		int fd, len;
		char buf[256];
		char *tmp, state, tty_dev[128];
		int pid, ppid, pgrp, session, tty_nr, tpgid;
		unsigned flags, sched_policy;
		unsigned long minflt, cminflt, majflt, cmajflt, uutime, ustime;
		long cutime, cstime, priority, cnice, nthreads, itrealvalue;
		unsigned long long start;
		SEXP_t *cmd_sexp = NULL, *pid_sexp = NULL;

		// Skip non-process58 dir entries
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
		memset(cmd_buffer + 1, 0, sizeof(cmd_buffer)-1); // clear cmd after starting '['
		sscanf(buf, "%d (%15c", &ppid, cmd_buffer + 1);
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

		const char* cmd;
		if (state == 'Z') { // zombie
			cmd = make_defunc_str(cmd_buffer);
		} else {
			snprintf(buf, 32, "/proc/%d/cmdline", pid);
			if (get_process_cmdline(buf, cmdline_buffer)) {
				cmd = oscap_buffer_get_raw(cmdline_buffer); // use full cmdline
			} else {
				cmd = cmd_buffer + 1;
			}
		}


		err = 0; // If we get this far, no permission problems
		dI("Have command: %s", cmd);
		cmd_sexp = SEXP_string_newf("%s", cmd);
		pid_sexp = SEXP_number_newu_32(pid);
		if ((cmd_sexp == NULL || probe_entobj_cmp(cmd_ent, cmd_sexp) == OVAL_RESULT_TRUE) &&
		    (pid_sexp == NULL || probe_entobj_cmp(pid_ent, pid_sexp) == OVAL_RESULT_TRUE)
		) {
			struct result_info r;
			unsigned long t = uutime/ticks + ustime/ticks;
			char tbuf[32], sbuf[32], *selinux_domain_label, **posix_capabilities;
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

			r.command_line = cmd;
			r.exec_time = convert_time(t, tbuf, sizeof(tbuf));
			r.pid = pid;
			r.ppid = ppid;
			r.priority = priority;
			r.start_time = sbuf;

			dev_to_tty(tty_dev, sizeof(tty_dev), (dev_t) tty_nr, pid, ABBREV_DEV);
			r.tty = tty_dev;

			r.exec_shield = (get_exec_shield_status(pid) > 0);

			selinux_domain_label = get_selinux_label(pid);
			r.selinux_domain_label = selinux_domain_label;

			posix_capabilities = get_posix_capability(pid, max_cap_id);
			r.posix_capability = posix_capabilities;

			r.session_id = session;

			get_uids(pid, &r);
			report_finding(&r, ctx);

			if (selinux_domain_label != NULL)
				free(selinux_domain_label);

			if (posix_capabilities != NULL) {
				char **posix_capabilities_p = posix_capabilities;
				while (*posix_capabilities_p)
					free(*posix_capabilities_p++);
				free(posix_capabilities);
			}
		}
		SEXP_free(cmd_sexp);
		SEXP_free(pid_sexp);
	}
        closedir(d);
	oscap_buffer_free(cmdline_buffer);
	return err;
}

int probe_main(probe_ctx *ctx, void *arg)
{
	SEXP_t *command_line_ent, *pid_ent;

	command_line_ent = probe_obj_getent(probe_ctx_getobject(ctx), "command_line", 1);
	pid_ent = probe_obj_getent(probe_ctx_getobject(ctx), "pid", 1);
	if (command_line_ent == NULL && pid_ent == NULL) {
		return PROBE_ENOVAL;
	}

	if (read_process(command_line_ent, pid_ent, ctx)) {
		SEXP_free(command_line_ent);
		SEXP_free(pid_ent);
		return PROBE_EACCESS;
	}

	SEXP_free(command_line_ent);
	SEXP_free(pid_ent);

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


		// Skip non-process58 dir entries
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

			r.command_line = psinfo->pr_fname;
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
int probe_main(probe_ctx *ctx, void *arg)
{
        SEXP_t *item_sexp;

	item_sexp = probe_item_creat ("process58_item", NULL, NULL);
        probe_item_setstatus (item_sexp, SYSCHAR_STATUS_NOT_COLLECTED);
        probe_item_collect(ctx, item_sexp);

	return 0;
}
#endif /* __linux */
