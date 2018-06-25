/**
 * @file   routingtable_probe.c
 * @brief  routingtable probe
 * @author "Daniel Kopecek <dkopecek@redhat.com>"
 */

/*
 * Copyright 2009-2012 Red Hat Inc., Durham, North Carolina.
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
 *   Daniel Kopecek <dkopecek@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <endian.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <net/route.h>
#include <arpa/inet.h>

#include "probe-api.h"
#include "probe/entcmp.h"
#include "util.h"
#include "debug_priv.h"
#include "SEAP/generic/strto.h"
#include "routingtable_probe.h"

#ifndef RT_FLAGS_MAX
#define RT_FLAGS_MAX 10
#endif

#ifndef IF_NAME_MAXLEN
#define IF_NAME_MAXLEN 128
#endif

#define RT_INFO_DELIMITERS " \t"

#ifndef htobe32
#include <byteswap.h>
# if BYTE_ORDER == LITTLE_ENDIAN
#  define htobe32(x) __bswap_32 (x)
# else
#  define htobe32(x) (x)
# endif
#endif

struct route_info {
    SEXP_t *ip_dst_ent;
    char ip_dst[INET6_ADDRSTRLEN+1];
    char ip_gw[INET6_ADDRSTRLEN+1];
    char *rt_flags[RT_FLAGS_MAX+1];
    char if_name[IF_NAME_MAXLEN+1];
    int ip_version;
};

static int hexstring2bin(const char *hexstr, size_t hexlen, uint8_t *binbuf, size_t binlen)
{
    register size_t i, l;

	if (hexlen % 2 != 0) {
		return -1;
	}
	if (binlen * 2 < hexlen) {
		return -1;
	}

    for (l = 0, i = 0; i < hexlen; ++i,++i,++l) {
       binbuf[l] = strto_uint8_hex(hexstr + i, 2, NULL);
       if (errno != 0)
           return -1;
    }

    return 0;
}

static int collect_item(struct route_info *rt, probe_ctx *ctx)
{
	SEXP_t *item, *rt_dst;
        oval_datatype_t addr_type;

	rt_dst = SEXP_string_new(rt->ip_dst, strlen(rt->ip_dst));

	if (probe_entobj_cmp(rt->ip_dst_ent, rt_dst) != OVAL_RESULT_TRUE) {
		SEXP_free(rt_dst);
		return 0;
	}

        addr_type = rt->ip_version == 4 ? OVAL_DATATYPE_IPV4ADDR : OVAL_DATATYPE_IPV6ADDR;
	
	/* create the item */
	item = probe_item_create(OVAL_UNIX_ROUTINGTABLE, NULL,
				"destination",    addr_type, rt->ip_dst,
				"gateway",        addr_type, rt->ip_gw,
				"flags",          OVAL_DATATYPE_STRING_M, rt->rt_flags,
				"interface_name", OVAL_DATATYPE_STRING,   rt->if_name,
				NULL);

        SEXP_free(rt_dst);
	return probe_item_collect(ctx, item) == 2 ? 1 : 0;
}

static int proc_ip4_to_string(const char *proc_ip, size_t proc_iplen, char *strbuf, size_t strbuflen)
{
    uint8_t bb[4];
    uint32_t *addr = (uint32_t *)bb;
    struct in_addr ip4;

	if (strbuf == NULL) {
		return -1;
	}
	if (proc_ip == NULL || proc_iplen <= 0) {
		return -1;
	}

    if (hexstring2bin(proc_ip, proc_iplen, bb, sizeof bb) != 0)
        return -1;

    ip4.s_addr = htobe32(*addr);

    if (inet_ntop(AF_INET, &ip4, strbuf, strbuflen) == NULL)
        return -1;
    return 0;
}

static int proc_ip6_to_string(const char *proc_ip, size_t proc_iplen, char *strbuf, size_t strbuflen)
{
    struct in6_addr ip6;

	if (strbuf == NULL) {
		return -1;
	}
	if (proc_ip == NULL || proc_iplen <= 0) {
		return -1;
	}

    if (hexstring2bin(proc_ip, proc_iplen, (uint8_t *)&ip6, sizeof ip6) != 0)
        return -1;
    if (inet_ntop(AF_INET6, &ip6, strbuf, strbuflen) == NULL)
        return -1;
    return 0;
}

static int process_line_ip4(char *line, struct route_info *rt)
{
    char *token[8];
    char *save;
    uint16_t rt_flags;
    register int i;

	if (line == NULL || rt == NULL) {
		return -1;
	}

    save = NULL;
    token[0] = strtok_r(line, RT_INFO_DELIMITERS, &save);

    for (i = 1; i < 4; ++i) {
        token[i] = strtok_r(NULL, RT_INFO_DELIMITERS, &save);
        if (token[i] == NULL)
            return -1;
    }
    /* Tokens
     * 0 - interface name
     * 1 - destination
     * 2 - gateway
     * 3 - flags
     * 7 - netmask
     */
#define TOK_dst    token[1]
#define TOK_gw     token[2]
#define TOK_flags  token[3]
#define TOK_ifname token[0]

    dI("name=%s, dst=%s, gw=%s, flags=%s", TOK_ifname, TOK_dst, TOK_gw, TOK_flags);

    if (proc_ip4_to_string(TOK_dst, strlen(TOK_dst), rt->ip_dst, sizeof rt->ip_dst) != 0 ||
        proc_ip4_to_string(TOK_gw, strlen(TOK_gw), rt->ip_gw, sizeof rt->ip_gw) != 0)
        return -1;

    strncpy(rt->if_name, TOK_ifname, IF_NAME_MAXLEN); /* interface name */
    rt_flags = strto_uint16_hex(TOK_flags, strlen(TOK_flags), NULL);

    if (errno != 0)
        return -1;

#define RT_COND_ADD_FLAG(flag, value) if (rt_flags & (flag)) rt->rt_flags[i++] = (value)

    i = 0;
    RT_COND_ADD_FLAG(RTF_UP, "UP");
    RT_COND_ADD_FLAG(RTF_GATEWAY, "GATEWAY");
    RT_COND_ADD_FLAG(RTF_HOST, "HOST");
    RT_COND_ADD_FLAG(RTF_REINSTATE, "REINSTATE");
    RT_COND_ADD_FLAG(RTF_DYNAMIC, "DYNAMIC");
    RT_COND_ADD_FLAG(RTF_MODIFIED, "MODIFIED");
    RT_COND_ADD_FLAG(RTF_REJECT, "REJECT");
    rt->rt_flags[i] = NULL;

#undef TOK_dst
#undef TOK_gw
#undef TOK_flags
#undef TOK_ifname
    rt->ip_version = 4;
    return 0;
}

static int process_line_ip6(char *line, struct route_info *rt)
{
    char *token[10];
    char *save;
    uint32_t rt_flags;
    register int i;

	if (line == NULL || rt == NULL) {
		return -1;
	}

    save = NULL;
    token[0] = strtok_r(line, RT_INFO_DELIMITERS, &save);

    for (i = 1; i < 10; ++i) {
        token[i] = strtok_r(NULL, RT_INFO_DELIMITERS, &save);
        if (token[i] == NULL)
            return -1;
    }

    /* Tokens
     * 0 - destination
     * 4 - gateway
     * 8 - flags
     * 9 - interface name
     */
#define TOK_dst    token[0]
#define TOK_gw     token[4]
#define TOK_flags  token[8]
#define TOK_ifname token[9]

    dI("name=%s, dst=%s, gw=%s, flags=%s", TOK_ifname, TOK_dst, TOK_gw, TOK_flags);

    if (proc_ip6_to_string(TOK_dst, strlen(TOK_dst), rt->ip_dst, sizeof rt->ip_dst) != 0 ||
        proc_ip6_to_string(TOK_gw, strlen(TOK_gw), rt->ip_gw, sizeof rt->ip_gw) != 0)
        return -1;

    strncpy(rt->if_name, TOK_ifname, IF_NAME_MAXLEN); /* interface name */
    rt_flags = strto_uint32_hex(TOK_flags, strlen(TOK_flags), NULL);
    if (errno != 0)
        return -1;

    i = 0;
    RT_COND_ADD_FLAG(RTF_UP, "UP");
    RT_COND_ADD_FLAG(RTF_GATEWAY, "GATEWAY");
    RT_COND_ADD_FLAG(RTF_HOST, "HOST");
    RT_COND_ADD_FLAG(RTF_REINSTATE, "REINSTATE");
    RT_COND_ADD_FLAG(RTF_DYNAMIC, "DYNAMIC");
    RT_COND_ADD_FLAG(RTF_MODIFIED, "MODIFIED");
    RT_COND_ADD_FLAG(RTF_ADDRCONF, "ADDRCONF");
    RT_COND_ADD_FLAG(RTF_CACHE, "CACHE");
    RT_COND_ADD_FLAG(RTF_REJECT, "REJECT");
    rt->rt_flags[i] = NULL;

#undef TOK_dst
#undef TOK_gw
#undef TOK_flags
#undef TOK_ifname
    rt->ip_version = 6;
    return 0;
}

int routingtable_probe_main(probe_ctx *ctx, void *arg)
{
	SEXP_t *probe_in, *dst_ent;
	FILE *fp;
	char *line_buf;
	size_t line_len;
        struct route_info rt;
        int probe_ret = 0;

	probe_in = probe_ctx_getobject(ctx);
	dst_ent  = probe_obj_getent(probe_in, "destination", 1);

	if (dst_ent == NULL)
		return (PROBE_ENOENT);

        rt.ip_dst_ent = dst_ent;
	line_len = 0;
	line_buf = NULL;
	fp = NULL;

	switch(probe_ent_getdatatype(dst_ent)) {
	  case OVAL_DATATYPE_IPV4ADDR:
	    fp = fopen("/proc/net/route", "r");
            /* Skip the header line */
            if (getline(&line_buf, &line_len, fp) != -1) {
                while(getline(&line_buf, &line_len, fp) != -1) {
                    if (process_line_ip4(line_buf, &rt) != 0)
                        break;
                    if (collect_item(&rt, ctx) != 0)
                        break;
                }
            }

	    if (!feof(fp)) {
	      /* error */
              dE("An error ocured while reading /proc/net/route: %s", strerror(errno));
	    }
	    break;
	  case OVAL_DATATYPE_IPV6ADDR:
	    fp = fopen("/proc/net/ipv6_route", "r");

	    while(getline(&line_buf, &line_len, fp) != -1) {
	      if (process_line_ip6(line_buf, &rt) != 0)
		break;
	      if (collect_item(&rt, ctx) != 0)
		break;
	    }

	    if (!feof(fp)) {
	      /* error */
              dE("An error ocured while reading /proc/net/ipv6_route: %s", strerror(errno));
            }
	    break;
          default:
            probe_ret = EINVAL;
	}

	if (fp != NULL)
	  fclose(fp);
	if (line_buf != NULL)
	  free(line_buf);

	SEXP_free(dst_ent);

	return (probe_ret);
}
