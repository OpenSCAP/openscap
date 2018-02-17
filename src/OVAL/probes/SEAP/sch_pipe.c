/*
 * Copyright 2009 Red Hat Inc., Durham, North Carolina.
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
 *      "Daniel Kopecek" <dkopecek@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <WinSock2.h>
#else
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#endif
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>
#include <common/assume.h>
#include <common/_error.h>
#include <errno.h>

#include "generic/common.h"
#include "public/sm_alloc.h"
#include "public/strbuf.h"
#include "_sexp-types.h"
#include "_seap-types.h"
#include "_sexp-output.h"
#include "_seap-scheme.h"
#include "sch_pipe.h"
#include "seap-descriptor.h"

extern char **environ;

#define MAX_WHITESPACE_CNT 64

#ifndef PATH_MAX
# define PATH_MAX 1024
#else
# if PATH_MAX < 1 || PATH_MAX > 4096
#  undef PATH_MAX
#  define PATH_MAX 2048
# endif
#endif /* PATH_MAX */

#ifndef WCOREDUMP
# if defined(_AIX)
# define WCOREDUMP(x) ((x) & 0x80)
# endif
#endif /* WCOREDUMP */

static char *get_exec_path (const char *uri, uint32_t flags)
{
        char  *path;
        size_t ulen;

        ulen = strlen (uri);

        if (ulen < 3)
                return (NULL);
        else if (uri[0] != '/' && uri[1] != '/')
                return (NULL);
        else {
                uri  += 2;
                ulen -= 2;
        }

        path = sm_alloc (sizeof (char) * (PATH_MAX + 1));

        if (*uri == '/') {
                if (ulen <= PATH_MAX) {
                        memcpy (path, uri, sizeof (char) * ulen);
                        path[ulen] = '\0';
                        path = (char *)sm_reallocf (path, sizeof (char) * (ulen + 1));

                        return (path);
                } else
                        goto fail;
        } else {
#ifndef SEAP_SCHEME_PIPE_NORELPATH
#ifndef SEAP_SCHEME_PIPE_NOPATHSEARCH
                if (strchr (uri, '/') == NULL) {
                        struct stat st;
                        char *ptok = NULL;
                        char *tctx = NULL;
                        char *PATH = getenv ("PATH");
                        char  __path[PATH_MAX];
                        size_t tlen;

                        for (ptok = strtok_r (PATH, ":", &tctx);
                             ptok;
                             ptok = strtok_r (NULL, ":", &tctx))
                        {
                                tlen = strlen (ptok);

                                if (tlen + ulen + 1 < PATH_MAX) {
                                        memcpy (__path, ptok, sizeof (char) * tlen);
                                        __path[tlen] = '/';
                                        memcpy (__path + tlen + 1, uri, sizeof (char) * ulen);
                                        __path[tlen + ulen + 1] = '\0';

                                        if (stat (__path, &st) != 0)
                                                continue;

                                        memcpy (path, __path, sizeof (char) * (tlen + ulen + 1));
                                        path = (char *)sm_reallocf (path, sizeof (char) * (tlen + ulen + 1));

                                        return (path);
                                } /* else - skip this token */
                        }

                        goto fail;
                }
#endif /* SEAP_SCHEME_PIPE_NOPATHSEARCH */
                if (getcwd (path, PATH_MAX) == NULL)
                        goto fail;
                else {
                        size_t clen = strlen (path);

                        if (clen + ulen > PATH_MAX)
                                goto fail;

                        if (path[clen - 1] != '/') {
                                if (clen + ulen + 1 > PATH_MAX)
                                        goto fail;
                                else {
                                        path[clen] = '/';
                                        ++clen;
                                }
                        }

                        memcpy (path + clen, uri, sizeof (char) * ulen);
                        path[clen + ulen] = '\0';
                        path = (char *)sm_reallocf (path, sizeof (char) * (clen + ulen + 1));

                        return (path);
                }
#else /* !SEAP_SCHEME_PIPE_NORELPATH */
                goto fail;
#endif
        }
fail:
        sm_free (path);
        return (NULL);
}

static int check_child (pid_t pid, int waitf)
{
#ifdef _WIN32
	errno = EOPNOTSUPP;
	return (-1);
#else
        int status = -1;

        switch (pid = waitpid (pid, &status, waitf ? 0 : WNOHANG)) {
        case  0: return (0);
        case -1: return (-1);
        default:
                /* child is dead */
		if (WIFSIGNALED(status)) {
			oscap_seterr(OSCAP_EFAMILY_OVAL, "Probe with PID=%ld has been killed with signal %d", (long)pid, WTERMSIG(status));
			errno = EINTR;
#if defined(WCOREDUMP)
			if (WCOREDUMP(status)) {
				oscap_seterr(OSCAP_EFAMILY_OVAL, "Probe with PID=%ld has core dumped.", (long)pid);
			}
#endif /* WCOREDUMP */
		}
                if (WIFEXITED(status)) {
                        errno = WEXITSTATUS(status);
                }
        }
        return (1);
#endif
}

int sch_pipe_connect (SEAP_desc_t *desc, const char *uri, uint32_t flags)
{
#ifdef _WIN32
	errno = EOPNOTSUPP;
	return (-1);
#else
        sch_pipedata_t *data;
        pid_t pid;
        int   pfd[2] = { -1, -1 };

        assume_r (desc != NULL, -1, errno = EFAULT;);
        assume_r (uri  != NULL, -1, errno = EFAULT;);
        assume_r (desc->scheme_data == NULL, -1, errno = EALREADY;);

        data = (sch_pipedata_t *) sm_talloc (sch_pipedata_t);
        data->execpath = get_exec_path (uri, flags);

        if (data->execpath == NULL) {
                errno = EINVAL;
                goto fail1;
        } else {
                struct stat st;

                if (stat (data->execpath, &st) != 0)
                        goto fail1;
                if (!S_ISREG(st.st_mode))
                        goto fail1;
        }

        if (socketpair (AF_UNIX, SOCK_STREAM, 0, pfd) < 0)
                goto fail1;

        switch (pid = fork ()) {
        case -1: /* error */
                goto fail1;
        case  0: /* child */
                close (pfd[0]);

                /*
                 * setup input and output streams
                 */
                if (dup2 (pfd[1], STDIN_FILENO) != STDIN_FILENO)
                        _exit (errno);
                if (dup2 (pfd[1], STDOUT_FILENO) != STDOUT_FILENO)
                        _exit (errno);
                execl (data->execpath, data->execpath, NULL);
                _exit (errno);
        default: /* parent */
                close (pfd[1]);

                data->pfd = pfd[0];
                data->pid = pid;

                if (check_child (data->pid, 0) != 0)
                        goto fail2;
        }

        desc->scheme_data = (void *)data;

        return (0);
fail2:
        protect_errno {
                close (data->pfd);
        }
fail1:
        protect_errno {
                if (data->execpath != NULL)
                        sm_free (data->execpath);
                sm_free (data);
        }
        return (-1);
#endif
}

int sch_pipe_openfd (SEAP_desc_t *desc, int fd, uint32_t flags)
{
        errno = EOPNOTSUPP;
        return (-1);
}

int sch_pipe_openfd2 (SEAP_desc_t *desc, int ifd, int ofd, uint32_t flags)
{
        errno = EOPNOTSUPP;
        return (-1);
}

ssize_t sch_pipe_recv (SEAP_desc_t *desc, void *buf, size_t len, uint32_t flags)
{
        sch_pipedata_t *data;
	ssize_t         ret;

        assume_d (desc != NULL, -1, errno = EFAULT;);
        assume_d (buf  != NULL, -1, errno = EFAULT;);

        data = (sch_pipedata_t *)desc->scheme_data;

        assume_r (data != NULL, -1, errno = EBADF;);
	/* TODO: This code is duplicated in every supported operation in the SEAP pipe schema. */

#ifdef _WIN32
	errno = EOPNOTSUPP;
	return (-1);
#else
        if (check_child (data->pid, 0) == 0) {
                if ((ret = read (data->pfd, buf, len)) == 0)
			if (check_child(data->pid, 0))
				return (-1);

		return (ret);
        } else
                return (-1);
#endif
}

ssize_t sch_pipe_send (SEAP_desc_t *desc, void *buf, size_t len, uint32_t flags)
{
        sch_pipedata_t *data;

        assume_d (desc != NULL, -1, errno = EFAULT;);
        assume_d (buf  != NULL, -1, errno = EFAULT;);

        data = (sch_pipedata_t *)desc->scheme_data;

        assume_r (data != NULL, -1, errno = EBADF;);

#ifdef _WIN32
	errno = EOPNOTSUPP;
	return (-1);
#else
        if (check_child (data->pid, 0) == 0)
                return write (data->pfd, buf, len);
        else
                return (-1);
#endif
}

ssize_t sch_pipe_sendsexp (SEAP_desc_t *desc, SEXP_t *sexp, uint32_t flags)
{
        sch_pipedata_t *data;

        assume_d (desc != NULL, -1, errno = EFAULT;);
        assume_d (sexp != NULL, -1, errno = EFAULT;);

        data = (sch_pipedata_t *)desc->scheme_data;

        assume_r (data != NULL, -1, errno = EBADF;);

#ifdef _WIN32
	errno = EOPNOTSUPP;
	return (-1);
#else
        if (check_child (data->pid, 0) != 0)
                return (-1);
        else {
                ssize_t ret;
		strbuf_t *sb;

                ret = 0;
                sb  = strbuf_new (SEAP_STRBUF_MAX);

                if (SEXP_sbprintf_t (sexp, sb) != 0)
                        ret = -1;
                else
                        ret = strbuf_write (sb, data->pfd);

                strbuf_free (sb);

                return (ret);
        }
#endif
}

int sch_pipe_close (SEAP_desc_t *desc, uint32_t flags)
{
        int try;
        sch_pipedata_t *data;

        assume_d (desc != NULL, -1, errno = EFAULT;);

        data = (sch_pipedata_t *)desc->scheme_data;

        assume_r (data != NULL, -1, errno = EBADF;);

#ifdef _WIN32
	errno = EOPNOTSUPP;
	return (-1);
#else
        kill (data->pid, SIGTERM);

        for (try = 0; try < 3; ++try) {
                switch (check_child (data->pid, 1)) {
                case  0:
                        kill (data->pid, SIGTERM);
                        break;
                case -1:
                        return (-1);
                case  1:
                        goto clean;
                }
        }

        /*
         * Child is not responding to our request. Kill it.
         */
        kill (data->pid, SIGKILL);

        switch (check_child (data->pid, 0)) {
        case  1:
                break;
        default:
                return (-1);
        }
clean:
        close (data->pfd);

        sm_free (data->execpath);
        sm_free (data);

        desc->scheme_data = NULL;

        return (0);
#endif
}

int sch_pipe_select (SEAP_desc_t *desc, int ev, uint16_t timeout, uint32_t flags)
{
        sch_pipedata_t *data;

        assume_d (desc != NULL, -1, errno = EFAULT;);

        data = (sch_pipedata_t *)desc->scheme_data;

        assume_r (data != NULL, -1, errno = EBADF;);

#ifdef _WIN32
	errno = EOPNOTSUPP;
	return (-1);
#else
        if (check_child (data->pid, 0) == 0) {
                fd_set *wptr, *rptr;
                fd_set  fset;
                struct timeval *tv_ptr, tv;

                FD_ZERO(&fset);
                tv_ptr = NULL;
                wptr   = NULL;
                rptr   = NULL;

                switch (ev) {
                case SEAP_IO_EVREAD:
                        FD_SET(data->pfd, &fset);
                        rptr = &fset;
                        break;
                case SEAP_IO_EVWRITE:
                        FD_SET(data->pfd, &fset);
                        wptr = &fset;
                        break;
                default:
                        abort ();
                }

                if (timeout > 0) {
                        tv.tv_sec  = (time_t)timeout;
                        tv.tv_usec = 0;
                        tv_ptr = &tv;
                }

                assume_d (!(wptr == NULL && rptr == NULL), -1, errno = EINVAL;);
                assume_d (!(wptr != NULL && rptr != NULL), -1, errno = EINVAL;);

                switch (select (data->pfd + 1, rptr, wptr, NULL, tv_ptr)) {
                case -1:
                        return (-1);
                case  0:
                        errno = ETIMEDOUT;
                        return (-1);
                default:
                        return (FD_ISSET(data->pfd, &fset) ? 0 : -1);
                }
        }

        return (-1);
#endif
}
