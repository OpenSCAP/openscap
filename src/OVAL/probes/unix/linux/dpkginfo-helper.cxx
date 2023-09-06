/*
 * Author: Pierre Chifflier <chifflier@edenwall.com>
 */

#include <cstdio>
#include <cstring>
#include <iostream>
#include <stdlib.h>

#include <apt-pkg/init.h>
#include <apt-pkg/error.h>
#include <apt-pkg/configuration.h>
#include <apt-pkg/fileutl.h>
#include <apt-pkg/mmap.h>
#include <apt-pkg/pkgcache.h>
#include <apt-pkg/pkgrecords.h>
#include <apt-pkg/pkgsystem.h>
#include <apt-pkg/cachefile.h>

#include "dpkginfo-helper.h"

using namespace std;

static int _init_done = 0;
static pkgCacheFile *cgCache = NULL;

static int opencache (void) {
        if (pkgInitConfig (*_config) == false) return 0;

        const char* root = getenv("OSCAP_PROBE_ROOT");
        if (root != NULL) {
            string pkgCacheRoot(root);
            _config->Set("RootDir", pkgCacheRoot);
        }

        if (pkgInitSystem (*_config, _system) == false) return 0;

        if (_error->PendingError () == true) {
                _error->DumpErrors ();
                return 0;
        }

        return 1;
}

struct dpkginfo_reply_t * dpkginfo_get_by_name(const char *name, int *err)
{
        pkgCache &cache = *cgCache->GetPkgCache();
        pkgRecords Recs (cache);
        struct dpkginfo_reply_t *reply = NULL;

        if (!cgCache->ReadOnlyOpen(NULL)) return 0;

        // Locate the package
        pkgCache::PkgIterator Pkg = cache.FindPkg(name);
        if (Pkg.end() == true) {
                /* not found, clear error flag */
                if (err) *err = 0;
                return NULL;
        }

        pkgCache::VerIterator V1 = Pkg.CurrentVer();
        if (V1.end() == true) {
                /* not installed, clear error flag */
                /* FIXME this should be different that not found */
                if (err) *err = 0;
                return NULL;
        }
        pkgRecords::Parser &P = Recs.Lookup(V1.FileList());

        /* split epoch, version and release */
        string evr = V1.VerStr();
        string epoch, version, release;
        string::size_type version_start = 0, version_stop;
        string::size_type pos;
        string evr_str;

        pos = evr.find_first_of(":");
        if (pos != string::npos) {
                epoch = evr.substr(0, pos);
                version_start = pos+1;
        } else
        {
		    epoch = "0";
        }

        pos = evr.find_first_of("-");
        if (pos != string::npos) {
                version = evr.substr(version_start, pos-version_start);
                version_stop = pos+1;
                release = evr.substr(version_stop, evr.length()-version_stop);
                evr_str = epoch + ":" + version + "-" + release;


        } else { /* no release number, probably a native package */
                version = evr.substr(version_start, evr.length()-version_start);
                release = "";
                evr_str = epoch + ":" + version;
        }

        reply = new(struct dpkginfo_reply_t);
        memset(reply, 0, sizeof(struct dpkginfo_reply_t));
        reply->name = strdup(Pkg.Name());
        reply->arch = strdup(V1.Arch());
        reply->epoch = strdup(epoch.c_str());
        reply->release = strdup(release.c_str());
        reply->version = strdup(version.c_str());
        reply->evr = strdup(evr_str.c_str());

        return reply;
}

void dpkginfo_free_reply(struct dpkginfo_reply_t *reply)
{
        if (reply) {
                free(reply->name);
                free(reply->arch);
                free(reply->epoch);
                free(reply->release);
                free(reply->version);
                free(reply->evr);
                delete reply;
        }
}

int dpkginfo_init()
{
        if (_init_done == 0) {
                cgCache = new pkgCacheFile;
                if (opencache() != 1) {
                        delete cgCache;
                        cgCache = NULL;
                        return -1;
                }
                _init_done = 1;
        }

        return 0;
}

int dpkginfo_fini()
{
        if (cgCache != NULL) {
                cgCache->Close();
        }

        return 0;
}

