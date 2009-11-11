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

#include <seap.h>
#include <probe-api.h>

#include <sys/utsname.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>

#if defined(__linux__)
#include <arpa/inet.h>
#include <netlink/route/link.h>
#include <netlink/route/addr.h>

struct nl_cache *link_cache;

static void cb(struct nl_object *obj, void *arg)
{
        struct rtnl_addr *rtaddr = (struct rtnl_addr *) obj;
        struct nl_addr *absaddr;
        struct rtnl_link *rtlink;
        int ifindex;
        char iabuf[64], mabuf[64];
        char *name = NULL;
        SEXP_t *item = (SEXP_t *) arg;
        SEXP_t *attrs, *r0, *r1, *r2;

        ifindex = rtnl_addr_get_ifindex(rtaddr);
        name = rtnl_addr_get_label(rtaddr);
        absaddr = rtnl_addr_get_local(rtaddr);
        nl_addr2str(absaddr, iabuf, sizeof (iabuf));

        rtlink = rtnl_link_get(link_cache, ifindex);
        if (name == NULL)
                name = rtnl_link_get_name(rtlink);

        absaddr = rtnl_link_get_addr(rtlink);
        rtnl_link_put(rtlink);
        nl_addr2str(absaddr, mabuf, sizeof (mabuf));
        
        attrs = probe_attr_creat("name",
                                 r0 = SEXP_string_newf("%s", name),
                                 "ip_address",
                                 r1 = SEXP_string_newf("%s", iabuf),
                                 "mac_address",
                                 r2 = SEXP_string_newf("%s", mabuf),
                                 NULL);
        probe_item_ent_add(item, "interface", attrs, NULL);
        SEXP_vfree(attrs, r0, r1, r2, NULL);
}

void get_ifs(SEXP_t *item)
{
        struct nl_handle *sock;
        struct nl_cache *addr_cache;

        sock = nl_handle_alloc();
        nl_connect(sock, NETLINK_ROUTE);
        link_cache = rtnl_link_alloc_cache(sock);
        addr_cache = rtnl_addr_alloc_cache(sock);

        nl_cache_foreach(addr_cache, cb, (void *) item);

        nl_cache_free(link_cache);
        nl_cache_free(addr_cache);
        nl_close(sock);
        nl_handle_destroy(sock);
}
#else
void get_ifs(SEXP_t *item)
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
        
        return;
}
#endif

SEXP_t *probe_main(SEXP_t *probe_in, int *err, void *arg)
{
	SEXP_t *item;
        SEXP_t *r0, *r1, *r2, *r3;
        char *os_name, *os_version, *architecture,
                hname[NI_MAXHOST];
        struct utsname sname;
        struct addrinfo *results;
        int e;
        
        (void)arg;

	if (probe_in == NULL) {
		*err = PROBE_EINVAL;
		return NULL;
	}

        if (uname(&sname) == -1) {
                *err = PROBE_EUNKNOWN;
                return NULL;
        }

        os_name = sname.sysname;
        os_version = sname.version;
        architecture = sname.machine;

        if (gethostname(hname, sizeof (hname)) == -1) {
                _D("gethostname: %u, %s.\n", errno, strerror (errno));
                *err = PROBE_EUNKNOWN;
                
                return NULL;
        }

        if ((e = getaddrinfo(hname, NULL, NULL, &results)) != 0) {
                _D("getaddrinfo(%s): %s\n", hname, gai_strerror (e));
                *err = PROBE_EUNKNOWN;
                
                return NULL;
        }

        if ((e = getnameinfo(results->ai_addr, sizeof (struct sockaddr),
                             hname, sizeof (hname), NULL, 0, 0)) != 0) {
                freeaddrinfo(results);
                _D("getnameinfo: %s\n", gai_strerror (e));
                *err = PROBE_EUNKNOWN;
                
                return NULL;
        }
        freeaddrinfo(results);

        item  = probe_item_creat ("system_info_item", NULL,
                                  /* entities */
                                  "os_name", NULL, r0 = SEXP_string_newf("%s", os_name),
                                  "os_version", NULL, r1 = SEXP_string_newf("%s", os_version),
                                  "os_architecture", NULL, r2 = SEXP_string_newf("%s", architecture),
                                  "primary_host_name", NULL, r3 = SEXP_string_newf("%s", hname),
                                  NULL);

        get_ifs(item);
        SEXP_vfree (r0, r1, r2, r3, NULL);
	*err = 0;

	return (item);
}
