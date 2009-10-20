#ifndef __STUB_PROBE
#include <seap.h>
#include <probe-api.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
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


static SEXP_t *gr_true   = NULL, *gr_false  = NULL, *gr_t_reg  = NULL;
static SEXP_t *gr_t_dir  = NULL, *gr_t_lnk  = NULL, *gr_t_blk  = NULL;
static SEXP_t *gr_t_fifo = NULL, *gr_t_sock = NULL, *gr_t_char = NULL;

static SEXP_t *strfiletype (mode_t mode)
{
        switch (mode & S_IFMT) {
        case S_IFREG:  return (gr_t_reg);
        case S_IFDIR:  return (gr_t_dir);
        case S_IFLNK:  return (gr_t_lnk);
        case S_IFBLK:  return (gr_t_blk);
        case S_IFIFO:  return (gr_t_fifo);
        case S_IFSOCK: return (gr_t_sock);
        case S_IFCHR:  return (gr_t_char);
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
                _D("p = \"%s\"; f = \"%s\"\n", p, f);
                snprintf (path_buffer, sizeof path_buffer, "%s/%s", p, f);
                st_path = path_buffer;
        } else {
                st_path = p;
        }
        
        if (stat (st_path, &st) == -1) {
                _D("FAIL: errno=%u, %s.\n", errno, strerror (errno));
                return (-1);
        } else {
                SEXP_t *r0, *r1, *r3, *r4;
                SEXP_t *r5, *r6, *r7, *r8;
                                                
                item = probe_obj_creat ("file_item", NULL,
                                        /* entities */                                        
                                        "path", NULL,
                                        r0 = SEXP_string_newf (p),
                                        
                                        "filename", NULL,
                                        r1 = (f != NULL ? SEXP_string_newf (f) : NULL),
                                        
                                        "type", NULL,
                                        strfiletype (st.st_mode),
                                        
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
                                        (st.st_mode & S_ISUID ? gr_true : gr_false),
                                        
                                        "sticky", NULL,
                                        (st.st_mode & S_ISVTX ? gr_true : gr_false),
                                        
                                        "uread", NULL,
                                        (st.st_mode & S_IRUSR ? gr_true : gr_false),
                                        
                                        "uwrite", NULL,
                                        (st.st_mode & S_IWUSR ? gr_true : gr_false),
                                        
                                        "uexec", NULL,
                                        (st.st_mode & S_IXUSR ? gr_true : gr_false),
                                        
                                        "gread", NULL,
                                        (st.st_mode & S_IRGRP ? gr_true : gr_false),
                                        
                                        "gwrite", NULL,
                                        (st.st_mode & S_IWGRP ? gr_true : gr_false),
                                        
                                        "gexec", NULL,
                                        (st.st_mode & S_IXGRP ? gr_true : gr_false),
                                        
                                        "oread", NULL,
                                        (st.st_mode & S_IROTH ? gr_true : gr_false),
                                        
                                        "owrite", NULL,
                                        (st.st_mode & S_IWOTH ? gr_true : gr_false),

                                        "oexec", NULL,
                                        (st.st_mode & S_IXOTH ? gr_true : gr_false),
                                        
                                        NULL);
                
                
                SEXP_list_add (res, item);
                
                _D("item memory size = %zu bytes\n", SEXP_sizeof (item));
                _D("list memory size = %zu bytes\n", SEXP_sizeof (res));
                
                SEXP_vfree (item,
                            r0, r1, r3, r4,
                            r5, r6, r7, r8, NULL);
        }
        
        return (0);
}

static pthread_mutex_t __file_probe_mutex;

SEXP_t *probe_init (void)
{
        _LOGCALL_;
        
        /*
         * Initialize true/false global reference.
         */
        gr_true  = SEXP_number_newb (true);
        gr_false = SEXP_number_newb (false);
        
        /*
         * Initialize file type string references.
         * (Used by strfiletype())
         */
        gr_t_reg  = SEXP_string_new ("regular", strlen ("regular"));
        gr_t_dir  = SEXP_string_new ("directory", strlen ("directory"));
        gr_t_lnk  = SEXP_string_new ("symbolic link", strlen ("symbolic link"));
        gr_t_blk  = SEXP_string_new ("block special", strlen ("block special"));
        gr_t_fifo = SEXP_string_new ("fifo", strlen ("fifo"));
        gr_t_sock = SEXP_string_new ("socket", strlen ("socket"));
        gr_t_char = SEXP_string_new ("character special", strlen ("character special"));

        /*
         * Initialize mutex.
         */
        switch (pthread_mutex_init (&__file_probe_mutex, NULL)) {
        case 0:
                return ((void *)&__file_probe_mutex);
        default:
                _D("Can't initialize mutex: errno=%u, %s.\n", errno, strerror (errno));
        }
        
        return (NULL);
}

void probe_fini (SEXP_t *arg)
{
        _A(arg == &__file_probe_mutex);
        
        /*
         * Release global reference.
         */
        SEXP_vfree (gr_true, gr_false, gr_t_reg,
                    gr_t_dir, gr_t_lnk, gr_t_blk,
                    gr_t_fifo, gr_t_sock, gr_t_char,
                    NULL);
        
        /*
         * Destroy mutex.
         */
        (void) pthread_mutex_destroy (&__file_probe_mutex);
        
        return;
}

SEXP_t *probe_main (SEXP_t *probe_in, int *err, void *mutex)
{
        SEXP_t *path, *filename, *behaviors, *items;
        SEXP_t *r0, *r1, *r2, *r3, *r4;
        
        if (mutex == NULL) {
                *err = PROBE_EINIT;
                return (NULL);
        }
        
        _A(mutex == &__file_probe_mutex);
        
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
         
                SEXP_vfree (bh_list,
                            r0, r1, r2, r3, r4, NULL);
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

        switch (pthread_mutex_lock (&__file_probe_mutex)) {
        case 0:
                break;
        default:
                _D("Can't lock mutex(%p): %u, %s.\n", &__file_probe_mutex, errno, strerror (errno));
                
                *err = PROBE_EFATAL;
                return (NULL);
        }
        
        /* FIXME: == 0 */
        if (find_files (path, filename, behaviors, &file_cb, items) < 0) {
                SEXP_free (items);
                SEXP_free (behaviors);
                SEXP_free (path);
                SEXP_free (filename);
                
                switch (pthread_mutex_unlock (&__file_probe_mutex)) {
                case 0:
                        break;
                default:
                        _D("Can't unlock mutex(%p): %u, %s.\n", &__file_probe_mutex, errno, strerror (errno));
                        
                        *err = PROBE_EFATAL;
                        return (NULL);
                }
                
                *err = PROBE_EUNKNOWN;
                return (NULL);
        }
        
        SEXP_free (behaviors);
        SEXP_free (filename);
        SEXP_free (path);
        
        switch (pthread_mutex_unlock (&__file_probe_mutex)) {
        case 0:
                break;
        default:
                _D("Can't unlock mutex(%p): %u, %s.\n", &__file_probe_mutex, errno, strerror (errno));
                
                *err = PROBE_EFATAL;
                return (NULL);
        }
        
        *err = 0;
        return (items);
}
#endif
