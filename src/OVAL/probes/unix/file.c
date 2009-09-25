#ifndef __STUB_PROBE
#include <seap.h>
#include <probe.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include "findfile.h"

#ifndef _A
#define _A(x) assert(x)
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#undef OS_FREEBSD
#undef OS_LINUX
#undef OS_SOLARIS
#undef OS_SUNOS
#undef OS_WINDOWS

#if defined(__FreeBSD__)
# define OS_FREEBSD
#elif defined(__linux__)
# define OS_LINUX
#elif defined(sun) || defined(__sun)
# if defined(__SVR4) || defined(__svr4__)
#  define OS_SOLARIS
# else
#  define OS_SUNOS
# endif
#elif defined(_WIN32)
# define OS_WINDOWS
#else
# error "Sorry, your OS isn't supported."
#endif

static const char *strfiletype (mode_t mode)
{
        switch (mode & S_IFMT) {
        case S_IFREG:  return ("regular");
        case S_IFDIR:  return ("directory");
        case S_IFLNK:  return ("symbolic link");
        case S_IFBLK:  return ("block special");
        case S_IFIFO:  return ("fifo");
        case S_IFSOCK: return ("socket");
        case S_IFCHR:  return ("character special");
        default:
                abort ();
        }
        /* NOTREACHED */
        return (NULL);
}

int file_cb (const char *p, const char *f, void *ptr)
{
        char path_buffer[PATH_MAX];
        SEXP_t *res = (SEXP_t *)ptr, *item;
        const char *st_path;
        struct stat st;
        
        
        if (f != NULL) {
                snprintf (path_buffer, sizeof path_buffer, "%s/%s", p, f);
                st_path = path_buffer;
        } else {
                st_path = p;
        }
        
        if (stat (st_path, &st) == -1) {
                _D("FAIL: errno=%u, %s.\n", errno, strerror (errno));
                return (-1);
        } else {
                item = SEXP_OVALobj_create ("file_item",
                                            SEXP_OVALattr_create ("id", SEXP_string_newf ("FIXME"),
                                                                  "status", SEXP_number_newu (OVAL_STATUS_EXISTS),
                                                                  NULL),
                                            "path", NULL,
                                            SEXP_string_newf (p),
                                            
                                            "filename", NULL,
                                            (f != NULL ? SEXP_string_newf (f) : NULL),
                                            
                                            "type", NULL,
                                            SEXP_string_newf (strfiletype (st.st_mode)),
                                            
                                            "group_id", NULL,
                                            SEXP_string_newf ("%hu", st.st_gid),
                                            
                                            "user_id", NULL,
                                            SEXP_string_newf ("%hu", st.st_uid),
                                            
                                            "a_time", NULL,
                                            SEXP_string_newf ("%u",
#if defined(OS_FREEBSD)
                                                              st.st_atimespec.tv_sec
#elif defined(OS_LINUX) || defined(OS_SOLARIS)
                                                              st.st_atim.tv_sec
#endif
                                                    ),
                                            
                                            "c_time", NULL,
                                            SEXP_string_newf ("%u",
#if defined(OS_FREEBSD)
                                                              st.st_ctimespec.tv_sec
#elif defined(OS_LINUX) || defined(OS_SOLARIS)
                                                              st.st_ctim.tv_sec
#endif
                                                    ),
                                            
                                            "m_time", NULL,
                                            SEXP_string_newf ("%u",
#if defined(OS_FREEBSD)
                                                              st.st_ctimespec.tv_sec
#elif defined(OS_LINUX) || defined(OS_SOLARIS)
                                                              st.st_ctim.tv_sec
#endif
                                                    ),
                                            
                                            "size", NULL,
#if defined(_FILE_OFFSET_BITS)
# if   _FILE_OFFSET_BITS == 64
                                            SEXP_number_newull (st.st_size),
# elif _FILE_OFFSET_BITS == 32
                                            SEXP_number_newu (st.st_size),
# else
#  error "Invalid _FILE_OFFSET_BITS value"
                                            NULL,
# endif
#elif defined(LARGE_FILE_SOURCE)
                                            SEXP_number_newull (st.st_size),
#else
                                            SEXP_number_newu   (st.st_size),
#endif
                                            "suid", NULL,
                                            SEXP_number_newu (st.st_mode & S_ISUID ? 1 : 0),
                                            
                                            "sticky", NULL,
                                            SEXP_number_newu (st.st_mode & S_ISVTX ? 1 : 0),
                                            
                                            "uread", NULL,
                                            SEXP_number_newu (st.st_mode & S_IRUSR ? 1 : 0),

                                            "uwrite", NULL,
                                            SEXP_number_newu (st.st_mode & S_IWUSR ? 1 : 0),

                                            "uexec", NULL,
                                            SEXP_number_newu (st.st_mode & S_IXUSR ? 1 : 0),
                                            
                                            "gread", NULL,
                                            SEXP_number_newu (st.st_mode & S_IRGRP ? 1 : 0),

                                            "gwrite", NULL,
                                            SEXP_number_newu (st.st_mode & S_IWGRP ? 1 : 0),

                                            "gexec", NULL,
                                            SEXP_number_newu (st.st_mode & S_IXGRP ? 1 : 0),
                                            
                                            "oread", NULL,
                                            SEXP_number_newu (st.st_mode & S_IROTH ? 1 : 0),

                                            "owrite", NULL,
                                            SEXP_number_newu (st.st_mode & S_IWOTH ? 1 : 0),

                                            "oexec", NULL,
                                            SEXP_number_newu (st.st_mode & S_IXOTH ? 1 : 0),
                                            
                                            NULL);
                
                SEXP_list_add (res, item);
        }

        return (0);
}

SEXP_t *probe_main (SEXP_t *probe_in, int *err)
{
        SEXP_t *path, *filename, *behaviors, *items;

        path      = SEXP_OVALobj_getelm (probe_in, "path",      1);
        filename  = SEXP_OVALobj_getelm (probe_in, "filename",  1);
        behaviors = SEXP_OVALobj_getelm (probe_in, "behaviors", 1);
        
        if (path == NULL || filename == NULL) {
                *err = PROBE_ENOELM;
                return (NULL);
        }
        
        if (behaviors == NULL) {
                SEXP_t *bh_list;
                
                bh_list = SEXP_OVALelm_create ("behaviors",
                                               SEXP_OVALattr_create ("max_depth",
                                                                     SEXP_string_newf ("-1"),
                                                                     "recurse",
                                                                     SEXP_string_newf ("symlinks and directories"),
                                                                     "recurse_direction",
                                                                     SEXP_string_newf ("none"),
                                                                     "recurse_file_system",
                                                                     SEXP_string_newf ("all"),
                                                                     NULL),
                                               NULL /* val */,
                                               NULL /* end */);
                
                behaviors = SEXP_list_first (bh_list);
        } else {
                if (!SEXP_OVALelm_hasattr (behaviors, "max_depth"))
                        SEXP_OVALelm_attr_add (behaviors,
                                               "max_depth", SEXP_string_newf ("-1"));
                
                if (!SEXP_OVALelm_hasattr (behaviors, "recurse"))
                        SEXP_OVALelm_attr_add (behaviors,
                                               "recurse", SEXP_string_newf ("symlinks and directories"));
                
                if (!SEXP_OVALelm_hasattr (behaviors, "recurse_direction"))
                        SEXP_OVALelm_attr_add (behaviors,
                                               "recurse_direction", SEXP_string_newf ("none"));
                
                if (!SEXP_OVALelm_hasattr (behaviors, "recurse_file_system"))
                        SEXP_OVALelm_attr_add (behaviors,
                                               "recurse_file_system", SEXP_string_newf ("all"));
        }
        
        _A(behaviors != NULL);

        items = SEXP_list_new ();
        
        /* FIXME: == 0 */
        if (find_files (path, filename, behaviors, &file_cb, items) < 0) {
                *err = PROBE_EUNKNOWN;
                return (NULL);
        }
        
        *err = 0;
        return (items);
}
#endif
