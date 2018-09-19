/**
 * @file   system_info_probe.c
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

#include "_seap.h"
#include <probe-api.h>
#include <probe/probe.h>
#include <probe/option.h>
#include <debug_priv.h>

#ifdef OS_WINDOWS
/* By defining WIN32_LEAN_AND_MEAN we ensure that Windows.h won't include
 * winsock.h, which would conflict with symbols from WinSock2.h.
 */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <WinSock2.h>
#include <Iphlpapi.h>
#include <Windows.h>
#include <ws2def.h>
#include <io.h>
#include <winternl.h>
#else
#include <sys/utsname.h>
#include <unistd.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include "system_info_probe.h"
#include "oscap_helpers.h"

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

static char *get_mac(const struct ifaddrs *ifa, int fd)
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

static char *get_mac(const struct ifaddrs *ifa, int fd)
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

       int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
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

                mac = get_mac(ifa, fd);
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
		SEXP_free(attrs);
		SEXP_free(r0);
		SEXP_free(r1);
		SEXP_free(r2);
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
		SEXP_free(attrs);
		SEXP_free(r0);
		SEXP_free(r1);
		SEXP_free(r2);
	}
 /* if not able to get info on interfaces, do not fail. */
	if (rc > 0)
		rc = 0;
#endif
leave1:
        freeifaddrs(ifaddr);
        return rc;
}

#elif defined(OS_WINDOWS)

#define VERSION_LEN 32
/* Microsoft recommends to start with a 15 kB buffer for GetAdaptersAddresses. */
#define ADDRESSES_BUFFER_SIZE 15000
#define MAX_TRIES 3
#define MAX_IP_ADDRESS_STRING_LENGTH 128

static int get_ifs(SEXP_t *item)
{
	/*
	 * Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h
	 * to define the Windows Socket specification version we will use.
	 * The current version of the Windows Sockets specification is version 2.2.
	 * Version 2.2 is supported on all systems since Windows 95 OSR2.
	 */
	WORD requested_version = MAKEWORD(2, 2);

	WSADATA wsa_data;
	int err = WSAStartup(requested_version, &wsa_data);
	if (err != 0) {
		char *error_message = oscap_windows_error_message(err);
		dE("Can't initialize Winsock DLL. WSAStartup failed: %s", error_message);
		free(error_message);
		return 1;
	}

	PIP_ADAPTER_ADDRESSES addresses = NULL;
	ULONG ret_val = 0;
	ULONG addresses_size = ADDRESSES_BUFFER_SIZE;
	ULONG family = AF_UNSPEC; // both IPv4 and IPv6
	ULONG flags = GAA_FLAG_INCLUDE_PREFIX;
	int iterations = 0;

	/* Try to get information about network adapters and their adresses */
	do {
		addresses = malloc(addresses_size);
		ret_val = GetAdaptersAddresses(family, flags, NULL, addresses, &addresses_size);
		if (ret_val == ERROR_BUFFER_OVERFLOW) {
			free(addresses);
			addresses = NULL;
		}
		iterations++;
	} while (ret_val == ERROR_BUFFER_OVERFLOW && iterations < MAX_TRIES);

	if (ret_val != NO_ERROR) {
		char *error_message = oscap_windows_error_message(err);
		dE("Can't get information about network adapters and their adresses. %s", error_message);
		free(error_message);
		free(addresses);
		WSACleanup();
		return 1;
	}

	/* Iterate over all network adapters */
	PIP_ADAPTER_ADDRESSES current_address = addresses;
	while (current_address != NULL) {
		/* MAC address */
		char *mac_address_str;
		if (current_address->PhysicalAddressLength != 0) {
			/* n bytes of MAC address will be printed as:
			(2 * n) hexa numbers + (n - 1) delimiters + 1 terminating null byte = 3 * n */
			const size_t mac_address_str_len = current_address->PhysicalAddressLength * 3;
			mac_address_str = malloc(mac_address_str_len);

			for (unsigned i = 0; i < current_address->PhysicalAddressLength; i++) {
				if (i == (current_address->PhysicalAddressLength - 1)) {
					snprintf(mac_address_str + i * 3, mac_address_str_len, "%.2X",
						(int)current_address->PhysicalAddress[i]);
				} else {
					snprintf(mac_address_str + i * 3, mac_address_str_len, "%.2X-",
						(int)current_address->PhysicalAddress[i]);
				}
			}
		} else {
			mac_address_str = strdup("unknown");
		}

		/* Find the network intefrace name */
		char *interface_name_str = oscap_windows_wstr_to_str(current_address->FriendlyName);

		/* Iterate over all unicast IP addresses of the network interface */
		PIP_ADAPTER_UNICAST_ADDRESS unicast_address = current_address->FirstUnicastAddress;
		while (unicast_address != NULL) {
			SOCKET_ADDRESS socket_address = unicast_address->Address;
			WCHAR ip_address_wstr[MAX_IP_ADDRESS_STRING_LENGTH];
			DWORD ip_address_wstr_length = MAX_IP_ADDRESS_STRING_LENGTH;
			int rc = WSAAddressToStringW(socket_address.lpSockaddr, socket_address.iSockaddrLength, NULL, ip_address_wstr, &ip_address_wstr_length);
			if (rc != 0) {
				free(mac_address_str);
				free(interface_name_str);
				free(addresses);
				WSACleanup();
				return 1;
			}

			/* Add prefix length (mask) */
			char *ip_address_str = oscap_windows_wstr_to_str(ip_address_wstr);
			UINT8 prefix = unicast_address->OnLinkPrefixLength;
			char *ip_address_with_prefix = oscap_sprintf("%s/%d", ip_address_str, prefix);
			free(ip_address_str);

			/* Create probe item */
			SEXP_t *interface_name_sexp = SEXP_string_newf(interface_name_str);
			SEXP_t *ip_address_sexp = SEXP_string_newf(ip_address_with_prefix);
			SEXP_t *mac_address_sexp = SEXP_string_newf(mac_address_str);
			SEXP_t *attrs = probe_attr_creat("name", interface_name_sexp,
				"ip_address", ip_address_sexp,
				"mac_address", mac_address_sexp,
				NULL);
			probe_item_ent_add(item, "interface", attrs, NULL);
			SEXP_free(attrs);
			SEXP_free(interface_name_sexp);
			SEXP_free(ip_address_sexp);
			SEXP_free(mac_address_sexp);
			free(ip_address_with_prefix);

			unicast_address = unicast_address->Next;
		}
		free(interface_name_str);
		free(mac_address_str);
		current_address = current_address->Next;
	}

	free(addresses);
	WSACleanup();
	return 0;
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

		SEXP_free(attrs);
		SEXP_free(r0);
		SEXP_free(r1);
		SEXP_free(r2);

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

#ifndef OS_WINDOWS
static FILE *_fopen_with_prefix(const char *prefix, const char *path)
{
	FILE *fp;
	if (prefix != NULL) {
		char *path_with_prefix = oscap_sprintf("%s%s", prefix, path);
		fp = fopen(path_with_prefix, "r");
		free(path_with_prefix);
	} else {
		fp = fopen(path, "r");
	}
	return fp;
}

static char *_offline_get_menuentry(const char *oscap_probe_root, int entry_num)
{
	FILE *fp;
	char *ret = NULL;
	char grubcfg[MAX_BUFFER_SIZE+1] = { '\0' };
	int len;

	fp = _fopen_with_prefix(oscap_probe_root, "/boot/grub2/grub.cfg");

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
	ret = strdup(grubcfg + ovec[0]);
fail2:
	fclose(fp);
fail:
	return ret;
}

static const char * _offline_get_os_version(char *os)
{
	char *ptr;

	if (os == NULL)
		return NULL;

	ptr = strchr(os, ' ');
	if (ptr)
		*ptr++ = '\0';

	return ptr;
}

static char * _offline_get_arch(const char *os)
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

static char *_offline_get_os_name(const char *oscap_probe_root)
{
	FILE *fp;
	int rc;
	char saved_entry[MAX_BUFFER_SIZE+1];
	char *ptr, *ret = NULL;

	fp = _fopen_with_prefix(oscap_probe_root, "/boot/grub2/grubenv");

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
		ret = _offline_get_menuentry(oscap_probe_root, nr);
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
		ret = strdup(ptr);
		pcre_free(re);
		goto finish;
	}
finish:
	fclose(fp);
fail:
	return ret;
}

static char *_offline_get_hname(const char *oscap_probe_root)
{
	FILE *fp;
	char hname[HOST_NAME_MAX+1] = { '\0' };
	char *ret = NULL;
	int rc;

	fp = _fopen_with_prefix(oscap_probe_root, "/etc/hostname");

	if (fp == NULL)
		goto fail;

	rc = fread(hname, 1, HOST_NAME_MAX, fp);
	/* If file is empty, we don't want to allocate an empty string for it */
	if (ferror(fp) || rc == 0 )
		goto finish;

	hname[strcspn(hname, "\n")] = '\0';
	ret = strdup(hname);

finish:
	fclose(fp);
fail:
	return ret;
}
#endif

#ifdef OS_WINDOWS
static char *get_windows_version()
{
	/* We can't use GetVersionEx to get Windows version because we want to get
	 * the real Windows version, but the compatibilty mode lies to applications
	 * about system version. Moreover the GetVersionEx function is deprecated
	 * since Windows 8.1. Instead we need to access RtlGetVersion.
	 * Code inspired by https://github.com/GNOME/glib/blob/master/glib/gwin32.c
	 */
	OSVERSIONINFOEXW osverinfo;
	typedef NTSTATUS(WINAPI f_rtl_get_version) (PRTL_OSVERSIONINFOEXW);
	f_rtl_get_version *rtl_get_version;
	HMODULE hmodule;

	hmodule = LoadLibraryW(L"ntdll.dll");
	rtl_get_version = (f_rtl_get_version *)GetProcAddress(hmodule, "RtlGetVersion");

	memset(&osverinfo, 0, sizeof(OSVERSIONINFOEXW));
	osverinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
	rtl_get_version(&osverinfo);

	char *version = oscap_sprintf("%d.%d.%d", osverinfo.dwMajorVersion,
		osverinfo.dwMinorVersion, osverinfo.dwBuildNumber);

	return version;
}

static const char *get_windows_architecture(void)
{
	SYSTEM_INFO si;
	GetNativeSystemInfo(&si);
	const char *arch;
	switch (si.wProcessorArchitecture) {
	case PROCESSOR_ARCHITECTURE_AMD64:
		arch = "x64";
		break;
	case PROCESSOR_ARCHITECTURE_ARM:
		arch = "ARM";
		break;
	case PROCESSOR_ARCHITECTURE_ARM64:
		arch = "ARM64";
		break;
	case PROCESSOR_ARCHITECTURE_IA64:
		arch = "Intel Itanium-based";
		break;
	case PROCESSOR_ARCHITECTURE_INTEL:
		arch = "x86";
		break;
	case PROCESSOR_ARCHITECTURE_UNKNOWN:
	default:
		arch = "unknown";
		break;
	}
	return arch;
}
#endif

int system_info_probe_offline_mode_supported()
{
	return PROBE_OFFLINE_OWN;
}

int system_info_probe_main(probe_ctx *ctx, void *arg)
{
	SEXP_t* item = NULL;
	char* os_name, *architecture, *hname;
	const char *os_version = NULL;
	const char unknown[] = "Unknown";
	int ret = 0;
	(void)arg;

	os_name = architecture = hname = NULL;

#ifdef OS_WINDOWS
	WCHAR computer_name_wstr[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD computer_name_len = MAX_COMPUTERNAME_LENGTH + 1;
	GetComputerNameW(computer_name_wstr, &computer_name_len);

	os_name = strdup("Windows");
	os_version = get_windows_version();
	architecture = strdup(get_windows_architecture());
	hname = oscap_windows_wstr_to_str(computer_name_wstr);
#else
	struct utsname sname;
	if (ctx->offline_mode == PROBE_OFFLINE_NONE) {
		if (uname(&sname) == 0) {
			os_name = strdup(sname.sysname);
			os_version = sname.version;
			architecture = strdup(sname.machine);
			hname = strdup(sname.nodename);
		}
	} else if (ctx->offline_mode & PROBE_OFFLINE_OWN) {
		const char *oscap_probe_root = getenv("OSCAP_PROBE_ROOT");
		os_name = _offline_get_os_name(oscap_probe_root);
		os_version = _offline_get_os_version(os_name);
		architecture = _offline_get_arch(os_version);
		hname = _offline_get_hname(oscap_probe_root);
	}
#endif

	/* All four elements are required */
	if (!os_name)
		os_name = strdup(unknown);

	// os_version kept static as it shared memory with os_name if os_name exists
	if (!os_version)
		os_version = unknown;

	if (!architecture)
		architecture = strdup(unknown);

	if (!hname)
		hname = strdup(unknown);

	if (__sysinfo_saneval(os_name) < 1 ||
		__sysinfo_saneval(os_version) < 1 ||
		__sysinfo_saneval(architecture) < 1 ||
		__sysinfo_saneval(hname) < 1) {
		ret = PROBE_EINVAL;
		goto cleanup;
	}

	item = probe_item_create(OVAL_INDEPENDENT_SYSCHAR_SUBTYPE, NULL,
	                         "os_name",           OVAL_DATATYPE_STRING, os_name,
	                         "os_version",        OVAL_DATATYPE_STRING, os_version,
	                         "os_architecture",   OVAL_DATATYPE_STRING, architecture,
	                         "primary_host_name", OVAL_DATATYPE_STRING, hname,
	                         NULL);
cleanup:
	free(os_name);
	// Free os_version only on Windows. On other platforms it shares same memory from os_name!
#ifdef OS_WINDOWS
	free(os_version);
#endif
	free(architecture);
	free(hname);

	if (ctx->offline_mode == PROBE_OFFLINE_NONE && item) {
		if (get_ifs(item)) {
			SEXP_free(item);
			return PROBE_EUNKNOWN;
		}
	}
	probe_item_collect(ctx, item);

	return ret;
}
