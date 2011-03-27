#include <string.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <ares.h>

#include "probe-api.h"
#include "common/debug_priv.h"
#include "common/assume.h"

void *probe_init(void)
{
        ares_library_init(0);
        return (NULL);
}

void probe_fini(void *unused)
{
        (void)unused;
        ares_library_cleanup();
}

struct cbarg {
        SEXP_t *probe_out;
        SEXP_t *se_domain_name;
};

static void ares_query_cb(void *arg, int status, int timeouts, unsigned char *abuf, int alen)
{
        struct cbarg        *res = (struct cbarg *)arg;
        struct ares_addrttl  ttls4[64];
        struct ares_addr6ttl ttls6[64];
        int                  nttl;

        char ip4_strbuf[INET_ADDRSTRLEN];
        char ip6_strbuf[INET6_ADDRSTRLEN];

        SEXP_t *item, *r0, *r1;

        nttl = 64;

        if (ares_parse_a_reply(abuf, alen, NULL, ttls4, &nttl) == ARES_SUCCESS) {
                for (--nttl; nttl >= 0; --nttl)
                {
                        inet_ntop(AF_INET, &ttls4[nttl].ipaddr, ip4_strbuf, INET_ADDRSTRLEN);
                        item = probe_item_creat("dnscache_item", NULL,
                                                "domain_name", NULL, res->se_domain_name,
                                                "ttl",         NULL, r0 = SEXP_number_newi(ttls4[nttl].ttl),
                                                "ip_address",  NULL, r1 = SEXP_string_new(ip4_strbuf, strlen(ip4_strbuf)),
                                                NULL);

                        probe_cobj_add_item(res->probe_out, item);
                        SEXP_vfree(r0, r1, item, NULL);
                }
        }

        nttl = 64;

        if (ares_parse_aaaa_reply(abuf, alen, NULL, ttls6, &nttl) == ARES_SUCCESS) {
                for (--nttl; nttl >= 0; --nttl)
                {
                        inet_ntop(AF_INET6, &ttls6[nttl].ip6addr, ip6_strbuf, INET6_ADDRSTRLEN);
                        item = probe_item_creat("dnscache_item", NULL,
                                                "domain_name", NULL, res->se_domain_name,
                                                "ttl",         NULL, r0 = SEXP_number_newi(ttls6[nttl].ttl),
                                                "ip_address",  NULL, r1 = SEXP_string_new(ip6_strbuf, strlen(ip6_strbuf)),
                                                NULL);

                        probe_cobj_add_item(res->probe_out, item);
                        SEXP_vfree(r0, r1, item, NULL);
                }
        }
}

int probe_main(SEXP_t *probe_in, SEXP_t *probe_out, void *unused, SEXP_t *filters)
{
        ares_channel        channel;
        struct ares_options options;

        int    nfds, cnt;
        fd_set rfds, wfds;
        struct timeval tv, *tvp;

        struct cbarg res;
        char *domain_name;
        (void)unused;

        res.probe_out = probe_out;
        res.se_domain_name = probe_obj_getentval(probe_in, "domain_name", 1);

        if (res.se_domain_name == NULL) {
                dE("Missing entity or value: obj=%p, ent=%s\n", probe_in, "domain_name");
                return (PROBE_ENOENT);
        }

        domain_name = SEXP_string_cstr(res.se_domain_name);

        if (domain_name == NULL) {
                SEXP_free(res.se_domain_name);
                return (PROBE_EINVAL);
        }

        options.flags = ARES_FLAG_NORECURSE;

        if (ares_init_options(&channel,
                              &options, ARES_OPT_FLAGS) != ARES_SUCCESS)
        {
                SEXP_free(res.se_domain_name);
                oscap_free(domain_name);
                return (PROBE_EINIT);
        }

        ares_query(channel, domain_name, C_IN, T_ANY, &ares_query_cb, &res);

        for(;;) {
                FD_ZERO(&rfds);
                FD_ZERO(&wfds);

                nfds = ares_fds(channel, &rfds, &wfds);

                if (nfds == 0)
                        break;

                tvp = ares_timeout(channel, NULL, &tv);
                cnt = select(nfds, &rfds, &wfds, NULL, tvp);

                if (cnt > 0)
                        ares_process(channel, &rfds, &wfds);
        }

        SEXP_free(res.se_domain_name);
        oscap_free(domain_name);
        ares_destroy(channel);

        return (0);
}
