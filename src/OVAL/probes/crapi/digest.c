#include <stdarg.h>
#include <stddef.h>
#include <assume.h>
#include <errno.h>
#include "digest.h"
#include "md5.h"
#include "sha1.h"
#include "sha2.h"
#include "rmd160.h"

int crapi_digest_fd (int fd, crapi_alg_t alg, void *dst, size_t *size)
{
        assume_r(dst  != NULL, -1, errno = EFAULT;);
        assume_r(size != NULL, -1, errno = EFAULT;);
        
        switch (alg) {
        case CRAPI_DIGEST_MD5:
                return crapi_md5_fd (fd, dst, size);
        case CRAPI_DIGEST_SHA1:
                return crapi_sha1_fd (fd, dst, size);
        case CRAPI_DIGEST_SHA256:
                return crapi_sha256_fd (fd, dst, size);
        case CRAPI_DIGEST_SHA512:
                return crapi_sha512_fd (fd, dst, size);
        case CRAPI_DIGEST_RMD160:
                return crapi_rmd160_fd (fd, dst, size);
        }
        
        errno = EINVAL;
        return (-1);
}

int crapi_mdigest_fd (int fd, int num, crapi_alg_t alg, void *dst, size_t *size, ...)
{
        errno = EOPNOTSUPP;
        return (-1);
}
