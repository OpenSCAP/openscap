/*
 * Copyright 2008 Red Hat Inc., Durham, North Carolina.
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
#  include <arpa/inet.h>
#  include <netlink/route/link.h>
#  include <netlink/route/addr.h>

#  ifdef HAVE_LIBNL10
#    define nl_cache_free nl_cache_destroy_and_free
#  endif

struct cbstate_s {
	SEXP_t *item_list;
	SEXP_t *name_ent;
	struct nl_cache *link_cache;
};

static void cb(struct nl_object *obj, void *arg)
{
	struct cbstate_s *cbstate = (struct cbstate_s *) arg;
	struct rtnl_addr *rtaddr = (struct rtnl_addr *) obj;
	struct rtnl_link *rtlink;
	int ifindex;
	char *name = NULL;
	SEXP_t *sname;

	ifindex = rtnl_addr_get_ifindex(rtaddr);
	name = rtnl_addr_get_label(rtaddr);
	rtlink = rtnl_link_get(cbstate->link_cache, ifindex);
	if (name == NULL)
		name = rtnl_link_get_name(rtlink);
	sname = SEXP_string_newf("%s", name);

	if (probe_entobj_cmp(cbstate->name_ent, sname) == OVAL_RESULT_TRUE) {
		struct nl_addr *absaddr;
		SEXP_t *item, *r0, *r1, *r2, *r3, *r4;
		unsigned int prefixlen, mask, flags;
		char iabuf[64], mabuf[20], babuf[64], netmask[20], fbuf[512], *p;

		memset(mabuf, 0, sizeof (mabuf));
		absaddr = rtnl_link_get_addr(rtlink);
		if (absaddr && !nl_addr_iszero(absaddr))
			nl_addr2str(absaddr, mabuf, sizeof (mabuf));

		memset(iabuf, 0, sizeof (iabuf));
		absaddr = rtnl_addr_get_local(rtaddr);
		if (absaddr && !nl_addr_iszero(absaddr)) {
			nl_addr2str(absaddr, iabuf, sizeof (iabuf));
			if ((p = strchr(iabuf, '/')))
				*p = '\0';
		}

		memset(babuf, 0, sizeof (babuf));
		absaddr = rtnl_addr_get_broadcast(rtaddr);
		if (absaddr && !nl_addr_iszero(absaddr))
			nl_addr2str(absaddr, babuf, sizeof (babuf));

		memset(netmask, 0, sizeof (netmask));
		prefixlen = rtnl_addr_get_prefixlen(rtaddr);
		mask = (~0) << (32 - prefixlen);
		snprintf(netmask, sizeof (netmask), "%u.%u.%u.%u",
			 (unsigned char) (mask >> 24),
			 (unsigned char) (mask >> 16),
			 (unsigned char) (mask >> 8),
			 (unsigned char) mask);

		item = probe_item_creat("interface_item", NULL,
					"name", NULL, sname,
					/* unix-system-characteristics-schema v5.6
					"type", NULL,
					r0 = SEXP_string_newf("%s", type),
					*/
					"hardware_addr", NULL,
					r1 = SEXP_string_newf("%s", mabuf),
					"inet_addr", NULL,
					r2 = SEXP_string_newf("%s", iabuf),
					"broadcast_addr", NULL,
					r3 = SEXP_string_newf("%s", babuf),
					"netmask", NULL,
					r4 = SEXP_string_newf("%s", netmask),
					NULL);
		SEXP_vfree(r1, r2, r3, r4, sname, NULL);

		flags = rtnl_link_get_flags(rtlink);
		rtnl_link_flags2str(flags, fbuf, sizeof (fbuf));
		p = fbuf;
		do {
			char *flag;

			flag = p;
			p = strchr(p, ',');
			if (p)
				*p++ = '\0';

			probe_item_ent_add(item, "flag", NULL,
					   r0 = SEXP_string_newf("%s", flag));
			SEXP_free(r0);
		} while (p);

		rtnl_link_put(rtlink);
		SEXP_list_add(cbstate->item_list, item);
		SEXP_free(item);
	}
}

static void get_ifs(SEXP_t *name_ent, SEXP_t *item_list)
{
	struct cbstate_s cbstate;
	struct nl_handle *sock;
	struct nl_cache *addr_cache;

	cbstate.item_list = item_list;
	cbstate.name_ent = name_ent;
	sock = nl_handle_alloc();
	nl_connect(sock, NETLINK_ROUTE);
	cbstate.link_cache = rtnl_link_alloc_cache(sock);
	addr_cache = rtnl_addr_alloc_cache(sock);

	nl_cache_foreach(addr_cache, cb, (void *) &cbstate);

	nl_cache_free(cbstate.link_cache);
	nl_cache_free(addr_cache);
	nl_close(sock);
	nl_handle_destroy(sock);
}
#else
static void get_ifs(SEXP_t *name_ent, SEXP_t *item_list)
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
	get_ifs(name_ent, probe_out);

	*err = 0;
	return probe_out;
}
