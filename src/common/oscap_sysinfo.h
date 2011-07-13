#ifndef SYSINFO_H
#define SYSINFO_H

#if defined(__linux__)
# include <sys/sysinfo.h>
#elif defined(__FreeBSD__) || defined(__SVR4)
/*
 * sysinfo structure as defined in sys/sysinfo.h on Linux
 */
struct sysinfo {
	long uptime;
	unsigned long loads[3];
	unsigned long totalram;
	unsigned long freeram;
	unsigned long sharedram;
	unsigned long bufferram;
	unsigned long totalswap;
	unsigned long freeswap;
	unsigned short procs;
	unsigned long totalhigh;
	unsigned long freehigh;
	unsigned int mem_unit;
	char _f[20-2*sizeof(long)-sizeof(int)];
};

int oscap_sysinfo(struct sysinfo *info);
#endif

#endif /* SYSINFO_H */
