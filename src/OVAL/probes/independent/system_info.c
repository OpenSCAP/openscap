/**
 * @file   system_info.c
 * @brief  system_info probe
 * @author "Tomas Heinrich" <theinric@redhat.com>
 */

/*
 * Copyright 2009 Red Hat Inc., Durham, North Carolina.
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
 *      "Tomas Heinrich" <theinric@redhat.com>
 */

/*
 * system_info probe:
 *
 *  (for internal use)
 *
 *  system_info_object
 *
 *  system_info_item
 *    attrs
 *      id
 *      status_enum status
 *    os_name
 *    os_version
 *    architecture
 *    primary_host_name
 *    //interfaces
 *    interface [0..*]
 *      attrs
 *        interface_name
 *        ip_address
 *        mac_address
 *    any [0..*]
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <seap.h>
#include <probe-api.h>
#include <probe/probe.h>
#include <probe/option.h>

#include <sys/utsname.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

#undef OS_FREEBSD
#undef OS_LINUX
#undef OS_SOLARIS
#undef OS_SUNOS
#undef OS_WINDOWS

#if defined(__FreeBSD__)
# define OS_FREEBSD
#elif defined(__linux__) || defined(__GNU__) || defined(__GLIBC__)
# define OS_LINUX
#elif defined(sun) || defined(__sun)
# if defined(__SVR4) || defined(__svr4__)
#  define OS_SOLARIS
# else
#  define OS_SUNOS
# endif
#elif defined(_WIN32)
# define OS_WINDOWS
#else
# error "Sorry, your OS isn't supported."
#endif

#if defined(OS_LINUX)
#include <sys/socket.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <string.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/types.h>

#include <pcre.h>
#define _REGEX_RES_VECSIZE     3
#define _REGEX_MENUENTRY       "(?<=menuentry ').*?(?=')"
#define _REGEX_SAVED_ENTRY_NR  "(?<=saved_entry=)[0-9]+"
#define _REGEX_SAVED_ENTRY     "(?<=saved_entry=).*"
#define _REGEX_ARCH            "(?<=\\.)i386|i686|x86_64|ia64|alpha|amd64|arm|armeb|armel|hppa|m32r" \
		               "|m68k|mips|mipsel|powerpc|ppc64|s390|s390x|sh3|sh3eb|sh4|sh4eb|sparc"
#define MAX_BUFFER_SIZE        4096

static int fd=-1;

static char *get_mac(const struct ifaddrs *ifa)
{
       struct ifreq ifr;
       unsigned char mac[6];
       static char mac_buf[20];

       memset(&ifr, 0, sizeof(struct ifreq));
       strncpy(ifr.ifr_name, ifa->ifa_name, IFNAMSIZ);
       ifr.ifr_name[IFNAMSIZ-1] = 0;
       if (ioctl(fd, SIOCGIFHWADDR, &ifr) >= 0) {
               memcpy(mac, ifr.ifr_hwaddr.sa_data, sizeof(mac));
               snprintf(mac_buf, sizeof(mac_buf),
                       "%02X:%02X:%02X:%02X:%02X:%02X",
                       mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
       } else
               mac_buf[0] = 0;

       return mac_buf;
}
#elif defined(OS_SOLARIS)
#include <sys/socket.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <string.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/sockio.h>
#include <net/if_types.h>
#include <libdlpi.h>

static int fd=-1;

static char *get_mac(const struct ifaddrs *ifa)
{
       struct lifreq lifr;
	size_t physaddrlen = DLPI_PHYSADDR_MAX;
	uchar_t physaddr[DLPI_PHYSADDR_MAX];
	static char mac_buf[DLPI_PHYSADDR_MAX];
	char *str;
	int retv;
	dlpi_handle_t dh;
	dlpi_info_t dlinfo;

	memset(mac_buf, 0, sizeof(mac_buf));
       memset(&lifr, 0, sizeof(struct lifreq));
       strlcpy(lifr.lifr_name, ifa->ifa_name, sizeof (lifr.lifr_name));
	if (ioctl(fd, SIOCGLIFFLAGS, &lifr) >= 0) {

		if (lifr.lifr_flags & (IFF_VIRTUAL| IFF_IPMP))
			return (mac_buf);

		if (dlpi_open(lifr.lifr_name, &dh, 0) != DLPI_SUCCESS)
			return (NULL);

		retv = dlpi_get_physaddr(dh, DL_CURR_PHYS_ADDR, physaddr,
			&physaddrlen);
		if (retv != DLPI_SUCCESS) {
			dlpi_close(dh);
			return (NULL);
		}

		retv = dlpi_info(dh, &dlinfo, DLPI_INFO_VERSION);
		if (retv != DLPI_SUCCESS) {
			dlpi_close(dh);
			return (NULL);
		}
		dlpi_close(dh);
		str = _link_ntoa(physaddr, NULL, physaddrlen, IFT_OTHER);

		if (str != NULL && physaddrlen != 0) {
			switch(dlinfo.di_mactype) {
			case DL_IB:
				break;
			default:
				strlcpy(mac_buf, str, sizeof(mac_buf));
				break;
			}
			free(str);
		}
	}
	return mac_buf;
}
#endif

#if defined(OS_LINUX) || (defined(OS_SOLARIS))
static int get_ifs(SEXP_t *item)
{
       struct ifaddrs *ifaddr, *ifa;
       int family, rc=1;
       char host[NI_MAXHOST], *mac;
       SEXP_t *attrs;
       SEXP_t *r0, *r1, *r2;
#if defined(OS_SOLARIS)
       int item_added = 0;
#endif

       if (getifaddrs(&ifaddr) == -1)
               return rc;

       fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
       if (fd < 0)
               goto leave1;

        /* Walk through linked list, maintaining head pointer so we
          can free list later */
        for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
                if (ifa->ifa_addr) {
                        family = ifa->ifa_addr->sa_family;
                        if (family != AF_INET && family != AF_INET6)
                                continue;
                } else
                        continue;

                mac = get_mac(ifa);
#if defined(OS_SOLARIS)
		if (mac == NULL) {
			rc = 1;
			goto leave2;
		}
		if (mac[0] == '\0')
			continue;
#endif
		if (family == AF_INET) {
			rc = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
				host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
			if (rc) {
				rc = 1;
				goto leave2;
			}
		}
		else {
			struct sockaddr_in6 *sin6p;
			sin6p = (struct sockaddr_in6 *) ifa->ifa_addr;
			if (! inet_ntop(family, (const void *)&sin6p->sin6_addr, host, NI_MAXHOST)) {
				rc = 1;
				goto leave2;
			}
		}
	        attrs = probe_attr_creat("name",
                                 r0 = SEXP_string_newf("%s", ifa->ifa_name),
                                 "ip_address",
                                 r1 = SEXP_string_newf("%s", host),
                                 "mac_address",
                                 r2 = SEXP_string_newf("%s", mac),
                                 NULL);
	        probe_item_ent_add(item, "interface", attrs, NULL);
#if defined(OS_SOLARIS)
		item_added = 1;
#endif
		SEXP_vfree(attrs, r0, r1, r2, NULL);
	}
leave2:
        close(fd);
#if defined(OS_SOLARIS)
	if (item_added == 0) {
		attrs = probe_attr_creat("name",
					 r0 = SEXP_string_newf("dummy0"),
					 "ip_address",
					 r1 = SEXP_string_newf("127.0.0.1"),
					 "mac_address",
					 r2 = SEXP_string_newf("aa:bb:cc:dd:ee:ff"),
					 NULL);
		probe_item_ent_add(item, "interface", attrs, NULL);
		SEXP_vfree(attrs, r0, r1, r2, NULL);
	}
 /* if not able to get info on interfaces, do not fail. */
	if (rc > 0)
		rc = 0;
#endif
leave1:
        freeifaddrs(ifaddr);
        return rc;
}

#else

static int get_ifs(SEXP_t *item)
{
        /* TODO */

        SEXP_t *attrs, *r0, *r1, *r2;

        attrs = probe_attr_creat("name",
                                 r0 = SEXP_string_newf("dummy0"),
                                 "ip_address",
                                 r1 = SEXP_string_newf("127.0.0.1"),
                                 "mac_address",
                                 r2 = SEXP_string_newf("aa:bb:cc:dd:ee:ff"),
                                 NULL);

        probe_item_ent_add(item, "interface", attrs, NULL);
        SEXP_vfree (attrs, r0, r1, r2, NULL);

        return 0;
}
#endif

/*
 * If no forbidden chars are found, returns the trimmed length
 * of the input string. Otherwise a negative number is returned
 */
static ssize_t __sysinfo_saneval(const char *s)
{
	size_t i, space_count, real_length;

	real_length = space_count = i = 0;

	for (; i < strlen(s); ++i) {
		/* check for space */
		if (isspace(s[i])) {
			if (real_length > 0) {
				++space_count;
				continue;
			}
		}
		/* check for printable */
		if (!isprint(s[i])) {
			return -1;
		}
		/* check for forbidden chars */
		switch(s[i]) {
		  case '\'':
		  case '"':
		  case '<':
		  case '>':
			return -1;
		}
		/* Allowed character found */
		real_length += 1 + space_count;
		space_count = 0;
	}
	return (ssize_t)real_length;
}

static char * _offline_chroot_get_menuentry(int entry_num)
{
	FILE *fp;
	char *ret = NULL;
	char grubcfg[MAX_BUFFER_SIZE+1] = { '\0' };
	int len;

	fp = fopen("/boot/grub2/grub.cfg", "r");
	if (fp == NULL)
		goto fail;

	int rc = PCRE_ERROR_NOMATCH;
	int erroffset, ovec[_REGEX_RES_VECSIZE];
	const char *error;
	pcre *re;
	re = pcre_compile(_REGEX_MENUENTRY, 0, &error, &erroffset, NULL);

	if (re == NULL)
		goto fail2;

	while ((len = fread(grubcfg, 1, MAX_BUFFER_SIZE, fp)) > 0) {
		if (ferror(fp)) {
			pcre_free(re);
			goto fail2;
		}

		memset(ovec, 0, sizeof(ovec));
		do {
			rc = pcre_exec(re, NULL, grubcfg, len, ovec[1], 0, ovec, _REGEX_RES_VECSIZE);
			if (rc > 0)
				entry_num--;
		} while (rc > 0 && entry_num > 0);

		if (entry_num == 0)
			break;
	}
	pcre_free(re);

	if (rc == PCRE_ERROR_NOMATCH)
		goto fail2;

	grubcfg[ovec[1]] = '\0';
	ret = oscap_strdup(grubcfg + ovec[0]);
fail2:
	fclose(fp);
fail:
	return ret;
}

static const char * _offline_chroot_get_os_version(char *os)
{
	char *ptr;

	if (os == NULL)
		return NULL;

	ptr = strchr(os, ' ');
	if (ptr)
		*ptr++ = '\0';

	return ptr;
}

static char * _offline_chroot_get_arch(const char *os)
{
	int rc;
	char *ptr = NULL;

	if (os == NULL)
		return NULL;

	int erroffset, ovec[_REGEX_RES_VECSIZE] = { 0 };
	const char *error;
	pcre *re;

	re = pcre_compile(_REGEX_ARCH, 0, &error, &erroffset, NULL);
	if (re == NULL)
		return NULL;

	rc = pcre_exec(re, NULL, os, strlen(os), 0, 0, ovec, _REGEX_RES_VECSIZE);
	if (rc == PCRE_ERROR_NOMATCH)
		goto fail;

	size_t len = ovec[1] - ovec[0];
	ptr = malloc(sizeof(char) * len + 1);
	if (ptr == NULL)
		goto fail;
	ptr = strncpy(ptr, os + ovec[0], len);
	ptr[len] = '\0';
fail:
	pcre_free(re);
	return ptr;
}

static char * _offline_chroot_get_os_name(void)
{
	FILE *fp;
	int rc;
	char saved_entry[MAX_BUFFER_SIZE+1];
	char *ptr, *ret = NULL;

	fp = fopen("/boot/grub2/grubenv", "r");
	if (fp == NULL)
		goto fail;

	rc = fread(saved_entry, 1, MAX_BUFFER_SIZE, fp);
	if (ferror(fp))
		goto finish;
	saved_entry[rc] = '\0';

	size_t len;
	int erroffset, ovec[_REGEX_RES_VECSIZE] = { 0 };
	const char *error;
	pcre *re;

	re = pcre_compile(_REGEX_SAVED_ENTRY_NR, 0, &error, &erroffset, NULL);
	if (re == NULL)
		goto finish;

	len = strlen(saved_entry);

	rc = pcre_exec(re, NULL, saved_entry, len, 0, 0, ovec, _REGEX_RES_VECSIZE);
	if (rc > 0) {
		saved_entry[ovec[1]] = '\0';
		ptr = saved_entry + ovec[0];
		int nr = atoi(ptr);
		ret = _offline_chroot_get_menuentry(nr);
		pcre_free(re);
		goto finish;
	}

	re = pcre_compile(_REGEX_SAVED_ENTRY, 0, &error, &erroffset, NULL);
	if (re == NULL)
		goto finish;

	rc = pcre_exec(re, NULL, saved_entry, len, 0, 0, ovec, _REGEX_RES_VECSIZE);
	if (rc > 0) {
		saved_entry[ovec[1]] = '\0';
		ptr = saved_entry + ovec[0];
		ret = oscap_strdup(ptr);
		pcre_free(re);
		goto finish;
	}
finish:
	fclose(fp);
fail:
	return ret;
}

static char * _offline_chroot_get_hname(void)
{
	FILE *fp;
	char hname[HOST_NAME_MAX+1] = { '\0' };
	char *ret = NULL;
	int rc;

	fp = fopen("/etc/hostname", "r");
	if (fp == NULL)
		goto fail;

	rc = fread(hname, 1, HOST_NAME_MAX, fp);
	/* If file is empty, we don't want to allocate an empty string for it */
	if (ferror(fp) || rc == 0 )
		goto finish;

	hname[strcspn(hname, "\n")] = '\0';
	ret = oscap_strdup(hname);

finish:
	fclose(fp);
fail:
	return ret;
}

void *probe_init(void)
{
	probe_setoption(PROBEOPT_OFFLINE_MODE_SUPPORTED, PROBE_OFFLINE_ALL);
	return NULL;
}

int probe_main(probe_ctx *ctx, void *arg)
{
	SEXP_t* item = NULL;
	char* os_name, *architecture, *hname;
	const char *os_version = NULL;
	const char unknown[] = "Unknown";
	struct utsname sname;
	probe_offline_flags offline_mode = PROBE_OFFLINE_NONE;
	int ret = 0;
	(void)arg;

	os_name = architecture = hname = NULL;
	probe_getoption(PROBEOPT_OFFLINE_MODE_SUPPORTED, NULL, &offline_mode);

	if (offline_mode == PROBE_OFFLINE_NONE) {
		if (uname(&sname) == 0) {
			os_name = oscap_strdup(sname.sysname);
			os_version = sname.version;
			architecture = oscap_strdup(sname.machine);
			hname = oscap_strdup(sname.nodename);
		}
	} else if (offline_mode == PROBE_OFFLINE_CHROOT) {
		os_name = _offline_chroot_get_os_name();
		os_version = _offline_chroot_get_os_version(os_name);
		architecture = _offline_chroot_get_arch(os_version);
		hname = _offline_chroot_get_hname();
	}

	/* All four elements are required */
	if (!os_name)
		os_name = oscap_strdup(unknown);

	// os_version kept static as it shared memory with os_name if os_name exists
	if (!os_version)
		os_version = unknown;

	if (!architecture)
		architecture = oscap_strdup(unknown);

	if (!hname)
		hname = oscap_strdup(unknown);

	if (__sysinfo_saneval(os_name) < 1 ||
		__sysinfo_saneval(os_version) < 1 ||
		__sysinfo_saneval(architecture) < 1 ||
		__sysinfo_saneval(hname) < 1) {
		ret = PROBE_EINVAL;
		goto cleanup;
	}

	item = probe_item_create(OVAL_SUBTYPE_SYSINFO, NULL,
	                         "os_name",           OVAL_DATATYPE_STRING, os_name,
	                         "os_version",        OVAL_DATATYPE_STRING, os_version,
	                         "os_architecture",   OVAL_DATATYPE_STRING, architecture,
	                         "primary_host_name", OVAL_DATATYPE_STRING, hname,
	                         NULL);
cleanup:
	free(os_name);
	// Don't free os_version, it shares same memory from os_name!
	free(architecture);
	free(hname);

	if (!offline_mode && item) {
		if (get_ifs(item)) {
			SEXP_free(item);
			return PROBE_EUNKNOWN;
		}
	}
	probe_item_collect(ctx, item);

	return ret;
}
