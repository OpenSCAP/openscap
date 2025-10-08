/*
 * Copyright 2010 Red Hat Inc., Durham, North Carolina.
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
 *      Lukas Kuklinek <lkuklinek@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <math.h>
#include <string.h>

#ifdef OS_WINDOWS
 /* By defining WIN32_LEAN_AND_MEAN we ensure that Windows.h won't include
  * winsock.h, which would conflict with symbols from WinSock2.h.
  */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <iphlpapi.h>
#include <windows.h>
#include <ws2def.h>
#include <io.h>
#include <winternl.h>
#include <lmcons.h>
#else
#include <unistd.h>
#endif

#ifdef OSCAP_UNIX
#include <sys/utsname.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <pwd.h>
#include <sys/types.h>
#endif

#if defined(OS_LINUX)
#include <ifaddrs.h>
#include <net/if.h>
#include <sys/ioctl.h>
#endif

#if defined(OS_FREEBSD)
#include <arpa/inet.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/sockio.h>
#endif

#include "item.h"
#include "helpers.h"
#include "xccdf_impl.h"
#include "common/_error.h"
#include "oscap_text.h"
#include "common/debug_priv.h"
#include "source/oscap_source_priv.h"
#include "oscap_helpers.h"

#define XCCDF_NUMERIC_SIZE 32

#ifdef OS_WINDOWS
#define timezone _timezone
#endif

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX _POSIX_HOST_NAME_MAX
#endif

// References containing STIG Rule IDs can be found by their href attribute, it must match the following url
static const char *DISA_STIG_VIEWER_HREF[] = { "http://iase.disa.mil/stigs/Pages/stig-viewing-guidance.aspx",
						"https://public.cyber.mil/stigs/srg-stig-tools/",
						"https://www.cyber.mil/stigs/srg-stig-tools/" };

// constants
static const xccdf_numeric XCCDF_SCORE_MAX_DAFAULT = 100.0f;
static const char *XCCDF_INSTANCE_DEFAULT_CONTEXT = "undefined";
const char *XCCDF_NUMERIC_FORMAT = "%f";

// prototypes
void xccdf_rule_result_free(struct xccdf_rule_result *rr);
void xccdf_identity_free(struct xccdf_identity *identity);
void xccdf_score_free(struct xccdf_score *score);
void xccdf_target_fact_free(struct xccdf_target_fact *fact);
void xccdf_message_free(struct xccdf_message *msg);
void xccdf_instance_free(struct xccdf_instance *inst);
void xccdf_override_free(struct xccdf_override *oride);

struct xccdf_result *xccdf_result_new(void)
{
	struct xccdf_item *result = xccdf_item_new(XCCDF_RESULT, NULL);
	oscap_create_lists(&result->sub.result.identities, &result->sub.result.targets,
		&result->sub.result.remarks, &result->sub.result.target_addresses,
		&result->sub.result.target_facts, &result->sub.result.target_id_refs, &result->sub.result.applicable_platforms,
		&result->sub.result.setvalues, &result->sub.result.organizations,
		&result->sub.result.rule_results, &result->sub.result.scores, NULL);
	return XRESULT(result);
}

struct xccdf_result * xccdf_result_clone(const struct xccdf_result * result)
{
	struct xccdf_item *new_result = calloc(1, sizeof(struct xccdf_item) + sizeof(struct xccdf_result_item));
	struct xccdf_item *old = XITEM(result);
    xccdf_item_base_clone(&new_result->item, &(old->item));
	new_result->type = old->type;
	xccdf_result_item_clone(&new_result->sub.result, &old->sub.result);
	return XRESULT(new_result);
}

static inline void xccdf_result_free_impl(struct xccdf_item *result)
{
	if (result != NULL) {
		free(result->sub.result.start_time);
		free(result->sub.result.end_time);
		free(result->sub.result.test_system);
		free(result->sub.result.benchmark_uri);
		free(result->sub.result.profile);

		oscap_list_free(result->sub.result.identities, (oscap_destruct_func) xccdf_identity_free);
		oscap_list_free(result->sub.result.target_facts, (oscap_destruct_func) xccdf_target_fact_free);
		oscap_list_free(result->sub.result.target_id_refs, (oscap_destruct_func) xccdf_target_identifier_free);
		oscap_list_free(result->sub.result.applicable_platforms, free);
		oscap_list_free(result->sub.result.targets, free);
		oscap_list_free(result->sub.result.scores, (oscap_destruct_func) xccdf_score_free);
		oscap_list_free(result->sub.result.remarks, (oscap_destruct_func) oscap_text_free);
		oscap_list_free(result->sub.result.target_addresses, free);
		oscap_list_free(result->sub.result.setvalues, (oscap_destruct_func) xccdf_setvalue_free);
		oscap_list_free(result->sub.result.rule_results, (oscap_destruct_func) xccdf_rule_result_free);
		oscap_list_free(result->sub.result.organizations, free);

		xccdf_item_release(result);
	}
}

XCCDF_FREE_GEN(result)

XCCDF_ACCESSOR_SIMPLE(result, const struct xccdf_version_info*, schema_version);
XCCDF_ACCESSOR_STRING(result, start_time)
XCCDF_ACCESSOR_STRING(result, end_time)
XCCDF_ACCESSOR_STRING(result, test_system)
XCCDF_ACCESSOR_STRING(result, benchmark_uri)
XCCDF_ACCESSOR_STRING(result, profile)
XCCDF_LISTMANIP(result, identity, identities)
XCCDF_LISTMANIP_STRING(result, target, targets)
XCCDF_LISTMANIP_STRING(result, target_address, target_addresses)
XCCDF_LISTMANIP_STRING(result, organization, organizations)
XCCDF_LISTMANIP_TEXT(result, remark, remarks)
XCCDF_LISTMANIP(result, target_fact, target_facts)
XCCDF_LISTMANIP(result, target_identifier, target_id_refs)
XCCDF_LISTMANIP_STRING(result, applicable_platform, applicable_platforms)
XCCDF_LISTMANIP(result, setvalue, setvalues)
XCCDF_LISTMANIP(result, rule_result, rule_results)
XCCDF_LISTMANIP(result, score, scores)
OSCAP_ITERATOR_GEN(xccdf_result)
OSCAP_ITERATOR_REMOVE_F(xccdf_result)

static inline void _xccdf_result_add_target_fact_uniq(struct xccdf_result *result, struct xccdf_target_fact *fact)
{
	struct xccdf_target_fact_iterator *target_facts = xccdf_result_get_target_facts(result);
	while (xccdf_target_fact_iterator_has_more(target_facts)) {
			struct xccdf_target_fact *target_fact = xccdf_target_fact_iterator_next(target_facts);
			if (target_fact->type == fact->type)
				if (target_fact->name != NULL && fact->name != NULL && !strcmp(target_fact->name, fact->name))
					if (target_fact->value != NULL && fact->value != NULL && !strcmp(target_fact->value, fact->value)) {
						xccdf_target_fact_free(fact);
						goto exit;
					}
	}
	xccdf_result_add_target_fact(result, fact);
exit:
	xccdf_target_fact_iterator_free(target_facts);
}

static inline void _xccdf_result_fill_scanner(struct xccdf_result *result)
{
	struct xccdf_target_fact *fact = NULL;
	fact = xccdf_target_fact_new();
	xccdf_target_fact_set_name(fact, "urn:xccdf:fact:scanner:name");
	xccdf_target_fact_set_string(fact, "OpenSCAP");
	xccdf_result_add_target_fact(result, fact);

	fact = xccdf_target_fact_new();
	xccdf_target_fact_set_name(fact, "urn:xccdf:fact:scanner:version");
	xccdf_target_fact_set_string(fact, oscap_get_version());
	xccdf_result_add_target_fact(result, fact);
}

#if defined(OSCAP_UNIX)
static inline char *_unix_get_name_fallback(void) {
	struct passwd *passwd;

	errno = 0;
	passwd = getpwuid(getuid());
	if (passwd == NULL || passwd->pw_name == NULL) {
		dW("Error when calling getpwuid(): %d, %s\n", errno, strerror(errno));
		return "\0";
	}

	return passwd->pw_name;
}
#endif

static inline void _xccdf_result_fill_identity(struct xccdf_result *result)
{
	struct xccdf_identity *id = xccdf_identity_new();
#if defined(OS_WINDOWS)
	TCHAR w32_username[UNLEN + 1];
	DWORD w32_usernamesize = UNLEN + 1;
#endif
	// Reasoning here is that OpenSCAP does not authenticate user
	// nor it grants she any additional privileges
	xccdf_identity_set_authenticated(id, 0);
	xccdf_identity_set_privileged(id, 0);
#ifdef OSCAP_UNIX
	char *name = getlogin();
	if (name == NULL)
		name = _unix_get_name_fallback();
	xccdf_identity_set_name(id, name);
#elif defined(OS_WINDOWS)
	GetUserName((TCHAR *) w32_username, &w32_usernamesize); /* XXX: Check the return value? */
	xccdf_identity_set_name(id, w32_username);
#else
# warning "_xccdf_result_fill_identity: no support for identity on this OS"
#endif
	xccdf_result_add_identity(result, id);
}

static inline void _xccdf_result_clear_metadata(struct xccdf_item *result)
{
	oscap_list_free(result->sub.result.targets, free);
	oscap_list_free(result->sub.result.target_addresses, free);
	oscap_list_free(result->sub.result.target_facts, (oscap_destruct_func) xccdf_target_fact_free);
	oscap_list_free(result->sub.result.identities, (oscap_destruct_func) xccdf_identity_free);
	oscap_create_lists(
		&result->sub.result.targets,
		&result->sub.result.target_addresses,
		&result->sub.result.target_facts,
		&result->sub.result.identities,
		NULL);
}

#ifdef OSCAP_UNIX
static char *_get_etc_hostname(const char *oscap_probe_root)
{
	FILE *fp;
	char hname[HOST_NAME_MAX+1] = { '\0' };
	char *ret = NULL;
	int rc;

	fp = oscap_fopen_with_prefix(oscap_probe_root, "/etc/hostname");

	if (fp == NULL) {
		dD("Trying to use /proc/sys/kernel/hostname instead of /etc/hostname");
		fp = oscap_fopen_with_prefix(oscap_probe_root, "/proc/sys/kernel/hostname");
	}

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

void xccdf_result_fill_sysinfo(struct xccdf_result *result)
{
#if defined(OS_LINUX) || defined(OS_FREEBSD)
	struct ifaddrs *ifaddr, *ifa;
	int fd;
#endif
	struct xccdf_target_fact *fact = NULL;
	const char *probe_root = getenv("OSCAP_PROBE_ROOT");

	_xccdf_result_clear_metadata(XITEM(result));
	_xccdf_result_fill_scanner(result);

#ifdef OSCAP_UNIX
	char *hostname = NULL;
	char *fqdn = NULL;
	if (probe_root) {
		hostname = _get_etc_hostname(probe_root);
	} else {
		char hname[_POSIX_HOST_NAME_MAX+1] = {0};
		if (gethostname(hname, _POSIX_HOST_NAME_MAX)) {
			dW("Unable to get hostname: %s", strerror(errno));
		} else {
			hostname = strdup(hname);
		}

		if (hostname) {
			struct addrinfo hints, *info, *p;
			int gai_res;

			memset(&hints, 0, sizeof hints);
			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_flags = AI_CANONNAME;

			if (!(gai_res = getaddrinfo(hostname, NULL, &hints, &info))) {
				for(p = info; p != NULL; p = p->ai_next) {
					if (!p->ai_canonname)
						continue;
					fact = xccdf_target_fact_new();
					xccdf_target_fact_set_name(fact, "urn:xccdf:fact:asset:identifier:fqdn");
					xccdf_target_fact_set_string(fact, p->ai_canonname);
					/* store FQDN under XCCDF1.2 (6.6.3) predefined name */
					_xccdf_result_add_target_fact_uniq(result, fact);
					if (!fqdn)
						fqdn = strdup(p->ai_canonname);
				}
				freeaddrinfo(info);
			} else {
				dI("Unable to get FQDN(s) via getaddrinfo: %s", gai_strerror(gai_res));
			}
		}
	}
	/* store target's host name */
	xccdf_result_add_target(result, fqdn ? fqdn : (hostname ? hostname : "unknown"));
	if (hostname) {
		fact = xccdf_target_fact_new();
		xccdf_target_fact_set_name(fact, "urn:xccdf:fact:asset:identifier:host_name");
		xccdf_target_fact_set_string(fact, hostname);
		/* store host name under XCCDF1.2 (6.6.3) predefined name */
		xccdf_result_add_target_fact(result, fact);
	}
	free(hostname);
	free(fqdn);
#elif defined(OS_WINDOWS)
	TCHAR computer_name[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD computer_name_size = MAX_COMPUTERNAME_LENGTH + 1;
	GetComputerName(computer_name, &computer_name_size);
	/* store target name */
	xccdf_result_add_target(result, computer_name);

	fact = xccdf_target_fact_new();
	xccdf_target_fact_set_name(fact, "urn:xccdf:fact:asset:identifier:host_name");
	xccdf_target_fact_set_string(fact, computer_name);
	/* store host name under XCCDF1.2 (6.6.3) predefined name */
	xccdf_result_add_target_fact(result, fact);
#endif

	const char *ev_target = getenv("OSCAP_EVALUATION_TARGET");
	if (ev_target) {
		fact = xccdf_target_fact_new();
		xccdf_target_fact_set_name(fact, "urn:xccdf:fact:identifier");
		xccdf_target_fact_set_string(fact, ev_target);
		xccdf_result_add_target_fact(result, fact);

		fact = xccdf_target_fact_new();
		xccdf_target_fact_set_name(fact, "urn:xccdf:fact:asset:identifier:ein");
		xccdf_target_fact_set_string(fact, ev_target);
		/* store target id under XCCDF1.2 (6.6.3) predefined name */
		xccdf_result_add_target_fact(result, fact);
	}

	if (!probe_root)
		_xccdf_result_fill_identity(result);

#if defined(OS_LINUX) || defined(OS_FREEBSD)
	if (!probe_root) {
		/* get network interfaces */
		if (getifaddrs(&ifaddr) == -1)
			return;

		fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
		if (fd == -1)
			goto out1;

		for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
			int family;
			char hostip[NI_MAXHOST];
			struct ifreq ifr;

			if (!ifa->ifa_addr)
				continue;
			family = ifa->ifa_addr->sa_family;

			if (family == AF_INET || family == AF_INET6) {
				if (family == AF_INET) {
					if (getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
							hostip, sizeof(hostip), NULL, 0, NI_NUMERICHOST))
						goto out2;
				} else if (family == AF_INET6) {
					struct sockaddr_in6 *sin6;

					sin6 = (struct sockaddr_in6 *) ifa->ifa_addr;
					if (!inet_ntop(family, (const void *) &sin6->sin6_addr,
							hostip, sizeof(hostip)))
						goto out2;
				}
				/* store ip address */
				xccdf_result_add_target_address(result, hostip);
				fact = xccdf_target_fact_new();
				xccdf_target_fact_set_name(fact, family == AF_INET ? "urn:xccdf:fact:asset:identifier:ipv4" : "urn:xccdf:fact:asset:identifier:ipv6");
				xccdf_target_fact_set_string(fact, hostip);
				/* store ipv4(6) address under XCCDF1.2 (6.6.3) predefined name */
				_xccdf_result_add_target_fact_uniq(result, fact);
			}

			memset(&ifr, 0, sizeof(ifr));
			strncpy(ifr.ifr_name, ifa->ifa_name, IFNAMSIZ);
			ifr.ifr_name[IFNAMSIZ - 1] = 0;
#if defined(OS_LINUX)
			if (ioctl(fd, SIOCGIFHWADDR, &ifr) >= 0) {
				unsigned char mac[6];
				char macbuf[20];

				memcpy(mac, ifr.ifr_hwaddr.sa_data, sizeof(mac));
				snprintf(macbuf, sizeof(macbuf), "%02X:%02X:%02X:%02X:%02X:%02X",
					 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
#elif defined(OS_FREEBSD)
			if (ioctl(fd, SIOCGHWADDR, &ifr) >= 0) {
				unsigned char mac[6];
				char macbuf[20];

				memcpy(mac, ifr.ifr_addr.sa_data, sizeof(mac));
				snprintf(macbuf, sizeof(macbuf), "%02X:%02X:%02X:%02X:%02X:%02X",
					 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
#endif
				fact = xccdf_target_fact_new();
				xccdf_target_fact_set_name(fact, "urn:xccdf:fact:ethernet:MAC");
				xccdf_target_fact_set_string(fact, macbuf);
				/* store mac address */
				_xccdf_result_add_target_fact_uniq(result, fact);

				fact = xccdf_target_fact_new();
				xccdf_target_fact_set_name(fact, "urn:xccdf:fact:asset:identifier:mac");
				xccdf_target_fact_set_string(fact, macbuf);
				/* store mac address under XCCDF1.2 (6.6.3) predefined name */
				_xccdf_result_add_target_fact_uniq(result, fact);
			}
		}

	out2:
		close(fd);
	out1:
		freeifaddrs(ifaddr);
	}
#elif defined(OS_WINDOWS)

#define VERSION_LEN 32
/* Microsoft recommends to start with a 15 kB buffer for GetAdaptersAddresses. */
#define ADDRESSES_BUFFER_SIZE 15000
#define MAX_TRIES 3
#define MAX_IP_ADDRESS_STRING_LENGTH 128

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
		return;
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
		return;
	}

	/* Iterate over all network adapters */
	PIP_ADAPTER_ADDRESSES current_address = addresses;
	while (current_address != NULL) {
		/* MAC address */
		char *mac_address_str = NULL;
		if (current_address->PhysicalAddressLength != 0) {
			/* n bytes of MAC address will be printed as:
			(2 * n) hexa numbers + (n - 1) delimiters + 1 terminating null byte = 3 * n */
			const size_t mac_address_str_len = current_address->PhysicalAddressLength * 3;
			mac_address_str = malloc(mac_address_str_len);

			for (unsigned i = 0; i < current_address->PhysicalAddressLength; i++) {
				if (i == (current_address->PhysicalAddressLength - 1)) {
					snprintf(mac_address_str + i * 3, mac_address_str_len, "%.2X",
						(int)current_address->PhysicalAddress[i]);
				}
				else {
					snprintf(mac_address_str + i * 3, mac_address_str_len, "%.2X-",
						(int)current_address->PhysicalAddress[i]);
				}
			}

			/* Add the MAC address to XCCDF TestResult/target-facts */
			struct xccdf_target_fact *fact = xccdf_target_fact_new();
			xccdf_target_fact_set_name(fact, "urn:xccdf:fact:ethernet:MAC");
			xccdf_target_fact_set_string(fact, mac_address_str);
			/* store mac address */
			_xccdf_result_add_target_fact_uniq(result, fact);

			fact = xccdf_target_fact_new();
			xccdf_target_fact_set_name(fact, "urn:xccdf:fact:asset:identifier:mac");
			xccdf_target_fact_set_string(fact, mac_address_str);
			/* store mac address under XCCDF1.2 (6.6.3) predefined name */
			_xccdf_result_add_target_fact_uniq(result, fact);

			free(mac_address_str);
		}

		/* Iterate over all unicast IP addresses of the network interface */
		PIP_ADAPTER_UNICAST_ADDRESS unicast_address = current_address->FirstUnicastAddress;
		while (unicast_address != NULL) {
			SOCKET_ADDRESS socket_address = unicast_address->Address;
			WCHAR ip_address_wstr[MAX_IP_ADDRESS_STRING_LENGTH];
			DWORD ip_address_wstr_length = MAX_IP_ADDRESS_STRING_LENGTH;
			int rc = WSAAddressToStringW(socket_address.lpSockaddr, socket_address.iSockaddrLength, NULL, ip_address_wstr, &ip_address_wstr_length);
			if (rc != 0) {
				free(addresses);
				WSACleanup();
				return;
			}

			/* Add the IP address to XCCDF TestResult/target-address */
			char *ip_address_str = oscap_windows_wstr_to_str(ip_address_wstr);
			xccdf_result_add_target_address(result, ip_address_str);

			fact = xccdf_target_fact_new();
			xccdf_target_fact_set_name(fact, socket_address.lpSockaddr->sa_family == AF_INET ? "urn:xccdf:fact:asset:identifier:ipv4" : "urn:xccdf:fact:asset:identifier:ipv6");
			xccdf_target_fact_set_string(fact, ip_address_str);
			/* store ipv4(6) address under XCCDF1.2 (6.6.3) predefined name */
			_xccdf_result_add_target_fact_uniq(result, fact);

			free(ip_address_str);

			unicast_address = unicast_address->Next;
		}
		current_address = current_address->Next;
	}

	free(addresses);
	WSACleanup();
	return 0;

#endif
}

struct xccdf_rule_result *xccdf_rule_result_new(void)
{
	struct xccdf_rule_result *rr = calloc(1, sizeof(struct xccdf_rule_result));
	oscap_create_lists(&rr->overrides, &rr->idents, &rr->messages,
		&rr->instances, &rr->fixes, &rr->checks, NULL);
	return rr;
}

void xccdf_rule_result_free(struct xccdf_rule_result *rr)
{
	if (rr != NULL) {
		free(rr->idref);
		free(rr->version);
		free(rr->time);

		oscap_list_free(rr->overrides, (oscap_destruct_func) xccdf_override_free);
		oscap_list_free(rr->idents, (oscap_destruct_func) xccdf_ident_free);
		oscap_list_free(rr->messages, (oscap_destruct_func) xccdf_message_free);
		oscap_list_free(rr->instances, (oscap_destruct_func) xccdf_instance_free);
		oscap_list_free(rr->fixes, (oscap_destruct_func) xccdf_fix_free);
		oscap_list_free(rr->checks, (oscap_destruct_func) xccdf_check_free);

		free(rr);
	}
}

OSCAP_ACCESSOR_SIMPLE(xccdf_role_t, xccdf_rule_result, role)
OSCAP_ACCESSOR_SIMPLE(float, xccdf_rule_result, weight)
OSCAP_ACCESSOR_SIMPLE(xccdf_level_t, xccdf_rule_result, severity)
OSCAP_ACCESSOR_SIMPLE(xccdf_test_result_type_t, xccdf_rule_result, result)
OSCAP_ACCESSOR_STRING(xccdf_rule_result, time)
OSCAP_ACCESSOR_STRING(xccdf_rule_result, version)
OSCAP_ACCESSOR_STRING(xccdf_rule_result, idref)
OSCAP_IGETINS(xccdf_ident, xccdf_rule_result, idents, ident)
OSCAP_IGETINS(xccdf_fix, xccdf_rule_result, fixes, fix)
OSCAP_IGETINS(xccdf_check, xccdf_rule_result, checks, check)
OSCAP_IGETINS_GEN(xccdf_override, xccdf_rule_result, overrides, override)
OSCAP_ITERATOR_REMOVE_F(xccdf_override)
OSCAP_IGETINS_GEN(xccdf_message, xccdf_rule_result, messages, message)
OSCAP_ITERATOR_REMOVE_F(xccdf_message)
OSCAP_IGETINS_GEN(xccdf_instance, xccdf_rule_result, instances, instance)
OSCAP_ITERATOR_REMOVE_F(xccdf_instance)
OSCAP_ITERATOR_GEN(xccdf_rule_result)
OSCAP_ITERATOR_REMOVE_F(xccdf_rule_result)


struct xccdf_identity *xccdf_identity_new(void)
{
	return calloc(1, sizeof(struct xccdf_identity));
}

void xccdf_identity_free(struct xccdf_identity *identity)
{
	if (identity != NULL) {
		free(identity->name);
		free(identity);
	}
}

OSCAP_ACCESSOR_EXP(bool, xccdf_identity, authenticated, sub.authenticated)
OSCAP_ACCESSOR_EXP(bool, xccdf_identity, privileged, sub.privileged)
OSCAP_ACCESSOR_STRING(xccdf_identity, name)
OSCAP_ITERATOR_GEN(xccdf_identity)
OSCAP_ITERATOR_REMOVE_F(xccdf_identity)

struct xccdf_score *xccdf_score_new(void)
{
	struct xccdf_score *score = calloc(1, sizeof(struct xccdf_score));
	score->score = NAN;
	score->maximum = XCCDF_SCORE_MAX_DAFAULT;
	return score;
}

void xccdf_score_free(struct xccdf_score *score)
{
	if (score != NULL) {
		free(score->system);
		free(score);
	}
}

OSCAP_ACCESSOR_SIMPLE(xccdf_numeric, xccdf_score, maximum)
OSCAP_ACCESSOR_SIMPLE(xccdf_numeric, xccdf_score, score)
OSCAP_ACCESSOR_STRING(xccdf_score, system)
OSCAP_ITERATOR_GEN(xccdf_score)
OSCAP_ITERATOR_REMOVE_F(xccdf_score)

struct xccdf_override *xccdf_override_new(void)
{
	return calloc(1, sizeof(struct xccdf_override));
}

void xccdf_override_free(struct xccdf_override *oride)
{
	if (oride != NULL) {
		free(oride->authority);
		oscap_text_free(oride->remark);
		free(oride);
	}
}

OSCAP_ACCESSOR_SIMPLE(xccdf_test_result_type_t, xccdf_override, new_result)
OSCAP_ACCESSOR_SIMPLE(xccdf_test_result_type_t, xccdf_override, old_result)
OSCAP_ACCESSOR_STRING(xccdf_override, time)
OSCAP_ACCESSOR_STRING(xccdf_override, authority)
OSCAP_ACCESSOR_TEXT(xccdf_override, remark)

struct xccdf_message *xccdf_message_new(void)
{
    return calloc(1, sizeof(struct xccdf_message));
}

void xccdf_message_free(struct xccdf_message *msg)
{
    if (msg != NULL) {
        free(msg->content);
        free(msg);
    }
}

bool xccdf_message_set_content(struct xccdf_message *obj, const char *newval){
	// in debug mode, we handle all new messages in case where the xccdf_message is never displayed
	dD("XCCDF_SET_MESSAGE: %s", newval);
	free(obj->content);
	obj->content=oscap_strdup(newval);
	return true;
}

OSCAP_ACCESSOR_SIMPLE(xccdf_message_severity_t, xccdf_message, severity)
OSCAP_GETTER(const char*, xccdf_message, content)

struct xccdf_target_fact* xccdf_target_fact_new(void)
{
    return calloc(1, sizeof(struct xccdf_target_fact));
}

void xccdf_target_fact_free(struct xccdf_target_fact *fact)
{
    if (fact != NULL) {
        free(fact->name);
        free(fact->value);
        free(fact);
    }
}

static inline bool xccdf_target_fact_set_value(struct xccdf_target_fact *fact, xccdf_value_type_t type, const char *str)
{
    assert(fact != NULL);
    free(fact->value);
    fact->value = oscap_strdup(str);
    fact->type  = type;
    return true;
}

bool xccdf_target_fact_set_string(struct xccdf_target_fact *fact, const char *str)
{
    return xccdf_target_fact_set_value(fact, XCCDF_TYPE_STRING, str);
}

bool xccdf_target_fact_set_number(struct xccdf_target_fact *fact, xccdf_numeric val)
{
    char buff[XCCDF_NUMERIC_SIZE];
    sprintf(buff, XCCDF_NUMERIC_FORMAT, val);
    return xccdf_target_fact_set_value(fact, XCCDF_TYPE_NUMBER, buff);
}

bool xccdf_target_fact_set_boolean(struct xccdf_target_fact *fact, bool val)
{
    return xccdf_target_fact_set_value(fact, XCCDF_TYPE_BOOLEAN, oscap_enum_to_string(OSCAP_BOOL_MAP, val));
}

OSCAP_GETTER(xccdf_value_type_t, xccdf_target_fact, type)
OSCAP_GETTER(const char *, xccdf_target_fact, value)
OSCAP_ACCESSOR_STRING(xccdf_target_fact, name)
OSCAP_ITERATOR_GEN(xccdf_target_fact)
OSCAP_ITERATOR_REMOVE_F(xccdf_target_fact)

struct xccdf_target_identifier* xccdf_target_identifier_new(void)
{
    return calloc(1, sizeof(struct xccdf_target_identifier));
}

struct xccdf_target_identifier* xccdf_target_identifier_clone(const struct xccdf_target_identifier* ti)
{
	struct xccdf_target_identifier* ret = xccdf_target_identifier_new();

	if (xccdf_target_identifier_get_xml_node(ti)) {
		xccdf_target_identifier_set_xml_node(ret, xccdf_target_identifier_get_xml_node(ti));
	}
	else {
		xccdf_target_identifier_set_system(ret, xccdf_target_identifier_get_system(ti));
		xccdf_target_identifier_set_name(ret, xccdf_target_identifier_get_name(ti));
		xccdf_target_identifier_set_href(ret, xccdf_target_identifier_get_href(ti));
	}

	return ret;
}

void xccdf_target_identifier_free(struct xccdf_target_identifier *ti)
{
    if (ti != NULL) {
    	if (ti->any_element) {
    		xmlFreeNode(ti->element);
    	}
		else {
			free(ti->system);
			free(ti->href);
			free(ti->name);
        }

		free(ti);
    }
}

bool xccdf_target_identifier_set_xml_node(struct xccdf_target_identifier *ti, void* node)
{
	if (!ti->any_element) {
		free(ti->system);
		free(ti->href);
		free(ti->name);
	}
	else if (ti->element)
		xmlFreeNode(ti->element);

	ti->any_element = true;
	ti->element = (xmlNodePtr)node;

	return true;
}

void* xccdf_target_identifier_get_xml_node(const struct xccdf_target_identifier* ti)
{
	if (!ti->any_element)
		return NULL;

	return ti->element;
}

bool xccdf_target_identifier_set_system(struct xccdf_target_identifier *ti, const char *newval)
{
	if (ti->any_element) {
		if (ti->element)
			xmlFreeNode(ti->element);
	}

	ti->any_element = false;
	free(ti->system);
	ti->system = oscap_strdup(newval);

	return true;
}

const char* xccdf_target_identifier_get_system(const struct xccdf_target_identifier* ti)
{
	if (ti->any_element)
		return NULL;

	return ti->system;
}

bool xccdf_target_identifier_set_href(struct xccdf_target_identifier *ti, const char *newval)
{
	if (ti->any_element) {
		if (ti->element)
			xmlFreeNode(ti->element);
	}

	ti->any_element = false;
	free(ti->href);
	ti->href = oscap_strdup(newval);

	return true;
}

const char* xccdf_target_identifier_get_href(const struct xccdf_target_identifier* ti)
{
	if (ti->any_element)
		return NULL;

	return ti->href;
}

bool xccdf_target_identifier_set_name(struct xccdf_target_identifier *ti, const char *newval)
{
	if (ti->any_element) {
		if (ti->element)
			xmlFreeNode(ti->element);
	}

	ti->any_element = false;
	free(ti->name);
	ti->name = oscap_strdup(newval);

	return true;
}

const char* xccdf_target_identifier_get_name(const struct xccdf_target_identifier* ti)
{
	if (ti->any_element)
		return NULL;

	return ti->name;
}

OSCAP_ITERATOR_GEN(xccdf_target_identifier)
OSCAP_ITERATOR_REMOVE_F(xccdf_target_identifier)

struct xccdf_instance *xccdf_instance_new(void)
{
    struct xccdf_instance *inst = calloc(1, sizeof(struct xccdf_instance));
    inst->context = oscap_strdup(XCCDF_INSTANCE_DEFAULT_CONTEXT);
    return inst;
}

void xccdf_instance_free(struct xccdf_instance *inst)
{
    if (inst != NULL) {
        free(inst->context);
        free(inst->parent_context);
        free(inst->content);
        free(inst);
    }
}

OSCAP_ACCESSOR_STRING(xccdf_instance, context)
OSCAP_ACCESSOR_STRING(xccdf_instance, parent_context)
OSCAP_ACCESSOR_STRING(xccdf_instance, content)

/* ------ string maps ------ */

const struct oscap_string_map XCCDF_FACT_TYPE_MAP[] = {
	{ XCCDF_TYPE_BOOLEAN, "boolean" },
	{ XCCDF_TYPE_STRING,  "string"  },
	{ XCCDF_TYPE_NUMBER,  "number"  },
	{ XCCDF_TYPE_BOOLEAN, NULL      }
};

const struct oscap_string_map XCCDF_RESULT_MAP[] = {
	{ XCCDF_RESULT_PASS,           "pass"          },
	{ XCCDF_RESULT_FAIL,           "fail"          },
	{ XCCDF_RESULT_ERROR,          "error"         },
	{ XCCDF_RESULT_UNKNOWN,        "unknown"       },
	{ XCCDF_RESULT_NOT_APPLICABLE, "notapplicable" },
	{ XCCDF_RESULT_NOT_CHECKED,    "notchecked"    },
	{ XCCDF_RESULT_NOT_SELECTED,   "notselected"   },
	{ XCCDF_RESULT_INFORMATIONAL,  "informational" },
	{ XCCDF_RESULT_FIXED,          "fixed"         },
	{ 0, NULL }
};

/* --------- import -------- */

static struct xccdf_identity *xccdf_identity_new_parse(xmlTextReaderPtr reader);
static struct xccdf_target_fact *xccdf_target_fact_new_parse(xmlTextReaderPtr reader);
static struct xccdf_target_identifier *xccdf_target_identifier_new_parse(xmlTextReaderPtr reader);
static struct xccdf_score *xccdf_score_new_parse(xmlTextReaderPtr reader);
static struct xccdf_rule_result *xccdf_rule_result_new_parse(xmlTextReaderPtr reader);
static struct xccdf_override *xccdf_override_new_parse(xmlTextReaderPtr reader);
static struct xccdf_message *xccdf_message_new_parse(xmlTextReaderPtr reader);
static struct xccdf_instance *xccdf_instance_new_parse(xmlTextReaderPtr reader);

struct xccdf_result *xccdf_result_import_source(struct oscap_source *source)
{
	xmlTextReader *reader = oscap_source_get_xmlTextReader(source);
	if (reader == NULL) {
		return NULL;
	}
	while (xmlTextReaderRead(reader) == 1
			&& xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT);
	struct xccdf_result *result = xccdf_result_new_parse(reader);
	xmlFreeTextReader(reader);
	return result;
}

struct xccdf_result *xccdf_result_new_parse(xmlTextReaderPtr reader)
{
	assert(reader != NULL);
	if (xccdf_element_get(reader) != XCCDFE_TESTRESULT) {
		oscap_seterr(OSCAP_EFAMILY_XCCDF, "Expected 'TestResult' element while found '%s'.",
				xmlTextReaderConstLocalName(reader));
		return NULL;
	}

	struct xccdf_item *res = XITEM(xccdf_result_new());
	xccdf_result_set_schema_version(XRESULT(res), xccdf_detect_version_parser(reader));

	if (!xccdf_item_process_attributes(res, reader))
		goto fail;

	/* end time attribute is mandatory */
	if (!xccdf_attribute_has(reader, XCCDFA_END_TIME)) {
		oscap_seterr(OSCAP_EFAMILY_XCCDF, "Missing end-time attribute in TestResult element: %s", xccdf_item_get_id(res));
		goto fail;
	}
	res->sub.result.end_time = xccdf_attribute_copy(reader, XCCDFA_END_TIME);
	res->sub.result.start_time = xccdf_attribute_copy(reader, XCCDFA_START_TIME);

	res->item.version = xccdf_attribute_copy(reader, XCCDFA_VERSION);
	res->sub.result.test_system = xccdf_attribute_copy(reader, XCCDFA_TEST_SYSTEM);

	int depth = oscap_element_depth(reader) + 1;

	while (oscap_to_start_element(reader, depth)) {
		switch (xccdf_element_get(reader)) {
		case XCCDFE_ORGANIZATION:
			oscap_list_add(res->sub.result.organizations, oscap_element_string_copy(reader));
			break;
		case XCCDFE_IDENTITY:
			oscap_list_add(res->sub.result.identities, xccdf_identity_new_parse(reader));
			break;
		case XCCDFE_RESULT_PROFILE:
			if (res->sub.result.profile == NULL)
				res->sub.result.profile = xccdf_attribute_copy(reader, XCCDFA_IDREF);
			break;
		case XCCDFE_TARGET:
			oscap_list_add(res->sub.result.targets, oscap_element_string_copy(reader));
			break;
		case XCCDFE_TARGET_ADDRESS:
			oscap_list_add(res->sub.result.target_addresses, oscap_element_string_copy(reader));
			break;
		case XCCDFE_TARGET_FACTS:
			// we have to read inside the target-facts element
			xmlTextReaderRead(reader);

			// iterate over all inner elements of the target-facts element
			while (oscap_to_start_element(reader, depth + 1)) {
				if (xccdf_element_get(reader) == XCCDFE_FACT)
					oscap_list_add(res->sub.result.target_facts, xccdf_target_fact_new_parse(reader));
				xmlTextReaderRead(reader);
			}
			break;
		case XCCDFE_TARGET_IDENTIFIER:
			oscap_list_add(res->sub.result.target_id_refs, xccdf_target_identifier_new_parse(reader));
			break;
		case XCCDFE_SET_VALUE:
			oscap_list_add(res->sub.result.setvalues, xccdf_setvalue_new_parse(reader));
			break;
		case XCCDFE_RULE_RESULT:
			oscap_list_add(res->sub.result.rule_results, xccdf_rule_result_new_parse(reader));
			break;
		case XCCDFE_SCORE:
			oscap_list_add(res->sub.result.scores, xccdf_score_new_parse(reader));
			break;
		case XCCDFE_RESULT_BENCHMARK:
			if (res->sub.result.benchmark_uri == NULL)
				res->sub.result.benchmark_uri = xccdf_attribute_copy(reader, XCCDFA_HREF);
			break;
		default:
			if (!xccdf_item_process_element(res, reader))
				dW("Encountered an unknown element '%s' while parsing XCCDF result.",
				   xmlTextReaderConstLocalName(reader));
			break;

		// TODO: any element from other namespace is supposed to go into xccdf_target_identifier
		}
		xmlTextReaderRead(reader);
	}

	return XRESULT(res);

fail:
	xccdf_result_free(XRESULT(res));
	return NULL;
}

struct oscap_source *xccdf_result_export_source(struct xccdf_result *result, const char *filepath)
{
	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	if (doc == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return NULL;
	}

	xccdf_result_to_dom(result, NULL, doc, NULL, false);
	return oscap_source_new_from_xmlDoc(doc, filepath);
}

struct oscap_source *xccdf_result_stig_viewer_export_source(struct xccdf_result *result, const char *filepath)
{
	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	if (doc == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return NULL;
	}

	xccdf_result_to_dom(result, NULL, doc, NULL, true);
	return oscap_source_new_from_xmlDoc(doc, filepath);
}

void xccdf_result_to_dom(struct xccdf_result *result, xmlNode *result_node, xmlDoc *doc, xmlNode *parent, bool use_stig_rule_id)
{
        xmlNs *ns_xccdf = NULL;
	struct xccdf_benchmark *associated_benchmark = xccdf_result_get_benchmark(result);
	const char *benchmark_ref_uri = xccdf_result_get_benchmark_uri(result);
	const struct xccdf_version_info* version_info = xccdf_item_get_schema_version(XITEM(result));
	if (parent) {
	        ns_xccdf = lookup_xccdf_ns(doc, parent, version_info);
	} else {
		if (!result_node) result_node = xccdf_item_to_dom(XITEM(result), doc, NULL, version_info);
                ns_xccdf = xmlNewNs(result_node,
                		(const xmlChar*)xccdf_version_info_get_namespace_uri(version_info), NULL);
		xmlDocSetRootElement(doc, result_node);

		// TestResult is the root element, we have to provide reference to
		// the benchmark that was the source of this test result

		if (benchmark_ref_uri == NULL) {
			oscap_seterr(OSCAP_EFAMILY_XCCDF,
				"Exporting TestResult as root element with no benchmark URI available. "
				"Specification requires us to provide a reference to the Benchmark in this "
				"case, this will result in XML file that won't be valid!");
		}
	}

	if (benchmark_ref_uri) {
		// We need to prepend this previous to the "title" element
		// to satisfy the specification.

		xmlNodePtr title_node = NULL;
		xmlNodePtr title_node_candidate = result_node->children;
		while (title_node_candidate) {
			if (title_node_candidate->type == XML_ELEMENT_NODE &&
				strcmp((const char*)title_node_candidate->name, "title") == 0) {
				title_node = title_node_candidate;
				break;
			}

			title_node_candidate = title_node_candidate->next;
		}

		if (title_node == NULL) {
			oscap_seterr(OSCAP_EFAMILY_XCCDF,
				"Can't find title node in the exported TestResult. This is most likely "
				"an internal issue with openscap, please report the bug!");
		}
		else {
			xmlNodePtr benchmark_ref = xmlNewNode(ns_xccdf, BAD_CAST "benchmark");
			xmlNewProp(benchmark_ref, BAD_CAST "href", BAD_CAST benchmark_ref_uri);

			// @id is disallowed in XCCDF 1.1 and optional in XCCDF 1.2
			if (xccdf_version_cmp(xccdf_item_get_schema_version(XITEM(result)), "1.2") >= 0 && associated_benchmark) {
				xmlNewProp(benchmark_ref, BAD_CAST "id", BAD_CAST xccdf_benchmark_get_id(associated_benchmark));
			}

			xmlAddPrevSibling(title_node, benchmark_ref);
		}
	}

	/* Handle attributes */
	const char * start = xccdf_result_get_start_time(result);
	if (start) {
		xmlNewProp(result_node, BAD_CAST "start-time", BAD_CAST start);
	}
	xmlNewProp(result_node, BAD_CAST "end-time", BAD_CAST xccdf_result_get_end_time(result));
	const char *version = xccdf_result_get_version(result);
        if (version != NULL)
		xmlNewProp(result_node, BAD_CAST "version", BAD_CAST version);
	const char *test_system = xccdf_result_get_test_system(result);
	if (test_system != NULL) {
		xmlNewProp(result_node, BAD_CAST "test-system", BAD_CAST test_system);
	}

	/* Handle children */
	xccdf_texts_to_dom(xccdf_result_get_remarks(result), result_node, "remark");

	struct oscap_string_iterator *orgs = xccdf_result_get_organizations(result);
	while (oscap_string_iterator_has_more(orgs)) {
		const char *org = oscap_string_iterator_next(orgs);
		xmlNewTextChild(result_node, ns_xccdf, BAD_CAST "organization", BAD_CAST org);
	}
	oscap_string_iterator_free(orgs);

	struct xccdf_identity_iterator *identities = xccdf_result_get_identities(result);
	while (xccdf_identity_iterator_has_more(identities)) {
		struct xccdf_identity *identity = xccdf_identity_iterator_next(identities);
		xmlNode *identity_node = xmlNewTextChild(result_node, ns_xccdf, BAD_CAST "identity", BAD_CAST xccdf_identity_get_name(identity));
		
		if (xccdf_identity_get_authenticated(identity))
			xmlNewProp(identity_node, BAD_CAST "authenticated", BAD_CAST "true");
		else
			xmlNewProp(identity_node, BAD_CAST "authenticated", BAD_CAST "false");

		if (xccdf_identity_get_privileged(identity))
			xmlNewProp(identity_node, BAD_CAST "privileged", BAD_CAST "true");
		else
			xmlNewProp(identity_node, BAD_CAST "privileged", BAD_CAST "false");
	}
	xccdf_identity_iterator_free(identities);

	const char *profile = xccdf_result_get_profile(result);
	if (profile) {
		xmlNode *prof_node = xmlNewTextChild(result_node, ns_xccdf, BAD_CAST "profile", NULL);
		xmlNewProp(prof_node, BAD_CAST "idref", BAD_CAST profile);
	}

	struct oscap_string_iterator *targets = xccdf_result_get_targets(result);
	while (oscap_string_iterator_has_more(targets)) {
		const char *target = oscap_string_iterator_next(targets);
		xmlNewTextChild(result_node, ns_xccdf, BAD_CAST "target", BAD_CAST target);
	}
	oscap_string_iterator_free(targets);

	struct oscap_string_iterator *target_addresses = xccdf_result_get_target_addresses(result);
	while (oscap_string_iterator_has_more(target_addresses)) {
		const char *target_address = oscap_string_iterator_next(target_addresses);
		if (strchr(target_address, ':') != NULL) { // a very simplistic check for IPv6
			char *expanded_ipv6 = oscap_expand_ipv6(target_address);
			// Not all but a lot of datastream or even XCCDF test result schematrons
			// require a fully expanded IPv6. It doesn't hurt anything to expand it
			// here. All tools should be able to consume both shorthand and
			// expanded IPv6 addresses.

			xmlNewTextChild(result_node, ns_xccdf, BAD_CAST "target-address", BAD_CAST expanded_ipv6);
			free(expanded_ipv6);
		}
		else {
			xmlNewTextChild(result_node, ns_xccdf, BAD_CAST "target-address", BAD_CAST target_address);
		}
	}
	oscap_string_iterator_free(target_addresses);

    // platform

	struct xccdf_target_fact_iterator *target_facts = xccdf_result_get_target_facts(result);
        if (xccdf_target_fact_iterator_has_more(target_facts)) {
                xmlNode *target_node = xmlNewTextChild(result_node, ns_xccdf, BAD_CAST "target-facts", NULL);
                while (xccdf_target_fact_iterator_has_more(target_facts)) {
                        struct xccdf_target_fact *target_fact = xccdf_target_fact_iterator_next(target_facts);
                        xmlNode *fact_node = xmlNewTextChild(target_node, ns_xccdf, BAD_CAST "fact", BAD_CAST target_fact->value);

                        const char *name = xccdf_target_fact_get_name(target_fact);
                        if (name)
                                xmlNewProp(fact_node, BAD_CAST "name", BAD_CAST name);

                        xccdf_value_type_t value = xccdf_target_fact_get_type(target_fact);
                        xmlNewProp(fact_node, BAD_CAST "type", BAD_CAST XCCDF_FACT_TYPE_MAP[value - 1].string);
                }
        }
        xccdf_target_fact_iterator_free(target_facts);

	struct xccdf_target_identifier_iterator *target_id_refs = xccdf_result_get_target_id_refs(result);
	while (xccdf_target_identifier_iterator_has_more(target_id_refs)) {
		struct xccdf_target_identifier *target_identifier = xccdf_target_identifier_iterator_next(target_id_refs);
		xccdf_target_identifier_to_dom(target_identifier, doc, result_node, version_info);
	}
	xccdf_target_identifier_iterator_free(target_id_refs);

	struct oscap_string_iterator *applicable_platforms = xccdf_result_get_applicable_platforms(result);
	while (oscap_string_iterator_has_more(applicable_platforms)) {
		const char *platform = oscap_string_iterator_next(applicable_platforms);
		xmlNode *platform_element = xmlNewTextChild(result_node, ns_xccdf, BAD_CAST "platform", NULL);
		xmlNewProp(platform_element, BAD_CAST "idref", BAD_CAST platform);
	}
	oscap_string_iterator_free(applicable_platforms);

	struct xccdf_setvalue_iterator *setvalues = xccdf_result_get_setvalues(result);
	while (xccdf_setvalue_iterator_has_more(setvalues)) {
		struct xccdf_setvalue *setvalue = xccdf_setvalue_iterator_next(setvalues);
		xccdf_setvalue_to_dom(setvalue, doc, result_node, version_info);
	}
	xccdf_setvalue_iterator_free(setvalues);

	struct oscap_htable *nodes_by_rule_id = oscap_htable_new();

	struct xccdf_rule_result_iterator *rule_results = xccdf_result_get_rule_results(result);
	if (use_stig_rule_id) {
		while (xccdf_rule_result_iterator_has_more(rule_results)) {
			struct xccdf_rule_result *rule_result = xccdf_rule_result_iterator_next(rule_results);

			const char *idref = xccdf_rule_result_get_idref(rule_result);
			if (!idref)
				continue;

			xccdf_test_result_type_t test_res = xccdf_rule_result_get_result(rule_result);

			const struct xccdf_item *item = xccdf_benchmark_get_member(associated_benchmark, XCCDF_RULE, idref);
			if (!item)
				continue;

			struct oscap_reference_iterator *references = xccdf_item_get_references(item);
			while (oscap_reference_iterator_has_more(references)) {
				struct oscap_reference *ref = oscap_reference_iterator_next(references);
				const char *href = oscap_reference_get_href(ref);
				if (href && (strcmp(href, DISA_STIG_VIEWER_HREF[0]) == 0 ||
							strcmp(href, DISA_STIG_VIEWER_HREF[1]) == 0 ||
							strcmp(href, DISA_STIG_VIEWER_HREF[2]) == 0)) {
					const char *stig_rule_id = oscap_reference_get_title(ref);

					xccdf_test_result_type_t other_res = (xccdf_test_result_type_t)oscap_htable_detach(nodes_by_rule_id, stig_rule_id);
					xccdf_test_result_type_t wanted_res;
					if (other_res == 0) {
						wanted_res = test_res;
					} else {
						// if one test passed, and the other didn't, the other one should win
						if (test_res == XCCDF_RESULT_PASS) {
							wanted_res = other_res;
						} else if (other_res == XCCDF_RESULT_PASS) {
							wanted_res = test_res;
						// if one had an error, that should win
						} else if (test_res == XCCDF_RESULT_ERROR || other_res == XCCDF_RESULT_ERROR) {
							wanted_res = XCCDF_RESULT_ERROR;
						// next prio: failures
						} else if (test_res == XCCDF_RESULT_FAIL || other_res == XCCDF_RESULT_FAIL) {
							wanted_res = XCCDF_RESULT_FAIL;
						// next prio: unknown
						} else if (test_res == XCCDF_RESULT_UNKNOWN || other_res == XCCDF_RESULT_UNKNOWN) {
							wanted_res = XCCDF_RESULT_UNKNOWN;
						// otherwise, just pick the lower one (more or less arbitrarily)
						} else {
							wanted_res = (test_res < other_res) ? test_res : other_res;
						}
					}
					oscap_htable_add(nodes_by_rule_id, stig_rule_id, (void*)wanted_res);
				}
			}
			oscap_reference_iterator_free(references);
		}
		xccdf_rule_result_iterator_reset(rule_results);
	}
	while (xccdf_rule_result_iterator_has_more(rule_results)) {
		struct xccdf_rule_result *rule_result = xccdf_rule_result_iterator_next(rule_results);
		xccdf_rule_result_to_dom(rule_result, doc, result_node, version_info, associated_benchmark, use_stig_rule_id, nodes_by_rule_id);
	}
	xccdf_rule_result_iterator_free(rule_results);

	oscap_htable_free0(nodes_by_rule_id);

	struct xccdf_score_iterator *scores = xccdf_result_get_scores(result);
	while (xccdf_score_iterator_has_more(scores)) {
		struct xccdf_score *score = xccdf_score_iterator_next(scores);

                float value = xccdf_score_get_score(score);
                char *value_str = oscap_sprintf("%f", value);
                xmlNode *score_node = xmlNewTextChild(result_node, ns_xccdf, BAD_CAST "score", BAD_CAST value_str);
                free(value_str);

		const char *sys = xccdf_score_get_system(score);
		if (sys)
			xmlNewProp(score_node, BAD_CAST "system", BAD_CAST sys);

		float max = xccdf_score_get_maximum(score);
		char *max_str = oscap_sprintf("%f", max);
		xmlNewProp(score_node, BAD_CAST "maximum", BAD_CAST max_str);
        free(max_str);
	}
	xccdf_score_iterator_free(scores);
}

static struct xccdf_identity *xccdf_identity_new_parse(xmlTextReaderPtr reader)
{
	XCCDF_ASSERT_ELEMENT(reader, XCCDFE_IDENTITY);

	struct xccdf_identity *identity = xccdf_identity_new();
	identity->sub.authenticated = xccdf_attribute_get_bool(reader, XCCDFA_AUTHENTICATED);
	identity->sub.privileged    = xccdf_attribute_get_bool(reader, XCCDFA_PRIVILEDGED);
	identity->name              = oscap_element_string_copy(reader);
	return identity;
}

static struct xccdf_target_fact *xccdf_target_fact_new_parse(xmlTextReaderPtr reader)
{
	XCCDF_ASSERT_ELEMENT(reader, XCCDFE_FACT);

	struct xccdf_target_fact *fact = xccdf_target_fact_new();
	fact->type = oscap_string_to_enum(XCCDF_FACT_TYPE_MAP, xccdf_attribute_get(reader, XCCDFA_TYPE));
	fact->name = xccdf_attribute_copy(reader, XCCDFA_NAME);
	fact->value = oscap_element_string_copy(reader);
	return fact;
}

static struct xccdf_target_identifier *xccdf_target_identifier_new_parse(xmlTextReaderPtr reader)
{
	struct xccdf_target_identifier *ret = xccdf_target_identifier_new();

	if (xccdf_element_get(reader) == XCCDFE_TARGET_IDENTIFIER) {
		xccdf_target_identifier_set_system(ret, xccdf_attribute_get(reader, XCCDFA_SYSTEM));
		xccdf_target_identifier_set_href(ret, xccdf_attribute_get(reader, XCCDFA_HREF));
		xccdf_target_identifier_set_name(ret, xccdf_attribute_get(reader, XCCDFA_NAME));
	}
	else {
		// this is OK because we clone the node in the setter
		xccdf_target_identifier_set_xml_node(ret, xmlTextReaderCurrentNode(reader));
	}

	return ret;
}

static struct xccdf_score *xccdf_score_new_parse(xmlTextReaderPtr reader)
{
	XCCDF_ASSERT_ELEMENT(reader, XCCDFE_SCORE);

	struct xccdf_score *score = xccdf_score_new();
	score->system = xccdf_attribute_copy(reader, XCCDFA_SYSTEM);
	if (xccdf_attribute_has(reader, XCCDFA_MAXIMUM))
		score->maximum = xccdf_attribute_get_float(reader, XCCDFA_MAXIMUM);
	else score->maximum = XCCDF_SCORE_MAX_DAFAULT;
	score->score = atof(oscap_element_string_get(reader));
	return score;
}

static struct xccdf_rule_result *xccdf_rule_result_new_parse(xmlTextReaderPtr reader)
{
	XCCDF_ASSERT_ELEMENT(reader, XCCDFE_RULE_RESULT);

	struct xccdf_rule_result *rr = xccdf_rule_result_new();

	rr->idref    = xccdf_attribute_copy(reader, XCCDFA_IDREF);
	rr->role     = oscap_string_to_enum(XCCDF_ROLE_MAP, xccdf_attribute_get(reader, XCCDFA_ROLE));
	rr->time     = xccdf_attribute_copy(reader, XCCDFA_TIME);
	rr->version  = xccdf_attribute_copy(reader, XCCDFA_VERSION);
	rr->weight   = xccdf_attribute_get_float(reader, XCCDFA_WEIGHT);
	rr->severity = oscap_string_to_enum(XCCDF_LEVEL_MAP, xccdf_attribute_get(reader, XCCDFA_SEVERITY));

	int depth = oscap_element_depth(reader) + 1;

	while (oscap_to_start_element(reader, depth)) {
		switch (xccdf_element_get(reader)) {
		case XCCDFE_RESULT:
			rr->result = oscap_string_to_enum(XCCDF_RESULT_MAP, oscap_element_string_get(reader));
			break;
		case XCCDFE_OVERRIDE:
			oscap_list_add(rr->overrides, xccdf_override_new_parse(reader));
			break;
		case XCCDFE_IDENT:
			oscap_list_add(rr->idents, xccdf_ident_parse(reader));
			break;
		case XCCDFE_MESSAGE:
			oscap_list_add(rr->messages, xccdf_message_new_parse(reader));
			break;
		case XCCDFE_INSTANCE:
			oscap_list_add(rr->instances, xccdf_instance_new_parse(reader));
			break;
		case XCCDFE_FIX:
			oscap_list_add(rr->fixes, xccdf_fix_parse(reader));
			break;
		case XCCDFE_CHECK:
			oscap_list_add(rr->checks, xccdf_check_parse(reader));
			break;
		default: break;
		}
		xmlTextReaderRead(reader);
	}

	return rr;
}

xmlNode *xccdf_target_identifier_to_dom(const struct xccdf_target_identifier *ti, xmlDoc *doc, xmlNode *parent, const struct xccdf_version_info* version_info)
{
	if (ti->any_element) {
		// we have to copy since we are adding the node to an existing document
		// and that document will take ownership
		xmlNodePtr ret = xmlCopyNode(ti->element, 1);
		xmlAddChild(parent, ret);
		return ret;
	}
	else {
		xmlNs *ns_xccdf = lookup_xccdf_ns(doc, parent, version_info);

		xmlNode *target_idref_node = xmlNewTextChild(parent, ns_xccdf, BAD_CAST "target-id-ref", NULL);

		/* Handle attributes */
		const char *psystem = xccdf_target_identifier_get_system(ti);
		if (psystem)
			xmlNewProp(target_idref_node, BAD_CAST "system", BAD_CAST psystem);

		const char *href = xccdf_target_identifier_get_href(ti);
		if (href)
			xmlNewProp(target_idref_node, BAD_CAST "href", BAD_CAST href);

		const char *name = xccdf_target_identifier_get_name(ti);
		if (name)
			xmlNewProp(target_idref_node, BAD_CAST "name", BAD_CAST name);

		return target_idref_node;
	}
}
static void _xccdf_rule_result_to_dom_idref(struct xccdf_rule_result *result, xmlDoc *doc, xmlNode *parent, const struct xccdf_version_info* version_info, struct xccdf_benchmark *benchmark, const char *idref);
void xccdf_rule_result_to_dom(struct xccdf_rule_result *result, xmlDoc *doc, xmlNode *parent, const struct xccdf_version_info* version_info, struct xccdf_benchmark *benchmark, bool use_stig_rule_id, struct oscap_htable *nodes_by_rule_id)
{
	const char *idref = xccdf_rule_result_get_idref(result);
	if (use_stig_rule_id) {
		// Don't output rules with no stig ids
		if (!idref || !benchmark)
			return;

		const struct xccdf_item *item = xccdf_benchmark_get_member(benchmark, XCCDF_RULE, idref);
		if (!item)
			return;

		struct oscap_reference_iterator *references = xccdf_item_get_references(item);
		while (oscap_reference_iterator_has_more(references)) {
			struct oscap_reference *ref = oscap_reference_iterator_next(references);
			const char *href = oscap_reference_get_href(ref);
			if (href && (strcmp(href, DISA_STIG_VIEWER_HREF[0]) == 0 ||
						strcmp(href, DISA_STIG_VIEWER_HREF[1]) == 0 ||
						strcmp(href, DISA_STIG_VIEWER_HREF[2]) == 0)) {
				const char *stig_rule_id = oscap_reference_get_title(ref);

				xccdf_test_result_type_t expected_res = (xccdf_test_result_type_t)oscap_htable_get(nodes_by_rule_id, stig_rule_id);
				xccdf_test_result_type_t test_res = xccdf_rule_result_get_result(result);
				if (expected_res == test_res) {
					oscap_htable_detach(nodes_by_rule_id, stig_rule_id);
					_xccdf_rule_result_to_dom_idref(result, doc, parent, version_info, benchmark, stig_rule_id);
				}
			}
		}
		oscap_reference_iterator_free(references);
	} else {
		_xccdf_rule_result_to_dom_idref(result, doc, parent, version_info, benchmark, idref);
	}
}
static void _xccdf_rule_result_to_dom_idref(struct xccdf_rule_result *result, xmlDoc *doc, xmlNode *parent, const struct xccdf_version_info* version_info, struct xccdf_benchmark *benchmark, const char *idref) {
	xmlNs *ns_xccdf = lookup_xccdf_ns(doc, parent, version_info);

	xmlNode *result_node = xmlNewTextChild(parent, ns_xccdf, BAD_CAST "rule-result", NULL);

	/* Handle attributes */
	if (idref)
		xmlNewProp(result_node, BAD_CAST "idref", BAD_CAST idref);

	xccdf_role_t role = xccdf_rule_result_get_role(result);
	if (role != 0)
            xmlNewProp(result_node, BAD_CAST "role", BAD_CAST XCCDF_ROLE_MAP[role - 1].string);

	const char * tm = xccdf_rule_result_get_time(result);
	if (tm) {
		xmlNewProp(result_node, BAD_CAST "time", BAD_CAST tm);
	}

	xccdf_level_t severity = xccdf_rule_result_get_severity(result);
	if (severity != XCCDF_LEVEL_NOT_DEFINED)
            xmlNewProp(result_node, BAD_CAST "severity", BAD_CAST XCCDF_LEVEL_MAP[severity - 1].string);

	const char *version = xccdf_rule_result_get_version(result);
	if (version)
		xmlNewProp(result_node, BAD_CAST "version", BAD_CAST version);

	float weight = xccdf_rule_result_get_weight(result);
	char *weight_str = oscap_sprintf("%f", weight);
	xmlNewProp(result_node, BAD_CAST "weight", BAD_CAST weight_str);
    free(weight_str);

	/* Handle children */
	xccdf_test_result_type_t test_res = xccdf_rule_result_get_result(result);
	if (test_res != 0)
            xmlNewTextChild(result_node, ns_xccdf, BAD_CAST "result", BAD_CAST XCCDF_RESULT_MAP[test_res - 1].string);

	struct xccdf_override_iterator *overrides = xccdf_rule_result_get_overrides(result);
	while (xccdf_override_iterator_has_more(overrides)) {
		struct xccdf_override *override = xccdf_override_iterator_next(overrides);
		xccdf_override_to_dom(override, doc, result_node, version_info);
	}
	xccdf_override_iterator_free(overrides);

	struct xccdf_ident_iterator *idents = xccdf_rule_result_get_idents(result);
	while (xccdf_ident_iterator_has_more(idents)) {
		struct xccdf_ident *ident = xccdf_ident_iterator_next(idents);
		xccdf_ident_to_dom(ident, doc, result_node, version_info);
	}
	xccdf_ident_iterator_free(idents);

	struct xccdf_message_iterator *messages = xccdf_rule_result_get_messages(result);
	while (xccdf_message_iterator_has_more(messages)) {
		struct xccdf_message *message = xccdf_message_iterator_next(messages);
		const char *content = xccdf_message_get_content(message);
		xmlChar *encoded_content = xmlEncodeEntitiesReentrant(doc, BAD_CAST content);
		xmlNode *message_node = xmlNewChild(result_node, ns_xccdf, BAD_CAST "message", encoded_content);
		xmlFree(encoded_content);

		xccdf_message_severity_t message_severity = xccdf_message_get_severity(message);
		if (message_severity != XCCDF_MSG_NOT_DEFINED)
                    xmlNewProp(message_node, BAD_CAST "severity", BAD_CAST XCCDF_LEVEL_MAP[message_severity - 1].string);
	}
	xccdf_message_iterator_free(messages);

	struct xccdf_instance_iterator *instances = xccdf_rule_result_get_instances(result);
	while (xccdf_instance_iterator_has_more(instances)) {
		struct xccdf_instance *instance = xccdf_instance_iterator_next(instances);
		const char *content = xccdf_instance_get_content(instance);
		xmlNode *instance_node = xmlNewTextChild(result_node, ns_xccdf, BAD_CAST "instance", BAD_CAST content);
		
		const char *context = xccdf_instance_get_context(instance);
		if (context)
			xmlNewProp(instance_node, BAD_CAST "context", BAD_CAST context); 

		const char *parent_context = xccdf_instance_get_context(instance);
		if (parent_context)
			xmlNewProp(instance_node, BAD_CAST "parentContext", BAD_CAST context); 
	}
	xccdf_instance_iterator_free(instances);

	struct xccdf_fix_iterator *fixes = xccdf_rule_result_get_fixes(result);
	while (xccdf_fix_iterator_has_more(fixes)) {
		struct xccdf_fix *fix = xccdf_fix_iterator_next(fixes);
		xccdf_fix_to_dom(fix, doc, result_node, version_info);
	}
	xccdf_fix_iterator_free(fixes);

	struct xccdf_check_iterator *checks = xccdf_rule_result_get_checks(result);
	while (xccdf_check_iterator_has_more(checks)) {
		struct xccdf_check *check = xccdf_check_iterator_next(checks);
		xccdf_check_to_dom(check, doc, result_node, version_info);
	}
	xccdf_check_iterator_free(checks);
}

bool xccdf_rule_result_override(struct xccdf_rule_result *rule_result, xccdf_test_result_type_t new_result, const char *waiver_time, const char *authority, struct oscap_text *remark)
{
	struct xccdf_override *o= xccdf_override_new();
	xccdf_override_set_old_result(o, xccdf_rule_result_get_result(rule_result));
	xccdf_override_set_new_result(o, new_result);
	xccdf_override_set_time(o, waiver_time);
	xccdf_override_set_authority(o, authority);
	xccdf_override_set_remark(o, remark);
	if (!xccdf_rule_result_add_override(rule_result, o)) {
		xccdf_override_free(o);
		return false;
	}
	return xccdf_rule_result_set_result(rule_result, new_result);
}

static struct xccdf_override *xccdf_override_new_parse(xmlTextReaderPtr reader)
{
	XCCDF_ASSERT_ELEMENT(reader, XCCDFE_OVERRIDE);

	struct xccdf_override *override = xccdf_override_new();

	override->time      = xccdf_attribute_copy(reader, XCCDFA_TIME);
	override->authority = xccdf_attribute_copy(reader, XCCDFA_AUTHORITY);

	int depth = oscap_element_depth(reader) + 1;

	while (oscap_to_start_element(reader, depth)) {
		switch (xccdf_element_get(reader)) {
		case XCCDFE_OLD_RESULT:
			override->old_result = oscap_string_to_enum(XCCDF_RESULT_MAP, oscap_element_string_get(reader));
			break;
		case XCCDFE_NEW_RESULT:
			override->new_result = oscap_string_to_enum(XCCDF_RESULT_MAP, oscap_element_string_get(reader));
			break;
		case XCCDFE_REMARK:
			if (override->remark == NULL)
				override->remark = oscap_text_new_parse(XCCDF_TEXT_PLAIN, reader);
			break;
		default: break;
		}
		xmlTextReaderRead(reader);
	}

	return override;
}

xmlNode *xccdf_override_to_dom(struct xccdf_override *override, xmlDoc *doc, xmlNode *parent, const struct xccdf_version_info* version_info)
{
	xmlNs *ns_xccdf = lookup_xccdf_ns(doc, parent, version_info);

	xmlNode *override_node = xmlNewTextChild(parent, ns_xccdf, BAD_CAST "override", NULL);

	/* Handle attributes */
	const char *tm = xccdf_override_get_time(override);
	if(tm) {
		xmlNewProp(override_node, BAD_CAST "date", BAD_CAST tm);
	}

	const char *authority = xccdf_override_get_authority(override);
	if (authority)
		xmlNewProp(override_node, BAD_CAST "authority", BAD_CAST authority);

	/* Handle children */
	xccdf_test_result_type_t old = xccdf_override_get_old_result(override);
	if (old != 0)
		xmlNewTextChild(override_node, ns_xccdf, BAD_CAST "old-result", BAD_CAST XCCDF_RESULT_MAP[old - 1].string);

	xccdf_test_result_type_t new = xccdf_override_get_new_result(override);
	if (new != 0)
		xmlNewTextChild(override_node, ns_xccdf, BAD_CAST "new-result", BAD_CAST XCCDF_RESULT_MAP[new - 1].string);

	oscap_text_to_dom(xccdf_override_get_remark(override), override_node, "remark");

	return override_node;
}

static struct xccdf_message *xccdf_message_new_parse(xmlTextReaderPtr reader)
{
	XCCDF_ASSERT_ELEMENT(reader, XCCDFE_MESSAGE);

	struct xccdf_message *msg = xccdf_message_new();
	msg->severity = oscap_string_to_enum(XCCDF_LEVEL_MAP, xccdf_attribute_get(reader, XCCDFA_SEVERITY));
	msg->content  = oscap_element_string_copy(reader);
	return msg;
}

static struct xccdf_instance *xccdf_instance_new_parse(xmlTextReaderPtr reader)
{
	XCCDF_ASSERT_ELEMENT(reader, XCCDFE_INSTANCE);

	struct xccdf_instance *inst = xccdf_instance_new();
	if (xccdf_attribute_has(reader, XCCDFA_CONTEXT))
		xccdf_instance_set_context(inst, xccdf_attribute_get(reader, XCCDFA_CONTEXT));
	inst->parent_context = xccdf_attribute_copy(reader, XCCDFA_PARENTCONTEXT);
	inst->content        = oscap_element_string_copy(reader);
	return inst;
}

const char * xccdf_test_result_type_get_text(xccdf_test_result_type_t id)
{
    if (id == 0) return NULL;

    return XCCDF_RESULT_MAP[id - 1].string;
}

struct xccdf_rule_result * xccdf_result_get_rule_result_by_id(struct xccdf_result * result, const char * id)
{
        struct xccdf_rule_result * rule_result = NULL;
    
        struct xccdf_rule_result_iterator * rr_it = xccdf_result_get_rule_results(result);
        while (xccdf_rule_result_iterator_has_more(rr_it)) {
            rule_result = xccdf_rule_result_iterator_next(rr_it);
            if (oscap_streq(xccdf_rule_result_get_idref(rule_result), id))
                break;
            else rule_result = NULL;
        }
        xccdf_rule_result_iterator_free(rr_it);

        return rule_result;
}

/* --------  DUMP ---------- */

static void xccdf_rule_result_dump(struct xccdf_rule_result *res, int depth)
{
	xccdf_print_depth(depth); printf("Rule result: %s\n", xccdf_rule_result_get_idref(res)); ++depth;
	xccdf_print_depth(depth); printf("version:   %s\n", xccdf_rule_result_get_version(res));
	xccdf_print_depth(depth); printf("weight:    %f\n", xccdf_rule_result_get_weight(res));
}

void xccdf_result_dump(struct xccdf_result *res, int depth)
{
	xccdf_print_depth(depth); printf("TestResult : %s\n", (res ? xccdf_result_get_id(res) : "(NULL)"));
	if (res != NULL) {
		++depth; xccdf_item_print(XITEM(res), depth);
		xccdf_print_depth(depth); printf("test system:   %s\n", xccdf_result_get_test_system(res));
		xccdf_print_depth(depth); printf("benchmark URI: %s\n", xccdf_result_get_benchmark_uri(res));
		xccdf_print_depth(depth); printf("profile ID:    %s\n", xccdf_result_get_profile(res));
		// start time, end time...
		//xccdf_print_depth(depth); printf("identities");
		//oscap_list_dump(XITEM(res)->result.identities, NULL, depth+1);
		xccdf_print_depth(depth); printf("targets");
		oscap_list_dump(XITEM(res)->sub.result.targets, xccdf_cstring_dump, depth+1);
		xccdf_print_depth(depth); printf("organizations");
		oscap_list_dump(XITEM(res)->sub.result.organizations, xccdf_cstring_dump, depth+1);
		xccdf_print_depth(depth); printf("remarks");
		xccdf_print_textlist(xccdf_result_get_remarks(res), depth+1, 80, "...");
		xccdf_print_depth(depth); printf("target addresses");
		oscap_list_dump(XITEM(res)->sub.result.target_addresses, xccdf_cstring_dump, depth+1);
		//xccdf_print_depth(depth); printf("target_facts");
		//xccdf_print_depth(depth); printf("target_id_refs");
		xccdf_print_depth(depth); printf("setvalues");
		oscap_list_dump(XITEM(res)->sub.result.setvalues, xccdf_setvalue_dump, depth+1);
		xccdf_print_depth(depth); printf("rule results");
		oscap_list_dump(XITEM(res)->sub.result.rule_results, (oscap_dump_func) xccdf_rule_result_dump, depth+1);
		//xccdf_print_depth(depth); printf("scores");
	}
}

/*
 * @returns pointer to text representation of the current local time
 * and NULL on failure. The string is statically allocated and might
 * be overwritten on subsequent calls to this function
 */
static inline const char *_get_timestamp(void)
{
	static char timestamp[] = "yyyy-mm-ddThh:mm:ss+zz:zz";
	time_t tm;
	struct tm *lt;
	int tz_diff;
	char tz_sign;

	tm = time(NULL);
	lt = localtime(&tm);

	if (!lt)
		return NULL;

#if defined(OS_FREEBSD)
	tz_diff = lt->tm_gmtoff;

	if (tz_diff < 0) {
		tz_sign = '-';
		tz_diff *= -1;
	} else {
		tz_sign = '+';
	}

        /*  glibc's timezone offset does not account for daylight savings time.
         *  So we match that behavior here by adding 3600 seconds
         */
        if (lt->tm_isdst)
		tz_diff += 3600;
#else
	/* timezone is a global variable set by localtime(3) */
	if (timezone <= 0) {
		tz_sign = '+';
		tz_diff = -timezone;
	} else {
		tz_sign = '-';
		tz_diff = timezone;
	}
#endif
	tz_diff /= 60;

	int ret = snprintf(timestamp, sizeof(timestamp), "%4d-%02d-%02dT%02d:%02d:%02d%c%02d:%02d",
		1900 + lt->tm_year, 1 + lt->tm_mon, lt->tm_mday,
		lt->tm_hour, lt->tm_min, lt->tm_sec, tz_sign, tz_diff / 60, tz_diff % 60);

	if (ret < 0) {
		return NULL;
	}

	return timestamp;
}

#define XCCDF_CURRENT_TIME_SETTER(TYPE, BOUND) \
	int xccdf_##TYPE##_set##BOUND##time_current(struct xccdf_##TYPE *r) \
	{ return xccdf_##TYPE##_set##BOUND##time(r, _get_timestamp()); }

XCCDF_CURRENT_TIME_SETTER(rule_result,_)
XCCDF_CURRENT_TIME_SETTER(result,_start_)
XCCDF_CURRENT_TIME_SETTER(result,_end_)
