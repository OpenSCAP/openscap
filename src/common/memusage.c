#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "memusage.h"
#include "assume.h"
#include "bfind.h"

#if defined(__linux__)
struct proc_status {
	/* Peak resident set size ("high water mark"). */
	size_t VmHWM;
	/* Shared library code size. */
	size_t VmLib;
	/* Size of data segments */
	size_t VmData;
	/* Size of stack segments */
	size_t VmStk;
	/* Size of text segments */
	size_t VmExe;
	/* Locked memory size */
	size_t VmLck;
	/* Resident set size. */
	size_t VmRSS;
	/* Number of voluntary context switches */
	unsigned long vcsw;
	/* Number of involuntary context switches */
	unsigned long ncsw;
};

static int read_common_sizet(size_t *szp, char *strval)
{
	char *end;

	assume_d(szp    != NULL, -1);
	assume_d(strval != NULL, -1);

	end = strchr(strval, ' ');

	if (end == NULL)
		return (-1);

	*end = '\0';
	*szp = strtoll(strval, NULL, 10);

	if (errno == EINVAL ||
	    errno == ERANGE)
		return (-1);

	return (0);
}

static int read_common_ulong(unsigned long *ulp, char *strval)
{
	assume_d(ulp    != NULL, -1);
	assume_d(strval != NULL, -1);

	*ulp = strtoul(strval, NULL, 10);

	if (errno == EINVAL ||
	    errno == ERANGE)
		return (-1);

	return (0);
}

static int read_VmData(struct proc_status *pst, char *strval)
{
	return read_common_sizet(&pst->VmData, strval);
}

static int read_VmHWM(struct proc_status *pst, char *strval)
{
	return read_common_sizet(&pst->VmHWM, strval);
}

static int read_VmLck(struct proc_status *pst, char *strval)
{
	return read_common_sizet(&pst->VmLck, strval);
}

static int read_VmLib(struct proc_status *pst, char *strval)
{
	return read_common_sizet(&pst->VmLib, strval);
}

static int read_VmRSS(struct proc_status *pst, char *strval)
{
	return read_common_sizet(&pst->VmRSS, strval);
}

static int read_VmStk(struct proc_status *pst, char *strval)
{
	return read_common_sizet(&pst->VmStk, strval);
}

static int read_VmExe(struct proc_status *pst, char *strval)
{
	return read_common_sizet(&pst->VmExe, strval);
}

static int read_ncsw(struct proc_status *pst, char *strval)
{
	return read_common_ulong(&pst->ncsw, strval);
}

static int read_vcsw(struct proc_status *pst, char *strval)
{
	return read_common_ulong(&pst->vcsw, strval);
}

struct stat_parser {
	char *keyword;
	int (*storval)(struct proc_status *, char *);
};

struct stat_parser ptable[] = {
	{ "VmData",                     &read_VmData },
	{ "VmExe",                      &read_VmExe  },
	{ "VmHWM",                      &read_VmHWM  },
	{ "VmLck",                      &read_VmLck  },
	{ "VmLib",                      &read_VmLib  },
	{ "VmRSS",                      &read_VmRSS  },
	{ "VmStk",                      &read_VmStk  },
 	{ "nonvoluntary_ctxt_switches", &read_ncsw   },
	{ "voluntary_ctxt_switches",    &read_vcsw   }
};

#define PTABLE_COUNT (sizeof ptable / sizeof(struct stat_parser))

static int cmpkey (const char *a, const struct stat_parser *b)
{
	return strcmp(a, b->keyword);
}

static int get_proc_status(struct proc_status *pst)
{
        FILE *fp;
#define MEMUSAGE_LINUX_STATUS "/proc/self/status"
#define MEMUSAGE_LINUX_ENV    "MEMUSAGE_LINUX_STATUS"

	fp = fopen(
#ifndef MEMUSAGE_LINUX_DEBUG
		MEMUSAGE_LINUX_STATUS
#else
		getenv(MEMUSAGE_LINUX_ENV)?
		getenv(MEMUSAGE_LINUX_ENV):MEMUSAGE_LINUX_STATUS
#endif
		, "r");

	if (fp == NULL)
		return (-1);
	else {
		char linebuf[256];
		char *strval;
		struct stat_parser *sp;

		while (fgets(linebuf, sizeof linebuf - 1, fp) != NULL) {
			strval = strchr(linebuf, ':');

			if (strval == NULL)
				return (-1);

			*strval++ = '\0';

			while(isspace(*strval))
				++strval;

			sp = oscap_bfind(ptable, PTABLE_COUNT, sizeof(struct stat_parser), linebuf,
                                         (int(*)(void *, void *))&cmpkey);

			if (sp == NULL)
				continue;

			if (sp->storval(pst, strval) != 0)
				return (-1);
		}

		fclose(fp);
	}

	return (0);
}
#endif /* __linux__ */

int memusage (struct memusage *mu)
{
#if defined(__linux__)
	struct proc_status pst;

	if (get_proc_status(&pst) != 0)
		return (-1);

	mu->mu_rss   = pst.VmRSS;
	mu->mu_hwm   = pst.VmHWM;
	mu->mu_lib   = pst.VmLib;
	mu->mu_text  = pst.VmExe;
	mu->mu_data  = pst.VmData;
	mu->mu_stack = pst.VmStk;
	mu->mu_lock  = pst.VmLck;

	return (0);
#elif defined(__FreeBSD__)
	return (-1);
#else
	errno = EOPNOTSUPP;
	return (-1);
#endif
}
