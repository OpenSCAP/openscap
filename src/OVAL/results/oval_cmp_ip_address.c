/**
 * @file oval_resultTest.c
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2009--2013 Red Hat Inc., Durham, North Carolina.
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
 *     Tomas Heinrich <theinric@redhat.com>
 *     Šimon Lukašík
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#endif

#include "oval_definitions.h"
#include "oval_types.h"

#include "common/util.h"
#include "common/debug_priv.h"
#include "oval_cmp_ip_address_impl.h"

static inline void ipv4addr_mask(struct in_addr *ip_addr, uint32_t netmask);
static inline void ipv6addr_mask(struct in6_addr *addr, int prefix_len);
static inline int ipv4addr_parse(const char *oval_ipv4_string, uint32_t *netmask_out, struct in_addr *ip_out);
static inline int ipv6addr_parse(const char *oval_ipv6_string, uint32_t *len_out, struct in6_addr *ip_out);


static inline int ipaddr_cmp(int af, const void *addr1, const void *addr2)
{
	if (af == AF_INET)
		return memcmp(addr1, addr2, sizeof(struct in_addr));
	else {
		assert(af == AF_INET6);
		return memcmp(addr1, addr2, sizeof(struct in6_addr));
	}
}

static inline void ipaddr_mask(int af, const void *ip_addr, uint32_t mask)
{
	if (af == AF_INET)
		ipv4addr_mask((struct in_addr *) ip_addr, mask);
	else if (af == AF_INET6)
		ipv6addr_mask((struct in6_addr *) ip_addr, mask);
	else
		assert(false);

}

static inline int ipaddr_parse(int af, const char *oval_ip_string, uint32_t *mask_out, void * ip_out)
{
	if (af == AF_INET)
		return ipv4addr_parse(oval_ip_string, mask_out, ip_out);
	assert (af == AF_INET6);
	return ipv6addr_parse(oval_ip_string, mask_out, ip_out);
}

oval_result_t oval_ipaddr_cmp(int af, const char *s1, const char *s2, oval_operation_t op)
{
	oval_result_t result = OVAL_RESULT_ERROR;
	uint32_t mask1 = 0, mask2 = 0;
	char addr1[INET6_ADDRSTRLEN];
	char addr2[INET6_ADDRSTRLEN];

	if (ipaddr_parse(af, s1, &mask1, &addr1) || ipaddr_parse(af, s2, &mask2, &addr2)) {
		return result;
	}

	switch (op) {
	case OVAL_OPERATION_EQUALS:
		if (!ipaddr_cmp(af, &addr1, &addr2) && mask1 == mask2)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_NOT_EQUAL:
		if (ipaddr_cmp(af, &addr1, &addr2) || mask1 != mask2)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_SUBSET_OF:
		/* This asserts that every IP address in the set of IP addresses
		 * on the system (add2, mask2) must be present in the set of IP
		 * addresses defined in the stated entity (addr1, mask1). */
		if (mask1 > mask2) {
			/* The bigger the netmask (IPv4) or prefix-length (IPv6) is
			 * the less IP addresses there are in the range. */
			result = OVAL_RESULT_FALSE;
			break;
		}

		/* Otherwise, compare the first bits defined by mask1 */
		ipaddr_mask(af, &addr1, mask1);
		ipaddr_mask(af, &addr2, mask1);
		if (ipaddr_cmp(af, &addr1, &addr2) == 0)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_GREATER_THAN:
		if (mask1 != mask2) {
			return OVAL_RESULT_ERROR;
		}
		ipaddr_mask(af, &addr1, mask1);
		ipaddr_mask(af, &addr2, mask2);
		if (ipaddr_cmp(af, &addr1, &addr2) < 0)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_GREATER_THAN_OR_EQUAL:
		if (mask1 != mask2) {
			return OVAL_RESULT_ERROR;
		}
		ipaddr_mask(af, &addr1, mask1);
		ipaddr_mask(af, &addr2, mask2);
		if (ipaddr_cmp(af, &addr1, &addr2) <= 0)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_SUPERSET_OF:
		/* This asserts that every IP address in the set of IP addresses defined in
		 * the stated entity (addr1, mask1) is present in the set of IP addresses
		 * on the system. (addr2, mask2). */
		if (mask1 < mask2) {
			/* The smaller the netmask (IPv4) or prefix-length (IPv6) is
			 * the more IP addresses there are in the range */
			result = OVAL_RESULT_FALSE;
			break;
		}

		/* Otherwise, compare the first bits defined by mask2 */
		ipaddr_mask(af, &addr1, mask2);
		ipaddr_mask(af, &addr2, mask2);
		if (ipaddr_cmp(af, &addr1, &addr2) == 0)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_LESS_THAN:
		if (mask1 != mask2) {
			return OVAL_RESULT_ERROR;
		}
		ipaddr_mask(af, &addr1, mask1);
		ipaddr_mask(af, &addr2, mask2);
		if (ipaddr_cmp(af, &addr1, &addr2) > 0)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_LESS_THAN_OR_EQUAL:
		if (mask1 != mask2) {
			return OVAL_RESULT_ERROR;
		}
		ipaddr_mask(af, &addr1, mask1);
		ipaddr_mask(af, &addr2, mask2);
		if (ipaddr_cmp(af, &addr1, &addr2) >= 0)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	default:
		dE("Unexpected compare operation: %d.", op);
		assert(false);
	}

	return result;
}

static inline int ipv4addr_parse(const char *oval_ipv4_string, uint32_t *netmask_out, struct in_addr *ip_out)
{
	char *s, *pfx;
	int result = -1;

	s = strdup(oval_ipv4_string);
	pfx = strchr(s, '/');
	if (pfx) {
		int cnt;
		unsigned char nm[4];

		*pfx++ = '\0';
		cnt = sscanf(pfx, "%hhu.%hhu.%hhu.%hhu", &nm[0], &nm[1], &nm[2], &nm[3]);
		if (cnt > 1) { /* netmask */
			*netmask_out = (nm[0] << 24) + (nm[1] << 16) + (nm[2] << 8) + nm[3];
		} else { /* prefix */
			*netmask_out = (~0) << (32 - nm[0]);
		}
	} else {
		*netmask_out = ~0;
	}

	if (inet_pton(AF_INET, s, ip_out) <= 0)
		dW("inet_pton() failed.");
	else
		result = 0;

	free(s);
	return result;
}

static inline void ipv4addr_mask(struct in_addr *ip_addr, uint32_t netmask)
{
	ip_addr->s_addr &= htonl(netmask);
}

static inline int ipv6addr_parse(const char *oval_ipv6_string, uint32_t *len_out, struct in6_addr *ip_out)
{
	char *s, *pfx;
	int result = -1;

	s = strdup(oval_ipv6_string);
	pfx = strchr(s, '/');
	if (pfx) {
		*pfx++ = '\0';
		*len_out = strtol(pfx, NULL, 10);
	} else {
		*len_out = 128;
	}

	if (inet_pton(AF_INET6, s, ip_out) <= 0)
		dW("inet_pton() failed.");
	else
		result = 0;

	free(s);
	return result;
}

static inline void ipv6addr_mask(struct in6_addr *addr, int prefix_len)
{
	assert(128 >= prefix_len);

	uint8_t mask = (~0) << (8 - (prefix_len % 8));

	/* First n (prefix_len/8 - 1) bytes are left untouched. */
	for (int i = prefix_len/8; i < 128/8; i++)
	{
		/* The (n+1) byte is masked according to the prefix_len */
		addr->s6_addr[i] &= mask;
		/* The rest will be zeroed. */
		mask = 0;
	}
}
