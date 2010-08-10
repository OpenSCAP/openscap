/**
 * @file   interface.c
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
 * interface probe:
 *
 *   interface_object:
 *     string name
 *
 *   interface_item:
 *     string name
 *     interfacetype type
 *     string hardware_addr
 *     string inet_addr
 *     string broadcast_addr
 *     string netmask
 *     [0..*] string flag
 */

#include <config.h>
#include <stdio.h>

#include <probe-api.h>
#include <probe-entcmp.h>

#if defined(__linux__)
#include <unistd.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <string.h>
#include <net/if.h>

static int fd=-1;

static char *get_mac(const struct ifaddrs *ifa)
{
	struct ifreq ifr;
	unsigned char mac[6];
	static char mac_buf[20];

	memset(&ifr, 0, sizeof(struct ifreq));
	strcpy(ifr.ifr_name, ifa->ifa_name);
	if (ioctl(fd, SIOCGIFHWADDR, &ifr) >= 0) {
		memcpy(mac, ifr.ifr_hwaddr.sa_data, sizeof(mac));
		snprintf(mac_buf, sizeof(mac_buf),
			"%02X:%02X:%02X:%02X:%02X:%02X",
			mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	} else
		mac_buf[0] = 0;

	return mac_buf;
}

static int get_ifs(SEXP_t *name_ent, SEXP_t *item_list)
{
	struct ifaddrs *ifaddr, *ifa;
	int family, rc=1;
	char host[NI_MAXHOST], broad[NI_MAXHOST], mask[NI_MAXHOST], *mac;
	SEXP_t *item;
	SEXP_t *r0, *r1, *r2, *r3, *r4;

	if (getifaddrs(&ifaddr) == -1)
		return rc;

	fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (fd < 0)
		goto leave1;

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
		if (probe_entobj_cmp(name_ent, sname) != OVAL_RESULT_TRUE)
			continue;
		SEXP_free(sname);

		mac = get_mac(ifa);
		rc = getnameinfo(ifa->ifa_addr, (family == AF_INET) ?
			sizeof(struct sockaddr_in) :
			sizeof(struct sockaddr_in6), host, NI_MAXHOST,
			NULL, 0, NI_NUMERICHOST);
		if (rc) {
			rc = 1;
			goto leave2;
		}
		rc = getnameinfo(ifa->ifa_netmask, (family == AF_INET) ?
			sizeof(struct sockaddr_in) :
			sizeof(struct sockaddr_in6), mask, NI_MAXHOST,
			NULL, 0, NI_NUMERICHOST);
		if (rc) {
			rc = 1;
			goto leave2;
		}
		if (family == AF_INET && ifa->ifa_flags & IFF_BROADCAST) {
			rc = getnameinfo(ifa->ifa_broadaddr, (family == AF_INET) ?
					 sizeof(struct sockaddr_in) :
					 sizeof(struct sockaddr_in6), broad, NI_MAXHOST,
					 NULL, 0, NI_NUMERICHOST);
			if (rc) {
				rc = 1;
				goto leave2;
			}
		} else
			*broad = '\0';

		item = probe_item_creat("interface_item", NULL,
				"name", NULL,
                                r0 = SEXP_string_new (ifa->ifa_name, strlen (ifa->ifa_name)),
				"hardware_addr", NULL,
                                r1 = SEXP_string_new (mac, strlen (mac)),
				"inet_addr", NULL,
                                r2 = SEXP_string_new (host, strlen (host)),
				"broadcast_addr", NULL,
                                r3 = SEXP_string_new (broad, strlen (broad)),
				"netmask", NULL,
                                r4 = SEXP_string_new (mask, strlen (mask)),
				NULL);

		SEXP_list_add(item_list, item);
		SEXP_vfree(r0, r1, r2, r3, r4, item, NULL);
	}

	rc = 0;
leave2:
	close(fd);
leave1:
	freeifaddrs(ifaddr);
	return rc;
}
#else
static int get_ifs(SEXP_t *name_ent, SEXP_t *item_list)
{
	/* todo */

	SEXP_t *item;
	SEXP_t *r0, *r1, *r2, *r3, *r4;

	item = probe_item_creat("interface_item", NULL,
				"name", NULL,
				r0 = SEXP_string_newf("dummy0"),
				"hardware_addr", NULL,
				r1 = SEXP_string_newf("aa:bb:cc:dd:ee:ff"),
				"inet_addr", NULL,
				r2 = SEXP_string_newf("1.2.3.4"),
				"broadcast_addr", NULL,
				r3 = SEXP_string_newf("1.2.3.255"),
				"netmask", NULL,
				r4 = SEXP_string_newf("255.255.255.0"),
				NULL);

	SEXP_list_add(item_list, item);
	SEXP_vfree(r0, r1, r2, r3, r4, item, NULL);

	return (0);
}
#endif

SEXP_t *probe_main(SEXP_t *probe_in, int *err, void *arg)
{
	SEXP_t *probe_out;
	SEXP_t *name_ent;

	if (probe_in == NULL) {
		*err = PROBE_EINVAL;
		return NULL;
	}

	name_ent = probe_obj_getent(probe_in, "name", 1);
	if (name_ent == NULL) {
		*err = PROBE_ENOELM;
		return NULL;
	}

	probe_out = SEXP_list_new(NULL);
	if (get_ifs(name_ent, probe_out)) {
                SEXP_t *eitm;

                eitm = probe_item_creat ("interface_item", NULL, NULL);
                probe_item_setstatus (eitm, OVAL_STATUS_ERROR);
                SEXP_list_add(probe_out, eitm);
                SEXP_free(eitm);
	}

	*err = 0;
	return probe_out;
}
