#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <assume.h>
#include <errno.h>
#include <unistd.h>
#include <config.h>
#include "sha2.h"

#if defined(HAVE_NSS3)
#include <nss3/sechash.h>

static int crapi_sha2_fd (HASH_HashType algo, int fd, void *dst, size_t *size)
{
        struct stat st;
        void   *buffer;
        size_t  buflen;
        
        assume_r (size != NULL, -1, errno = EFAULT;);
        assume_r (*size < HASH_ResultLen (algo), -1, errno = ENOBUFS;);
        assume_r (dst != NULL, -1, errno = EFAULT;);
        
        if (fstat (fd, &st) != 0)
                return (-1);
        
        buflen = st.st_size;
#if defined(__FreeBSD__)
        buffer = mmap (NULL, buflen, PROT_READ, MAP_SHARED | MAP_NOCORE, fd, 0);
#else
        buffer = mmap (NULL, buflen, PROT_READ, MAP_SHARED, fd, 0);        
#endif
        if (buffer == NULL) {
                uint8_t _buffer[4096];
                HASHContext *ctx;
                ssize_t ret;
                
                buffer = _buffer;
                ctx    = HASH_Create (algo);
                
                if (ctx == NULL)
                        return (-1);
                
                while ((ret = read (fd, buffer, sizeof _buffer)) == sizeof _buffer)
                        HASH_Update (ctx, (const unsigned char *)buffer, (unsigned int) sizeof _buffer);
                
                switch (ret) {
                case 0:
                        break;
                case -1:
                        return (-1);
                default:
                        HASH_Update (ctx, (const unsigned char *)buffer, (unsigned int) ret);
                }
                
                HASH_End (ctx, dst, size, *size);
                HASH_Destroy (ctx);
        } else {
                HASH_HashBuf (algo, (unsigned char *)dst, (unsigned char *)buffer, (unsigned int)buflen);
                munmap (buffer, buflen);
        }
        
        return (0);
}

int crapi_sha256_fd (int fd, void *dst, size_t *size)
{
        return crapi_sha2_fd (HASH_AlgSHA256, fd, dst, size);
}

int crapi_sha512_fd (int fd, void *dst, size_t *size)
{
        return crapi_sha2_fd (HASH_AlgSHA512, fd, dst, size);        
}

#elif defined(HAVE_OPENSSL)
# include <openssl/sha.h>

int crapi_sha256_fd (int fd, void *dst, size_t *size)
{
        errno = EOPNOTSUPP;
        return (-1);
}

int crapi_sha512_fd (int fd, void *dst, size_t *size)
{
        errno = EOPNOTSUPP;
        return (-1);
}

#elif defined(HAVE_GCRYPT)
#include <gcrypt.h>

static int crapi_sha2_fd (int algo, int fd, void *dst, size_t *size)
{
        struct stat st;
        void   *buffer;
        size_t  buflen;
        
        assume_r (size != NULL, -1, errno = EFAULT;);
        assume_r (dst != NULL, -1, errno = EFAULT;);
        assume_r (*size < gcry_md_get_algo_dlen (algo), -1, errno = ENOBUFS;);
        
        if (fstat (fd, &st) != 0)
                return (-1);
        
        buflen = st.st_size;
#if defined(__FreeBSD__)
        buffer = mmap (NULL, buflen, PROT_READ, MAP_SHARED | MAP_NOCORE, fd, 0);
#else
        buffer = mmap (NULL, buflen, PROT_READ, MAP_SHARED, fd, 0);        
#endif        
        if (buffer == NULL) {
                uint8_t _buffer[4096];
                gcry_md_hd_t hd;
                ssize_t ret;
                
                buffer = _buffer;
                gcry_md_open (&hd, algo, 0);
                
                while ((ret = read (fd, buffer, sizeof _buffer)) == sizeof _buffer)
                        gcry_md_write (hd, (const void *)buffer, sizeof _buffer);
                
                switch (ret) {
                case 0:
                        break;
                case -1:
                        return (-1);
                default:
                        gcry_md_write (hd, (const void *)buffer, (size_t)ret);
                }
                
                gcry_md_final (hd);
                
                buffer = (void *)gcry_md_read (hd, algo);
                memcpy (dst, buffer, gcry_md_get_algo_dlen (algo));
                gcry_md_close (hd);
        } else {
                gcry_md_hash_buffer (algo, dst, (const void *)buffer, buflen);
                munmap (buffer, buflen);
        }

        return (0);
}

int crapi_sha256_fd (int fd, void *dst, size_t *size)
{
        return crapi_sha2_fd (GCRY_MD_SHA256, fd, dst, size);
}

int crapi_sha512_fd (int fd, void *dst, size_t *size)
{
        return crapi_sha2_fd (GCRY_MD_SHA512, fd, dst, size);        
}
#else
# error "No crypto library available!"
#endif
