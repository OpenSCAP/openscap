#ifndef __STUB_PROBE
#ifndef __DPKGINFO_HELPER__
#define __DPKGINFO_HELPER__

/*
 * Author: Pierre Chifflier <chifflier@edenwall.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

struct dpkginfo_reply_t {
        char *name;
        char *arch;
        char *epoch;
        char *release;
        char *version;
        char *evr;
};

int dpkginfo_init();
int dpkginfo_fini();

struct dpkginfo_reply_t * dpkginfo_get_by_name(const char *name, int *err);

void * dpkginfo_free_reply(struct dpkginfo_reply_t *reply);

#ifdef __cplusplus
}
#endif

#endif /* __DPKGINFO_HELPER__ */
#endif /* __STUB_PROBE */
