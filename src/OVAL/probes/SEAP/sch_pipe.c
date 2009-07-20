#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>
#include <config.h>
#include "generic/common.h"
#include "public/sm_alloc.h"
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

static char *get_exec_path (const char *uri, uint32_t flags)
{
        char *path = NULL;
        uint32_t i = 0;
again:
        switch (*uri) {
        case '/': /* absolute path */
                if (eaccess (uri, X_OK) == 0)
                        return strdup (uri);
                else
                        return (NULL);
                
        case '\0': /* end of string - error */
                return (NULL);
        case ' ':
                if (i < MAX_WHITESPACE_CNT) {
                        ++uri;
                        ++i;
                        goto again;
                } else {
                        _D("Maximum whitespace count reached: %u.\n", MAX_WHITESPACE_CNT);
                        return (NULL);
                }
        default: { /* relative path */
                char *ptok = NULL, *tokctx = NULL;
                
                path = sm_alloc (sizeof (char) * PATH_MAX);
                
                for (ptok = strtok_r (getenv ("PATH"), ":", &tokctx);
                     ptok;
                     ptok = strtok_r (NULL, ":", &tokctx))
                {
                        snprintf (path, PATH_MAX, "%s/%s", ptok, uri);
                        if (eaccess (path, X_OK) == 0) {
                                path = sm_reallocf (path, sizeof (char) * (strlen (path) + 1));
                                return (path);
                        }
                }
                /* not found */
                sm_free (path);
                return (NULL);
        }}
        
        /* NOTREACHED */
        return (NULL);
}

#define DATA(ptr) ((sch_pipedata_t *)((ptr)->scheme_data))

int sch_pipe_connect (SEAP_desc_t *desc, const char *uri, uint32_t flags)
{
        char *execpath;
        pid_t pid;
        int   err;
        int   pfd[2];
        uint8_t i;
                
        desc->scheme_data = sm_talloc (sch_pipedata_t);
        DATA(desc)->execpath = execpath = get_exec_path (uri, flags);
        
        if (DATA(desc)->execpath == NULL) {
                _D("Invalid URI\n");
                sm_free (desc->scheme_data);
                return (-1);
        }

        _D("Executing: \"%s\"\n", execpath);
        
        err = socketpair (AF_UNIX, SOCK_STREAM, 0, pfd);
        if (err < 0) {
                err = errno;
                sm_free (desc->scheme_data);
                sm_free (execpath);
                errno = err;
                return (-1);
        }
        
        switch (pid = fork ()) {
        case -1: /* error */
                err = errno;
                sm_free (desc->scheme_data);
                sm_free (execpath);
                errno = err;
                
                return (-1);
        case  0: /* child */
                close (pfd[0]);
                
                /* err stream will be redirected to /dev/null */
                err = open ("/dev/null", O_WRONLY);
                if (err == -1)
                        _exit (errno);
                
                /* setup in, out, err streams */
                if (dup2 (pfd[1], STDIN_FILENO) != STDIN_FILENO) {
                        _exit (errno);
                }
                if (dup2 (pfd[1], STDOUT_FILENO) != STDOUT_FILENO) {
                        _exit (errno);
                }
                if (dup2 (err, STDERR_FILENO) != STDERR_FILENO) {
                        _exit (errno);
                }
                
                /*
                  if (daemon (1, 1) != 0)
                        _exit (errno);
                */
                        
                /* exec */
                execl (execpath, execpath, NULL);
                _exit (errno);
        default: /* parent */
                close (pfd[1]);
                DATA(desc)->pfd = pfd[0];
                DATA(desc)->pid = pid;

                /*
                err = -1;
                waitpid (pid, &err, 0);
                
                if (WEXITSTATUS(err) != 0) {
                        _D("Child died :[\n");
                        close (DATA(desc)->pfd);
                        xfre e (&(desc->scheme_data));
                        xfre e (&execpath);
                        return (-1);
                }
                */
        }
        
        _D("%s@%u ready. pfd=%d\n",
           execpath, pid, DATA(desc)->pfd);
        
        return (0);
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
        return read (DATA(desc)->pfd, buf, len);
}

ssize_t sch_pipe_send (SEAP_desc_t *desc, void *buf, size_t len, uint32_t flags)
{
        return write (DATA(desc)->pfd, buf, len);
}

ssize_t sch_pipe_sendsexp (SEAP_desc_t *desc, SEXP_t *sexp, uint32_t flags)
{
        return SEXP_st_dprintc (DATA(desc)->pfd, sexp, &(desc->ostate));
}

int sch_pipe_close (SEAP_desc_t *desc, uint32_t flags)
{
        int ret, err = 0;
        /* close the pipe */
        /* FIXME: kill the process */
        
        close (DATA(desc)->pfd);
        
        if (waitpid (DATA(desc)->pid, &ret, WNOHANG) == DATA(desc)->pid) {
                err = WEXITSTATUS(ret);
                _D("child err: %u, %s.\n", err, strerror (err));
        }
        sm_free (DATA(desc)->execpath);
        sm_free (desc->scheme_data);
        return (err);
}
