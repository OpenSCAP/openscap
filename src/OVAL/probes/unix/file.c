#include <seap.h>
#include <probe-api.h>
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
                SEXP_t *r0, *r1, *r2, *r3, *r4;
                SEXP_t *r5, *r6, *r7, *r8;
                SEXP_t *r_t, *r_f;
                
                r_t = SEXP_number_newb (true);
                r_f = SEXP_number_newb (false);
                
                item = probe_obj_creat ("file_item", NULL,
                                        /* entities */                                        
                                        "path", NULL,
                                        r0 = SEXP_string_newf (p),
                                        
                                        "filename", NULL,
                                        r1 = (f != NULL ? SEXP_string_newf (f) : NULL),
                                        
                                        "type", NULL,
                                        r2 = SEXP_string_newf (strfiletype (st.st_mode)),
                                        
                                        "group_id", NULL,
                                        r3 = SEXP_string_newf ("%hu", st.st_gid),
                                        
                                        "user_id", NULL,
                                        r4 = SEXP_string_newf ("%hu", st.st_uid),
                                        
                                        "a_time", NULL,
                                        r5 = SEXP_string_newf ("%u",
#if defined(OS_FREEBSD)
                                                               st.st_atimespec.tv_sec
#elif defined(OS_LINUX) || defined(OS_SOLARIS)
                                                               st.st_atim.tv_sec
#endif
                                                ),
                                        
                                        "c_time", NULL,
                                        r6 = SEXP_string_newf ("%u",
#if defined(OS_FREEBSD)
                                                               st.st_ctimespec.tv_sec
#elif defined(OS_LINUX) || defined(OS_SOLARIS)
                                                               st.st_ctim.tv_sec
#endif
                                                ),
                                        
                                        "m_time", NULL,
                                        r7 = SEXP_string_newf ("%u",
#if defined(OS_FREEBSD)
                                                               st.st_ctimespec.tv_sec
#elif defined(OS_LINUX) || defined(OS_SOLARIS)
                                                               st.st_ctim.tv_sec
#endif
                                                ),
                                        
                                        "size", NULL,
#if defined(_FILE_OFFSET_BITS)
# if   _FILE_OFFSET_BITS == 64
                                        r8 = SEXP_number_newu_64 (st.st_size),
# elif _FILE_OFFSET_BITS == 32
                                        r8 = SEXP_number_newu_32 (st.st_size),
# else
#  error "Invalid _FILE_OFFSET_BITS value"
                                        r8 = NULL,
# endif
#elif defined(LARGE_FILE_SOURCE)
                                        r8 = SEXP_number_newu_64 (st.st_size),
#else
                                        r8 = SEXP_number_newu_32 (st.st_size),
#endif
                                        "suid", NULL,
                                        (st.st_mode & S_ISUID ? r_t : r_f),
                                        
                                        "sticky", NULL,
                                        (st.st_mode & S_ISVTX ? r_t : r_f),
                                        
                                        "uread", NULL,
                                        (st.st_mode & S_IRUSR ? r_t : r_f),
                                        
                                        "uwrite", NULL,
                                        (st.st_mode & S_IWUSR ? r_t : r_f),
                                        
                                        "uexec", NULL,
                                        (st.st_mode & S_IXUSR ? r_t : r_f),
                                        
                                        "gread", NULL,
                                        (st.st_mode & S_IRGRP ? r_t : r_f),
                                        
                                        "gwrite", NULL,
                                        (st.st_mode & S_IWGRP ? r_t : r_f),
                                        
                                        "gexec", NULL,
                                        (st.st_mode & S_IXGRP ? r_t : r_f),
                                        
                                        "oread", NULL,
                                        (st.st_mode & S_IROTH ? r_t : r_f),
                                        
                                        "owrite", NULL,
                                        (st.st_mode & S_IWOTH ? r_t : r_f),

                                        "oexec", NULL,
                                        (st.st_mode & S_IXOTH ? r_t : r_f),
                                        
                                        NULL);
                
                SEXP_list_add (res, item);
        
                SEXP_free (item);
                SEXP_free (r0); SEXP_free (r1);
                SEXP_free (r2); SEXP_free (r3);
                SEXP_free (r4); SEXP_free (r5);
                SEXP_free (r6); SEXP_free (r7);
                SEXP_free (r8);
                SEXP_free (r_t); SEXP_free (r_f);
                
        }
        
        return (0);
}

SEXP_t *probe_main (SEXP_t *probe_in, int *err)
{
        SEXP_t *path, *filename, *behaviors, *items;
        SEXP_t *r0, *r1, *r2, *r3, *r4;
        
        path      = probe_obj_getent (probe_in, "path",      1);
        filename  = probe_obj_getent (probe_in, "filename",  1);
        behaviors = probe_obj_getent (probe_in, "behaviors", 1);
        
        if (path == NULL || filename == NULL) {
                *err = PROBE_ENOELM;
                
                SEXP_free (behaviors);
                SEXP_free (path);
                SEXP_free (filename);
                
                return (NULL);
        }
        
        if (behaviors == NULL) {
                SEXP_t *bh_list;
                
                bh_list = probe_ent_creat ("behaviors",
                                           r0 = probe_attr_creat ("max_depth",
                                                                  r1 = SEXP_string_newf ("-1"),
                                                                  "recurse",
                                                                  r2 = SEXP_string_newf ("symlinks and directories"),
                                                                  "recurse_direction",
                                                                  r3 = SEXP_string_newf ("none"),
                                                                  "recurse_file_system",
                                                                  r4 = SEXP_string_newf ("all"),
                                                                  NULL),
                                           NULL /* val */,
                                           NULL /* end */);
                
                behaviors = SEXP_list_first (bh_list);
                
                SEXP_free (bh_list);
                SEXP_free (r0);
                SEXP_free (r1);
                SEXP_free (r2);
                SEXP_free (r3);
                SEXP_free (r4);
        } else {
                if (!probe_ent_attrexists (behaviors, "max_depth")) {
                        probe_ent_attr_add (behaviors,
                                            "max_depth", r0 = SEXP_string_newf ("-1"));
                        SEXP_free (r0);
                }
                
                if (!probe_ent_attrexists (behaviors, "recurse")) {
                        probe_ent_attr_add (behaviors,
                                            "recurse", r0 = SEXP_string_newf ("symlinks and directories"));
                        SEXP_free (r0);
                }
                
                if (!probe_ent_attrexists (behaviors, "recurse_direction")) {
                        probe_ent_attr_add (behaviors,
                                            "recurse_direction", r0 = SEXP_string_newf ("none"));
                        SEXP_free (r0);
                }
                
                if (!probe_ent_attrexists (behaviors, "recurse_file_system")) {
                        probe_ent_attr_add (behaviors,
                                            "recurse_file_system", r0 = SEXP_string_newf ("all"));
                        SEXP_free (r0);
                }
        }
        
        _A(behaviors != NULL);
        
        items = SEXP_list_new (NULL);
        
        /* FIXME: == 0 */
        if (find_files (path, filename, behaviors, &file_cb, items) < 0) {
                SEXP_free (items);
                SEXP_free (behaviors);
                SEXP_free (path);
                SEXP_free (filename);

                *err = PROBE_EUNKNOWN;
                return (NULL);
        }
        
        SEXP_free (behaviors);
        SEXP_free (filename);
        SEXP_free (path);

        *err = 0;
        return (items);
}
