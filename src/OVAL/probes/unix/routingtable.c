/**
 * @file   routingtable.c
 * @brief  routingtable probe
 * @author "Daniel Kopecek <dkopecek@redhat.com>"
 *
 */

/*
 * Copyright 2009-2011 Red Hat Inc., Durham, North Carolina.
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
 *   Steve Grubb <sgrubb@redhat.com>
 *   Daniel Kopecek <dkopecek@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "probe-api.h"
#include "probe/entcmp.h"
#include "alloc.h"
#include "util.h"

#if defined(HAVE_LIBNL)
#include <netinet/in.h>
#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/list.h>
#include <netlink/cache.h>
#include <netlink/route/nexthop.h>
#include <netlink/route/rtnl.h>
#include <netlink/route/route.h>
#include <netlink/route/link.h>
#include <net/route.h>

typedef struct {
	probe_ctx *ctx;
	SEXP_t    *in_dst;
        struct nl_cache *links;
} rt_cb_helper;

static void collect_item(struct nl_object *object, void *arg)
{
	rt_cb_helper *h = (rt_cb_helper *)arg;
	struct rtnl_route *route = nl_object_priv(object);

	SEXP_t *item, *out_dst;
	char   *flag_array[32];
	size_t  flag_count = 0;

	struct nl_addr *dst, *gw;

	uint32_t flags;
	char dst_str[INET6_ADDRSTRLEN + 1], *gw_str, gw_strmem[INET6_ADDRSTRLEN + 1];
        char if_name[128];
        int  if_index;

	dI("processing object: %p\n", object);

	/* get destination addr */
	dst = rtnl_route_get_dst(route);

	if (dst != NULL)
		nl_addr2str(dst, dst_str, sizeof dst_str);
	else {
		dE("rtnl_route_get_dst(%p) returned NULL!\n", route);
		return;
	}

	out_dst = SEXP_string_new(dst_str, strlen(dst_str));

	/* get gateway addr */
	gw = rtnl_route_get_gateway(route);

	if (gw != NULL)
		gw_str = nl_addr2str(gw, gw_strmem, sizeof gw_strmem);
	else
		gw_str = NULL;

	dI("dst: %s\n"
	   " gw: %s\n"
	   "  a: %p\n"
	   "  b: %p\n", dst_str, gw_str, h->in_dst, out_dst);

	if (probe_entobj_cmp(h->in_dst, out_dst) != OVAL_RESULT_TRUE) {
		SEXP_free(out_dst);
		return;
	}

        dI("match\n");

	/* construct the flag array */
	flags = rtnl_route_get_flags(route);

#define RTF_COND_ADD(flg, str) if (flags & (flg)) flag_array[flag_count++] = str

	RTF_COND_ADD(RTF_UP,        "UP");
	RTF_COND_ADD(RTF_REINSTATE, "REINSTATE");
	RTF_COND_ADD(RTF_MTU,       "MTU");
	RTF_COND_ADD(RTF_WINDOW,    "WINDOW");
	RTF_COND_ADD(RTF_REJECT,    "REJECT");
	RTF_COND_ADD(RTF_STATIC,    "STATIC");
	RTF_COND_ADD(RTF_ALLONLINK, "ALLONLINK");
	RTF_COND_ADD(RTF_ADDRCONF,  "ADDRCONF");
	RTF_COND_ADD(RTF_NONEXTHOP, "NONEXTHOP");
	RTF_COND_ADD(RTF_FLOW,      "FLOW");
	RTF_COND_ADD(RTF_POLICY,    "POLICY");

	flag_array[flag_count] = NULL;

        dI("flags      = %x\n", flags);
        dI("flag_count = %zu\n", flag_count);
        dI("flag_array[0] = %s\n", flag_array[0]);

        if_index = rtnl_route_get_oif(route);
        rtnl_link_i2name(h->links, if_index, if_name, sizeof if_name);

	/* create the item */
	item = probe_item_create(OVAL_UNIX_ROUTINGTABLE, NULL,
				 "destination",    OVAL_DATATYPE_IPV4ADDR, dst_str,
				 "gateway",        OVAL_DATATYPE_IPV4ADDR, gw_str,
				 "flags",          OVAL_DATATYPE_STRING_M, flag_array,
				 "interface_name", OVAL_DATATYPE_STRING,   if_name,
				 NULL);

	probe_item_collect(h->ctx, item);
	SEXP_free(out_dst);
}
#endif /* HAVE_LIBNL */

int probe_main (probe_ctx *ctx, void *arg)
{
        SEXP_t *probe_in, *dst_ent;
#if defined(HAVE_LIBNL)
	struct nl_handle *sk;
	struct nl_cache  *routes, *links;
	rt_cb_helper h;
#endif /* HAVE_LIBNL */
        probe_in = probe_ctx_getobject(ctx);
        dst_ent  = probe_obj_getent(probe_in, "destination", 1);

        if (dst_ent == NULL)
                return (PROBE_ENOENT);

#if defined(HAVE_LIBNL)
	sk = nl_handle_alloc();

	if (nl_connect(sk, NETLINK_ROUTE) != 0) {
		dE("nl_connect(%p, %d) failed: errno=%d, %s\n", errno, strerror(errno));
		return (PROBE_ESYSTEM);
	}

	if ((routes = rtnl_route_alloc_cache(sk)) == NULL) {
		dE("rtnl_route_alloc_cache() failed\n");
		return (PROBE_ESYSTEM);
	}

        if ((links = rtnl_link_alloc_cache(sk)) == NULL) {
                dE("rtnl_link_alloc_cache() failed\n");
                return (PROBE_ESYSTEM);
        }

	h.ctx    = ctx;
	h.in_dst = dst_ent;
        h.links  = links;

	nl_cache_foreach(routes, collect_item, &h);

	nl_cache_free(routes);
	nl_close(sk);
#endif /* HAVE_LIBNL */

	SEXP_free(dst_ent);

        return (0);
}
