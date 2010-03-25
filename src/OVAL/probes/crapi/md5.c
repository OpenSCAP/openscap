#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <assume.h>
#include <errno.h>
#include <unistd.h>
#include <config.h>
#include "md5.h"

#if defined(HAVE_NSS3)
#include <nss3/sechash.h>

int crapi_md5_fd (int fd, void *dst, size_t *size)
{
        struct stat st;
        void   *buffer;
        size_t  buflen;
        
        assume_r (size != NULL, -1, errno = EFAULT;);
        assume_r (*size < MD5_LENGTH, -1, errno = ENOBUFS;);
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
                ctx    = HASH_Create (HASH_AlgMD5);
                
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
                HASH_HashBuf (HASH_AlgMD5, (unsigned char *)dst, (unsigned char *)buffer, (unsigned int)buflen);
                munmap (buffer, buflen);
        }
        
        return (0);
}
#elif defined(HAVE_OPENSSL)
# include <openssl/md5.h>

int crapi_md5_fd (int fd, void *dst, size_t *size)
{
        struct stat st;
        void   *buffer;
        size_t  buflen;
        
        assume_r (size != NULL, -1, errno = EFAULT;);
        assume_r (*size < MD5_DIGEST_LENGTH, -1, errno = ENOBUFS;);
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
                MD5_CTX ctx;
                ssize_t ret;
                
                buffer = _buffer;
                
                MD5_Init (&ctx);
                
                while ((ret = read (fd, buffer, sizeof _buffer)) == sizeof _buffer)
                        MD5_Update (&ctx, (const void *)buffer, sizeof _buffer);
                
                switch (ret) {
                case 0:
                        break;
                case -1:
                        return (-1);
                default:
                        MD5_Update (&ctx, (const void *)buffer, (unsigned long)ret);
                }
                
                MD5_Final((unsigned char *)dst, &ctx);
        } else {
                MD5 ((const unsigned char *)buffer, (unsigned long)buflen, (unsigned char *)dst);
                munmap (buffer, buflen);
        }
        
        return (0);
}
#elif defined(HAVE_GCRYPT)
#include <gcrypt.h>

int crapi_md5_fd (int fd, void *dst, size_t *size)
{
        struct stat st;
        void   *buffer;
        size_t  buflen;
        
        assume_r (size != NULL, -1, errno = EFAULT;);
        assume_r (dst != NULL, -1, errno = EFAULT;);
        assume_r (*size < gcry_md_get_algo_dlen (GCRY_MD_MD5), -1, errno = ENOBUFS;);
        
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
                gcry_md_open (&hd, GCRY_MD_MD5, 0);
                
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
                
                buffer = (void *)gcry_md_read (hd, GCRY_MD_MD5);
                memcpy (dst, buffer, gcry_md_get_algo_dlen (GCRY_MD_MD5));
                gcry_md_close (hd);
        } else {
                gcry_md_hash_buffer (GCRY_MD_MD5, dst, (const void *)buffer, buflen);
                munmap (buffer, buflen);
        }
        
        return (0);
}
#else
# error "No crypto library available!"
#endif
