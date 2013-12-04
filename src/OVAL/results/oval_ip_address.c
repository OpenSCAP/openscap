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
#include <arpa/inet.h>

#include "oval_definitions.h"
#include "oval_types.h"

#include "common/util.h"
#include "common/debug_priv.h"
#include "results/oval_ip_address_impl.h"

static int ipv4addr_parse(char *oval_ipv4_string, int *netmask_out, struct in_addr *ip_out)
{
	char *s, *pfx;
	int result = -1;

	s = strdup(oval_ipv4_string);
	pfx = strchr(s, '/');
	if (pfx) {
		int cnt;
		char nm[4];

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
		dW("inet_pton() failed.\n");
	else
		result = 0;

	oscap_free(s);
	return result;
}

oval_result_t ipv4addr_cmp(char *s1, char *s2, oval_operation_t op)
{
	oval_result_t result = OVAL_RESULT_ERROR;
	int nm1, nm2;
	struct in_addr addr1, addr2;

	if (ipv4addr_parse(s1, &nm1, &addr1) || ipv4addr_parse(s2, &nm2, &addr2)) {
		return result;
	}

	switch (op) {
	case OVAL_OPERATION_EQUALS:
		if (!memcmp(&addr1, &addr2, sizeof(struct in_addr)) && nm1 == nm2)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_NOT_EQUAL:
		if (memcmp(&addr1, &addr2, sizeof(struct in_addr)) || nm1 != nm2)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_SUBSET_OF:
		if (nm1 <= nm2) {
			result = OVAL_RESULT_FALSE;
			break;
		}
		/* FALLTHROUGH */
	case OVAL_OPERATION_GREATER_THAN:
		addr1.s_addr &= nm1;
		addr2.s_addr &= nm2;
		if (memcmp(&addr1, &addr2, sizeof(struct in_addr)) > 0)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_GREATER_THAN_OR_EQUAL:
		addr1.s_addr &= nm1;
		addr2.s_addr &= nm2;
		if (memcmp(&addr1, &addr2, sizeof(struct in_addr)) >= 0)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_SUPERSET_OF:
		if (nm1 >= nm2) {
			result = OVAL_RESULT_FALSE;
			break;
		}
		/* FALLTHROUGH */
	case OVAL_OPERATION_LESS_THAN:
		addr1.s_addr &= nm1;
		addr2.s_addr &= nm2;
		if (memcmp(&addr1, &addr2, sizeof(struct in_addr)) < 0)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_LESS_THAN_OR_EQUAL:
		addr1.s_addr &= nm1;
		addr2.s_addr &= nm2;
		if (memcmp(&addr1, &addr2, sizeof(struct in_addr)) <= 0)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	default:
		dE("Unexpected compare operation: %d.\n", op);
	}

	return result;
}

static int ipv6addr_parse(char *oval_ipv6_string, int *len_out, struct in6_addr *ip_out)
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
		dW("inet_pton() failed.\n");
	else
		result = 0;

	oscap_free(s);
	return result;
}

static void mask_v6_addrs(struct in6_addr *addr1, int p1len, struct in6_addr *addr2, int p2len)
{
	int i;

	for (i = 0; i < 128; ++i) {
		addr1->s6_addr[i / 8] &= (((i < p1len) ? 1 : 0) << (i % 8));
		addr2->s6_addr[i / 8] &= (((i < p2len) ? 1 : 0) << (i % 8));
	}
}

oval_result_t ipv6addr_cmp(char *s1, char *s2, oval_operation_t op)
{
	oval_result_t result = OVAL_RESULT_ERROR;
	int p1len, p2len;
	struct in6_addr addr1, addr2;

	if (ipv6addr_parse(s1, &p1len, &addr1) || ipv6addr_parse(s2, &p2len, &addr2)) {
		return result;
	}

	switch (op) {
	case OVAL_OPERATION_EQUALS:
		if (!memcmp(&addr1, &addr2, sizeof(struct in6_addr)) && p1len == p2len)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_NOT_EQUAL:
		if (memcmp(&addr1, &addr2, sizeof(struct in6_addr)) || p1len != p2len)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_SUBSET_OF:
		if (p1len <= p2len) {
			result = OVAL_RESULT_FALSE;
			break;
		}
		/* FALLTHROUGH */
	case OVAL_OPERATION_GREATER_THAN:
		mask_v6_addrs(&addr1, p1len, &addr2, p2len);
		if (memcmp(&addr1, &addr2, sizeof(struct in6_addr)) > 0)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_GREATER_THAN_OR_EQUAL:
		mask_v6_addrs(&addr1, p1len, &addr2, p2len);
		if (memcmp(&addr1, &addr2, sizeof(struct in6_addr)) >= 0)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_SUPERSET_OF:
		/* This asserts that every IP address in the set of IP addresses defined in
		 * the stated entity (addr1, p1len) is present in the set of IP addresses
		 * on the system (addr2, p2len). */
		if (p1len < p2len) {
			/* The lesser is the prefix-length of CIDR -> the more IP addresses
			 * there are in the range. */
			result = OVAL_RESULT_FALSE;
			break;
		}

		/* Otherwise, compare the first p2len (!) bytes. */
		mask_v6_addrs(&addr1, p2len, &addr2, p2len);
		if (memcmp(&addr1, &addr2, sizeof(struct in6_addr)) == 0)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_LESS_THAN:
		mask_v6_addrs(&addr1, p1len, &addr2, p2len);
		if (memcmp(&addr1, &addr2, sizeof(struct in6_addr)) < 0)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_LESS_THAN_OR_EQUAL:
		mask_v6_addrs(&addr1, p1len, &addr2, p2len);
		if (memcmp(&addr1, &addr2, sizeof(struct in6_addr)) <= 0)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	default:
		dE("Unexpected compare operation: %d.\n", op);
	}

	return result;
}

