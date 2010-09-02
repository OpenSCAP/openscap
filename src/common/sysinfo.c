#include "sysinfo.h"

#if defined(__FreeBSD__)
int sysinfo(struct sysinfo *info)
{
	return (-1);
}
#endif
