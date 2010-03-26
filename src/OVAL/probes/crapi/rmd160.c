#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <assume.h>
#include <errno.h>
#include <unistd.h>
#include <config.h>
#include "crapi.h"
#include "rmd160.h"

#if defined(HAVE_NSS3)
#include <nss3/sechash.h>

int crapi_rmd160_fd (int fd, void *dst, size_t *size)
{
        errno = EOPNOTSUPP;
        return (-1);
}
#elif defined(HAVE_GCRYPT)
#include <gcrypt.h>

int crapi_rmd160_fd (int fd, void *dst, size_t *size)
{
        struct stat st;
        void   *buffer;
        size_t  buflen;
        
        assume_r (size != NULL, -1, errno = EFAULT;);
        assume_r (dst != NULL, -1, errno = EFAULT;);
        assume_r (*size < gcry_md_get_algo_dlen (GCRY_MD_RMD160), -1, errno = ENOBUFS;);
        
        if (fstat (fd, &st) != 0)
                return (-1);
        else {
#if _FILE_OFFSET_BITS == 32
                buflen = st.st_size;
# if defined(__FreeBSD__)
                buffer = mmap (NULL, buflen, PROT_READ, MAP_SHARED | MAP_NOCORE, fd, 0);
# else
                buffer = mmap (NULL, buflen, PROT_READ, MAP_SHARED, fd, 0);        
# endif        
                if (buffer == NULL) {
#endif /* _FILE_OFFSET_BITS == 32 */
                        uint8_t _buffer[CRAPI_IO_BUFSZ];
                        gcry_md_hd_t hd;
                        ssize_t ret;
                
                        buffer = _buffer;
                        gcry_md_open (&hd, GCRY_MD_RMD160, 0);
                
                        while ((ret = read (fd, buffer, sizeof _buffer)) == sizeof _buffer)
                                gcry_md_write (hd, (const void *)buffer, sizeof _buffer);
                
                        switch (ret) {
                        case 0:
                                break;
                        case -1:
                                return (-1);
                        default:
                                assume_r (ret > 0, -1, gcry_md_close (hd););
                                gcry_md_write (hd, (const void *)buffer, (size_t)ret);
                        }
                
                        gcry_md_final (hd);
                
                        buffer = (void *)gcry_md_read (hd, GCRY_MD_RMD160);
                        memcpy (dst, buffer, gcry_md_get_algo_dlen (GCRY_MD_RMD160));
                        gcry_md_close (hd);
#if _FILE_OFFSET_BITS == 32
                } else {
                        gcry_md_hash_buffer (GCRY_MD_RMD160, dst, (const void *)buffer, buflen);
                        munmap (buffer, buflen);
                }
#endif /* _FILE_OFFSET_BITS == 32 */
        }
        return (0);
}
#else
# error "No crypto library available!"
#endif
