/**
 * @file   interface_probe.c
 * @brief  interface probe
 * @author "Tomas Heinrich" <theinric@redhat.com>
 * @author "Steve Grubb" <sgrubb@redhat.com>
 *
 * 2010/06/13 dkopecek@redhat.com
 *  This probe is able to process an interface_object as defined in OVAL 5.4 and 5.5.
 *
 */

/*
 * Copyright 2009,2010 Red Hat Inc., Durham, North Carolina.
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
 *      "Steve Grubb"    <sgrubb@redhat.com>
 */

/*
 * interface probe
 *
 *   reference:
 *     http://oval.mitre.org/language/version5.6/ovaldefinition/documentation/unix-definitions-schema.html#interface_object
 *     http://oval.mitre.org/language/version5.6/ovalsc/documentation/unix-system-characteristics-schema.html#interface_item
 *
 *   object:
 *     name oval-def:EntityObjectStringType 1 1
 *
 *   item:
 *     name           oval-sc:EntityItemStringType    0 1
 *     type           unix-sc:EntityItemInterfaceType 0 1
 *     hardware_addr  oval-sc:EntityItemStringType    0 1
 *     inet_addr      oval-sc:EntityItemStringType    0 1
 *     broadcast_addr oval-sc:EntityItemStringType    0 1
 *     netmask        oval-sc:EntityItemStringType    0 1
 *     flag           oval-sc:EntityItemStringType    0 unbounded
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>

#include <probe-api.h>
#include <probe/entcmp.h>

#if defined(__linux__)
#include <unistd.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <string.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <arpa/inet.h>
#include "interface_probe.h"

static int fd=-1;

static void get_l2_info(const struct ifaddrs *ifa, char **mp, char **tp)
{
	struct ifreq ifr;
	unsigned char mac[6];
	static char mac_buf[20];

	*mp = mac_buf;
	*tp = "";

	memset(&ifr, 0, sizeof(struct ifreq));
	strcpy(ifr.ifr_name, ifa->ifa_name);
	if (ioctl(fd, SIOCGIFHWADDR, &ifr) >= 0) {
		memcpy(mac, ifr.ifr_hwaddr.sa_data, sizeof(mac));
		snprintf(mac_buf, sizeof(mac_buf),
			"%02X:%02X:%02X:%02X:%02X:%02X",
			mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		switch (ifr.ifr_hwaddr.sa_family) {
		case ARPHRD_ETHER:
			*tp = "ARPHRD_ETHER";
			break;
		case ARPHRD_FDDI:
			*tp = "ARPHRD_FDDI";
			break;
		case ARPHRD_LOOPBACK:
			*tp = "ARPHRD_LOOPBACK";
			break;
		case ARPHRD_PPP:
			*tp = "ARPHRD_PPP";
			break;
		case ARPHRD_PRONET:
			*tp = "ARPHRD_PRONET";
			break;
		case ARPHRD_SLIP:
			*tp = "ARPHRD_SLIP";
			break;
		case ARPHRD_VOID:
			*tp = "ARPHRD_VOID";
			break;
		}
	} else
		mac_buf[0] = 0;
}


static void get_flags(const struct ifaddrs *ifa, char ***fp) {
	static char *flags_buf[17];
	int i = 0;

	*fp = flags_buf;

	if (ifa != NULL) {
		/* follow values from net/if.h */
		if (ifa->ifa_flags & IFF_UP) {
			flags_buf[i] = "UP";
			i++;
		}
		if (ifa->ifa_flags & IFF_BROADCAST) {
			flags_buf[i] = "BROADCAST";
			i++;
		}
		if (ifa->ifa_flags & IFF_DEBUG) {
			flags_buf[i] = "DEBUG";
			i++;
		}
		if (ifa->ifa_flags & IFF_LOOPBACK) {
			flags_buf[i] = "LOOPBACK";
			i++;
		}
		if (ifa->ifa_flags & IFF_POINTOPOINT) {
			flags_buf[i] = "POINTOPOINT";
			i++;
		}
		if (ifa->ifa_flags & IFF_NOTRAILERS) {
			flags_buf[i] = "NOTRAILERS";
			i++;
		}
		if (ifa->ifa_flags & IFF_RUNNING) {
			flags_buf[i] = "RUNNING";
			i++;
		}
		if (ifa->ifa_flags & IFF_NOARP) {
			flags_buf[i] = "NOAPP";
			i++;
		}
		if (ifa->ifa_flags & IFF_PROMISC) {
			flags_buf[i] = "PROMISC";
			i++;
		}
		if (ifa->ifa_flags & IFF_ALLMULTI) {
			flags_buf[i] = "ALLMULTI";
			i++;
		}
		if (ifa->ifa_flags & IFF_MASTER) {
			flags_buf[i] = "MASTER";
			i++;
		}
		if (ifa->ifa_flags & IFF_SLAVE) {
			flags_buf[i] = "SLAVE";
			i++;
		}
		if (ifa->ifa_flags & IFF_MULTICAST) {
			flags_buf[i] = "MULTICAST";
			i++;
		}
		if (ifa->ifa_flags & IFF_PORTSEL) {
			flags_buf[i] = "PORTSEL";
			i++;
		}
		if (ifa->ifa_flags & IFF_AUTOMEDIA) {
			flags_buf[i] = "AUTOMEDIA";
			i++;
		}
		if (ifa->ifa_flags & IFF_DYNAMIC) {
			flags_buf[i] = "DYNAMIC";
			i++;
		}
	}
	flags_buf[i] = NULL;

}

static int get_ifs(SEXP_t *name_ent, probe_ctx *ctx, oval_schema_version_t over)
{
	struct ifaddrs *ifaddr, *ifa;
	int family, rc=1;
	char host[NI_MAXHOST], broad[NI_MAXHOST], mask[NI_MAXHOST], *mac, *type, **flags;
	oval_datatype_t address_type;
	SEXP_t *item;
	bool include_type, use_ipstring;

	if (getifaddrs(&ifaddr) == -1) {
		SEXP_t *msg;

		msg = probe_msg_creat(OVAL_MESSAGE_LEVEL_ERROR, "getifaddrs() failed.");
		probe_cobj_add_msg(probe_ctx_getresult(ctx), msg);
		SEXP_free(msg);
		probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_ERROR);

		return rc;
	}

	fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (fd < 0) {
		SEXP_t *msg;

		msg = probe_msg_creat(OVAL_MESSAGE_LEVEL_ERROR, "socket() failed.");
		probe_cobj_add_msg(probe_ctx_getresult(ctx), msg);
		SEXP_free(msg);
		probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_ERROR);

		goto leave1;
	}

	include_type = oval_schema_version_cmp(over, OVAL_SCHEMA_VERSION(5.6)) >= 0;
	use_ipstring = oval_schema_version_cmp(over, OVAL_SCHEMA_VERSION(5.8)) >= 0;

        /* Walk through linked list, maintaining head pointer so we
	   can free list later */
	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		SEXP_t *sname;

		if (!ifa->ifa_addr)
			continue;

		family = ifa->ifa_addr->sa_family;
		if (family != AF_INET && family != AF_INET6)
			continue;

		sname = SEXP_string_newf("%s", ifa->ifa_name);
		if (probe_entobj_cmp(name_ent, sname) != OVAL_RESULT_TRUE) {
			SEXP_free(sname);
			continue;
		}
		SEXP_free(sname);

		get_l2_info(ifa, &mac, &type);
		get_flags(ifa, &flags);

/* The inet_addr entity is the IP address of the specific interface.
 * Note that the IP address can be IPv4 or IPv6. If the IP address is an IPv6 address,
 * this entity should be expressed as an IPv6 address prefix using CIDR notation and
 * the netmask entity should not be collected.
 */
		if (family == AF_INET) {
			rc = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
					host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
			if (rc) {
				SEXP_t *msg;

				msg = probe_msg_creat(OVAL_MESSAGE_LEVEL_ERROR, "getnameinfo() failed.");
				probe_cobj_add_msg(probe_ctx_getresult(ctx), msg);
				SEXP_free(msg);
				probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_ERROR);
				rc = 1;

				goto leave2;
			}
			address_type = use_ipstring ? OVAL_DATATYPE_IPV4ADDR : OVAL_DATATYPE_STRING;

			rc = getnameinfo(ifa->ifa_netmask, sizeof(struct sockaddr_in),
					mask, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
			if (rc) {
				SEXP_t *msg;

				msg = probe_msg_creat(OVAL_MESSAGE_LEVEL_ERROR, "getnameinfo() failed.");
				probe_cobj_add_msg(probe_ctx_getresult(ctx), msg);
				SEXP_free(msg);
				probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_ERROR);
				rc = 1;

				goto leave2;
			}

		}
		else {
			struct sockaddr_in6 *sin6p;
			char host_tmp[NI_MAXHOST];
			int bit, byte, prefix = 0;
			u_int32_t tmp;

			sin6p = (struct sockaddr_in6 *) ifa->ifa_addr;

			if (! inet_ntop(family, (const void *)&sin6p->sin6_addr, host, NI_MAXHOST)) {
				SEXP_t *msg;

				msg = probe_msg_creat(OVAL_MESSAGE_LEVEL_ERROR, "inet_ntop() failed.");
				probe_cobj_add_msg(probe_ctx_getresult(ctx), msg);
				SEXP_free(msg);
				probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_ERROR);
				rc = 1;

				goto leave2;
			}

			address_type = use_ipstring ? OVAL_DATATYPE_IPV6ADDR : OVAL_DATATYPE_STRING;
			*mask = '\0';

			/* count prefix */
			sin6p = (struct sockaddr_in6 *) ifa->ifa_netmask;

			for (byte = 0; byte < 4 && sin6p->sin6_addr.s6_addr32[byte] == 0xffffffff; byte++) {
			        prefix += 32;
			}
			if (byte < 4) {
			        tmp = ntohl(sin6p->sin6_addr.s6_addr32[byte]);
			        for (bit = 31; tmp & (1 << bit); bit--)
			                prefix++;
			}
			snprintf(host_tmp, NI_MAXHOST, "%s/%d", host, prefix);
			strncpy(host, host_tmp, NI_MAXHOST);
		}

		if (family == AF_INET && ifa->ifa_flags & IFF_BROADCAST) {
			rc = getnameinfo(ifa->ifa_broadaddr, (family == AF_INET) ?
					 sizeof(struct sockaddr_in) :
					 sizeof(struct sockaddr_in6), broad, NI_MAXHOST,
					 NULL, 0, NI_NUMERICHOST);
			if (rc) {
				SEXP_t *msg;

				msg = probe_msg_creat(OVAL_MESSAGE_LEVEL_ERROR, "getnameinfo() failed.");
				probe_cobj_add_msg(probe_ctx_getresult(ctx), msg);
				SEXP_free(msg);
				probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_ERROR);
				rc = 1;

				goto leave2;
			}
		} else
			*broad = '\0';

                item = probe_item_create(OVAL_UNIX_INTERFACE, NULL,
                                         "name",           OVAL_DATATYPE_STRING, ifa->ifa_name,
					"type",           OVAL_DATATYPE_STRING, include_type ? type : NULL,
                                         "hardware_addr",  OVAL_DATATYPE_STRING, mac,
                                         "inet_addr",      address_type, host,
                                         "broadcast_addr", address_type, broad,
                                         "netmask",        address_type, mask,
                                         "flag",          OVAL_DATATYPE_STRING_M, flags,
                                         NULL);

                probe_item_collect(ctx, item);
	}

	rc = 0;
leave2:
	close(fd);
leave1:
	freeifaddrs(ifaddr);
	return rc;
}
#else
static int get_ifs(SEXP_t *name_ent, probe_ctx *ctx, oval_schema_version_t over)
{
	/* todo */
	SEXP_t *item;

        item = probe_item_create(OVAL_UNIX_INTERFACE, NULL,
                                 "name",           OVAL_DATATYPE_STRING, "dummy0"
                                 "hardware_addr",  OVAL_DATATYPE_STRING, "aa:bb:cc:dd:ee:ff",
                                 "inet_addr",      OVAL_DATATYPE_STRING, "1.2.3.4",
                                 "broadcast_addr", OVAL_DATATYPE_STRING, "1.2.3.254",
                                 "netmask",        OVAL_DATATYPE_STRING, "255.255.255.0",
                                 NULL);

        probe_item_collect(ctx, item);

	return (0);
}
#endif

int interface_probe_main(probe_ctx *ctx, void *arg)
{
	SEXP_t *name_ent, *probe_in;
	oval_schema_version_t over;

        (void)arg;

	probe_in = probe_ctx_getobject(ctx);
	name_ent = probe_obj_getent(probe_in, "name", 1);
	over     = probe_obj_get_platform_schema_version(probe_in);

	if (name_ent == NULL) {
		return PROBE_ENOELM;
	}

	get_ifs(name_ent, ctx, over);
	SEXP_free(name_ent);

	return 0;
}
