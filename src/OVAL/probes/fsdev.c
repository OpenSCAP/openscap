#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#if defined(__linux__)
# include <mntent.h>
# include <unistd.h>
#elif defined(__FreeBSD__)
# include <sys/param.h>
# include <sys/ucred.h>
# include <sys/mount.h>
#else
# error "Sorry, your OS isn't supported."
#endif

#include "fsdev.h"

static int fsdev_cmp (const void *a, const void *b)
{
        return memcmp (a, b, sizeof (dev_t));
}

static int fsname_cmp (const void *a, const void *b)
{
        return strcmp (a, b);
}

static int match_fs (const char *fsname, const char **fs_arr, size_t fs_cnt)
{
        size_t w, s;
        int cmp;

        w = fs_cnt;
        s = 0;
        
        while (w > 0) {
                cmp = fsname_cmp (fsname, fs_arr[s + w/2]);
                if (cmp > 0) {
                        s += w/2 + 1;
                        w  = w - w/2 - 1;
                } else if (cmp < 0) {
                        w = w/2;
                } else {
                        return (1);
                }
        }
        
        return (0);
}

#if defined(__linux__)

#define DEVID_ARRAY_SIZE 16
#define DEVID_ARRAY_ADD  8

static fsdev_t *__fsdev_init (fsdev_t *lfs, const char **fs, size_t fs_cnt)
{
        int e;
        FILE *fp;
        size_t i;
        
        struct mntent *ment;
        struct stat    st;

        fp = setmntent (_PATH_MOUNTED, "r");
        if (fp == NULL) {
                e = errno;
                free (lfs);
                errno = e;
                return (NULL);
        }

        lfs->ids = malloc (sizeof (dev_t) * DEVID_ARRAY_SIZE);
        
        if (lfs->ids == NULL) {
                e = errno;
                free (lfs);
                errno = e;
                return (NULL);
        }

        lfs->cnt = DEVID_ARRAY_SIZE;
        i = 0;

        if (fs == NULL) {
                while ((ment = getmntent (fp)) != NULL) {
                        /* TODO: Is this check reliable? */
                        if (eaccess (ment->mnt_fsname, F_OK) == 0) {
                                
                                if (stat (ment->mnt_dir, &st) != 0)
                                        continue;
                                
                                if (i >= lfs->cnt) {
                                        lfs->cnt += DEVID_ARRAY_ADD;
                                        lfs->ids  = realloc (lfs->ids, sizeof (dev_t) * lfs->cnt);
                                }
                                
                                memcpy (&(lfs->ids[i++]), &st.st_dev, sizeof (dev_t));
                        }
                }
        } else {
                while ((ment = getmntent (fp)) != NULL) {
                        
                        if (match_fs (ment->mnt_type, fs, fs_cnt)) {
                                
                                if (stat (ment->mnt_dir, &st) != 0)
                                        continue;
                                
                                if (i >= lfs->cnt) {
                                        lfs->cnt += DEVID_ARRAY_ADD;
                                        lfs->ids  = realloc (lfs->ids, sizeof (dev_t) * lfs->cnt);
                                }
                                
                                memcpy (&(lfs->ids[i++]), &st.st_dev, sizeof (dev_t));
                        }
                }
        }
        
        fclose (fp);
        
        lfs->ids = realloc (lfs->ids, sizeof (dev_t) * i);
        lfs->cnt = (lfs->ids == NULL ? 0 : i);
        
        return (lfs);
}
#elif defined(__FreeBSD__)
static fsdev_t *__fsdev_init (fsdev_t *lfs, const char **fs, size_t fs_cnt)
{
        struct statfs *mntbuf = NULL;
        struct stat st;
        int i;
        
        
        lfs->cnt = getmntinfo (&mntbuf,
                               (fs == NULL ? MNT_LOCAL : 0 ) | MNT_NOWAIT);
        lfs->ids = malloc (sizeof (dev_t) * lfs->cnt);
        
        if (fs == NULL) {
                for (i = 0; i < lfs->cnt; ++i) {
                        if (stat (mntbuf[i].f_mntonname, &st) != 0)
                                continue;
                        
                        memcpy (&(lfs->ids[i]), &st.st_dev, sizeof (dev_t));
                }
        } else {
                for (i = 0; i < lfs->cnt; ++i) {
                        if (!match_fs (mntbuf[i].f_fstypename, fs, fs_cnt))
                                continue;
                        
                        memcpy (&(lfs->ids[i]), &st.st_dev, sizeof (dev_t));
                }
        }
        
        if (i != lfs->cnt) {
                lfs->ids = realloc (lfs->ids, sizeof (dev_t) * i);
                lfs->cnt = i;
        }
        
        return (lfs);
}
#endif

fsdev_t *fsdev_init (const char **fs, size_t fs_cnt)
{
        fsdev_t *lfs;

        lfs = malloc (sizeof (fsdev_t));
        lfs = __fsdev_init (lfs, fs, fs_cnt);
        
        if (lfs == NULL)
                return (NULL);
        
        qsort (lfs->ids, lfs->cnt, sizeof (dev_t), fsdev_cmp);
        
        return (lfs);
}

static inline int isfschar (int c)
{
        return (isalpha (c) ||
                isdigit (c) ||
                c == '-' || c == '_');
}

fsdev_t *fsdev_strinit (const char *fs_names)
{
        fsdev_t *lfs;
        char *pstr, **fs_arr;
        size_t fs_cnt;
        int state, e;
        
        pstr = strdup (fs_names);
        state  = 0;
        fs_arr = NULL;
        fs_cnt = 0;

        while (*pstr != '\0') {
                switch (state) {
                case 0:
                        if (isfschar (*pstr)) {
                                state = 1;
                                ++fs_cnt;
                                fs_arr = realloc (fs_arr, sizeof (char *) * fs_cnt);
                                fs_arr[fs_cnt - 1] = pstr;
                        }
                        
                        ++pstr;
                        
                        break;
                case 1:
                        if (!isfschar (*pstr) && *pstr != '\0') {
                                state = 0;
                                *pstr = '\0';
                                ++pstr;
                        }
                        break;
                }
        }
        
        qsort (fs_arr, fs_cnt, sizeof (char *), fsname_cmp);
        
        lfs = fsdev_init ((const char **)fs_arr, fs_cnt);
        e = errno;
        free (fs_arr);
        errno = e;
        
        return (lfs);
}

void fsdev_free (fsdev_t *lfs)
{
        free (lfs->ids);
        free (lfs);
}

int fsdev_search (fsdev_t *lfs, void *id)
{
        uint16_t w, s;
        int cmp;
        
        w = lfs->cnt;
        s = 0;
        
        while (w > 0) {
                cmp = fsdev_cmp (id, &(lfs->ids[s + w/2]));
                if (cmp > 0) {
                        s += w/2 + 1;
                        w  = w - w/2 - 1;
                } else if (cmp < 0) {
                        w = w/2;
                } else {
                        return (1);
                }
        }
        
        return (0);
}

int fsdev_path (fsdev_t *lfs, const char *path)
{
        struct stat st;
        
        if (stat (path, &st) != 0)
                return (-1);
        
        return fsdev_search (lfs, &st.st_dev);
}

int fsdev_fd (fsdev_t *lfs, int fd)
{
        struct stat st;

        if (fstat (fd, &st) != 0)
                return (-1);

        return fsdev_search (lfs, &st.st_dev);
}
