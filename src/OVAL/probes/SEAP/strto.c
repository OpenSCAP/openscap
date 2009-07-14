#include <errno.h>
#include "strto.h"

uint64_t strto_uint64 (const char *str, size_t len, char **endptr, int base)
{
        switch (base) {
        case 10: return strto_uint64_dec (str, len, endptr);
        case 16: return strto_uint64_hex (str, len, endptr);
        case  2: return strto_uint64_bin (str, len, endptr);
        case  8: return strto_uint64_oct (str, len, endptr);
        }
        
        /* Invalid number base */
        errno = EINVAL;
        
        return UINT64_C(0);
}

uint64_t strto_uint64_bin (const char *str, size_t len, char **endptr)
{
        uint64_t n, t;
        char    *s;
        unsigned char c;

        n = t = 0;
        s = (char *)str;
        
        while (*s >= '0' && *s <= '1' && len > 0) {
                n += t;
                c  = *s - '0';
                
                if (n < t || (UINT64_MAX - n) < c) {
                        errno = ERANGE;
                        return (UINT64_MAX);
                }
               
                n += c;
                
                t = n;
                --len;
                ++s;
        }
        
        if (len != 0) {
                if (s == str)
                        errno = EINVAL;
                if (endptr != NULL)
                        *endptr = s;
        }
        
        return (n);
}

uint64_t strto_uint64_oct (const char *str, size_t len, char **endptr)
{
        uint64_t n, t;
        char    *s;
        unsigned char c;

        n = t = 0;
        s = (char *)str;
        
        while (*s >= '0' && *s <= '7' && len > 0) {
                n += t; n += t;
                n += t; n += t;
                n += t; n += t;
                n += t;

                c  = *s - '0';
                
                if (n < t || (UINT64_MAX - n) < c) {
                        errno = ERANGE;
                        return (UINT64_MAX);
                }

                n += c;
                t  = n;
                --len;
                ++s;
        }

        if (len != 0) {
                if (s == str)
                        errno = EINVAL;
                if (endptr != NULL)
                        *endptr = s;
        }
        
        return (n);
}

uint64_t strto_uint64_dec (const char *str, size_t len, char **endptr)
{
        uint64_t n, t;
        char    *s;
        unsigned char c;

        n = t = 0;
        s = (char *)str;
        
        while (*s >= '0' && *s <= '9' && len > 0) {
                n += t; n += t;
                n += t; n += t;
                n += t; n += t;
                n += t; n += t;
                n += t;
                     
                c  = *s - '0';
                
                if (n < t || (UINT64_MAX - n) < c) {
                        errno = ERANGE;
                        return (UINT64_MAX);
                }
                
                n += c;
                t  = n;
                --len;
                ++s;
        }

        if (len != 0) {
                if (s == str)
                        errno = EINVAL;
                if (endptr != NULL)
                        *endptr = s;
        }
        
        return (n);
}

uint64_t strto_uint64_hex (const char *str, size_t len, char **endptr)
{
        uint64_t n, t;
        char    *s;
        unsigned char c;

        n = t = 0;
        s = (char *)str;
        
        while (*s >= '0' && *s <= 'f' && len > 0) {
                n += t; n += t;
                n += t; n += t;
                n += t; n += t;
                n += t; n += t;
                n += t; n += t;
                n += t; n += t;
                n += t; n += t;
                n += t;
                
                if (*s <= '9')
                        c = *s - '0';
                else if (*s >= 'a')
                        c = *s - 'a' + 10;
                else if (*s >= 'A' && *s <= 'F')
                        c = *s - 'A' + 10;
                else
                        break;
                                
                if (n < t || (UINT64_MAX - n) < c) {
                        errno = ERANGE;
                        return (UINT64_MAX);
                }
                
                n += c;
                t  = n;
                --len;
                ++s;
        }

        if (len != 0) {
                if (s == str)
                        errno = EINVAL;
                if (endptr != NULL)
                        *endptr = s;
        }
        
        return (n);
}
