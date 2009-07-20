#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <errno.h>
#include <config.h>
#include "generic/common.h"
#include "sm_alloc.h"
#include "_sexp-types.h"
#include "_sexp-manip.h"

SEXP_t *SEXP_number_new (const void *num, NUM_type_t type)
{
        SEXP_t *sexp;

        _A(num != NULL);

        sexp = SEXP_new ();
        SEXP_SETTYPE(sexp, ATOM_NUMBER);
   
        switch (type) {
        case NUM_INT8:
                NUM_STORE(int8_t, (*(int8_t *)(num)), sexp->atom.number.nptr);
                break;
        case NUM_UINT8:
                NUM_STORE(uint8_t, (*(uint8_t *)(num)), sexp->atom.number.nptr);
                break;
        case NUM_INT16:
                NUM_STORE(int16_t, (*(int16_t *)(num)), sexp->atom.number.nptr);
                break;
        case NUM_UINT16:
                NUM_STORE(uint16_t, (*(uint16_t *)(num)), sexp->atom.number.nptr);
                break;
        case NUM_INT32:
                NUM_STORE(int32_t, (*(int32_t *)(num)), sexp->atom.number.nptr);
                break;
        case NUM_UINT32:
                NUM_STORE(uint32_t, (*(uint32_t *)(num)), sexp->atom.number.nptr);
                break;
        case NUM_INT64:
                NUM_STORE(int64_t, (*(int64_t *)(num)), sexp->atom.number.nptr);
                break;
        case NUM_UINT64:
                NUM_STORE(uint64_t, (*(uint64_t *)(num)), sexp->atom.number.nptr);
                break;
        case NUM_DOUBLE:
                NUM_STORE(double, (*(double *)(num)), sexp->atom.number.nptr);
                break;
        default:
                _D("Unsupported number type: %d\n", type);
                abort ();                
        }
   
        sexp->atom.number.type = type;
     
        return (sexp);
}

int SEXP_numberp (const SEXP_t *sexp)
{
        if (sexp != NULL) {
                SEXP_VALIDATE(sexp);
                return (SEXP_TYPE(sexp) == ATOM_NUMBER);
        } else {
                errno = EFAULT;
                return (0);
        }
}

size_t SEXP_number_size (const SEXP_t *sexp)
{
        if (sexp != NULL) {
                SEXP_VALIDATE(sexp);
        
                if (SEXP_TYPE(sexp) == ATOM_NUMBER) {
                        switch (sexp->atom.number.type) {
                        case NUM_INT8:
                                return sizeof (int8_t);
                        case NUM_UINT8:
                                return sizeof (uint8_t);
                        case NUM_INT16:
                                return sizeof (int16_t);
                        case NUM_UINT16:
                                return sizeof (uint16_t);
                        case NUM_INT32:
                                return sizeof (int32_t);
                        case NUM_UINT32:
                                return sizeof (uint32_t);
                        case NUM_INT64:
                                return sizeof (int64_t);
                        case NUM_UINT64:
                                return sizeof (uint64_t);
                        case NUM_DOUBLE:
                                return sizeof (double);
                        default:
                                _D("Unsupported number type: %d\n", sexp->atom.number.type);
                                abort ();
                        }
                } else {
                        errno = EINVAL;
                }
        } else {
                errno = EFAULT;
        }

        return ((size_t)-1);
}

int SEXP_number_get (const SEXP_t *sexp, void *ptr, NUM_type_t type)
{
        _A(ptr != NULL);
        
        if (sexp != NULL) {
                SEXP_VALIDATE(sexp);
                
                if (SEXP_TYPE(sexp) == ATOM_NUMBER) {
                        if (type >= sexp->atom.number.type) {
                                switch (sexp->atom.number.type) {
                                case NUM_INT8:
                                        switch (type) {
                                        case NUM_INT8:
                                                *((int8_t *)ptr)   = (int8_t)NUM(int8_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT8:
                                                *((uint8_t *)ptr)  = (uint8_t)NUM(int8_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_INT16:
                                                *((int16_t *)ptr)  = (int16_t)NUM(int8_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT16:
                                                *((uint16_t *)ptr) = (uint16_t)NUM(int8_t, sexp->atom.number.nptr);                                        
                                                break;
                                        case NUM_INT32:
                                                *((int32_t *)ptr)  = (int32_t)NUM(int8_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT32:
                                                *((uint32_t *)ptr) = (uint32_t)NUM(int8_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_INT64:
                                                *((int64_t *)ptr)  = (int64_t)NUM(int8_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT64:
                                                *((uint64_t *)ptr) = (uint64_t)NUM(int8_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_DOUBLE:
                                                *((double *)ptr)   = (double)NUM(int8_t, sexp->atom.number.nptr);
                                                break;
                                        default:
                                                abort ();
                                        }
                                        return (0);
                                case NUM_UINT8:
                                        switch (type) {
                                        case NUM_INT8:
                                                *((int8_t *)ptr)   = (int8_t)NUM(uint8_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT8:
                                                *((uint8_t *)ptr)  = (uint8_t)NUM(uint8_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_INT16:
                                                *((int16_t *)ptr)  = (int16_t)NUM(uint8_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT16:
                                                *((uint16_t *)ptr) = (uint16_t)NUM(uint8_t, sexp->atom.number.nptr);                                        
                                                break;
                                        case NUM_INT32:
                                                *((int32_t *)ptr)  = (int32_t)NUM(uint8_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT32:
                                                *((uint32_t *)ptr) = (uint32_t)NUM(uint8_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_INT64:
                                                *((int64_t *)ptr)  = (int64_t)NUM(uint8_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT64:
                                                *((uint64_t *)ptr) = (uint64_t)NUM(uint8_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_DOUBLE:
                                                *((double *)ptr)   = (double)NUM(uint8_t, sexp->atom.number.nptr);
                                                break;
                                        default:
                                                abort ();
                                        }
                                        return (0);
                                case NUM_INT16:
                                        switch (type) {
                                        case NUM_INT16:
                                                *((int16_t *)ptr)  = (int16_t)NUM(int16_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT16:
                                                *((uint16_t *)ptr) = (uint16_t)NUM(int16_t, sexp->atom.number.nptr);                                        
                                                break;
                                        case NUM_INT32:
                                                *((int32_t *)ptr)  = (int32_t)NUM(int16_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT32:
                                                *((uint32_t *)ptr) = (uint32_t)NUM(int16_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_INT64:
                                                *((int64_t *)ptr)  = (int64_t)NUM(int16_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT64:
                                                *((uint64_t *)ptr) = (uint64_t)NUM(int16_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_DOUBLE:
                                                *((double *)ptr)   = (double)NUM(int16_t, sexp->atom.number.nptr);
                                                break;
                                        default:
                                                abort ();
                                        }
                                        return (0);
                                case NUM_UINT16:
                                        switch (type) {
                                        case NUM_INT16:
                                                *((int16_t *)ptr)  = (int16_t)NUM(uint16_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT16:
                                                *((uint16_t *)ptr) = (uint16_t)NUM(uint16_t, sexp->atom.number.nptr);                                        
                                                break;
                                        case NUM_INT32:
                                                *((int32_t *)ptr)  = (int32_t)NUM(uint16_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT32:
                                                *((uint32_t *)ptr) = (uint32_t)NUM(uint16_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_INT64:
                                                *((int64_t *)ptr)  = (int64_t)NUM(uint16_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT64:
                                                *((uint64_t *)ptr) = (uint64_t)NUM(uint16_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_DOUBLE:
                                                *((double *)ptr)   = (double)NUM(uint16_t, sexp->atom.number.nptr);
                                                break;
                                        default:
                                                abort ();
                                        }
                                        return (0);
                                case NUM_INT32:
                                        switch (type) {
                                        case NUM_INT32:
                                                *((int32_t *)ptr)  = (int32_t)NUM(int32_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT32:
                                                *((uint32_t *)ptr) = (uint32_t)NUM(int32_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_INT64:
                                                *((int64_t *)ptr)  = (int64_t)NUM(int32_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT64:
                                                *((uint64_t *)ptr) = (uint64_t)NUM(int32_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_DOUBLE:
                                                *((double *)ptr)   = (double)NUM(int32_t, sexp->atom.number.nptr);
                                                break;
                                        default:
                                                abort ();
                                        }
                                        return (0);
                                case NUM_UINT32:
                                        switch (type) {
                                        case NUM_INT32:
                                                *((int32_t *)ptr)  = (int32_t)NUM(uint32_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT32:
                                                *((uint32_t *)ptr) = (uint32_t)NUM(uint32_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_INT64:
                                                *((int64_t *)ptr)  = (int64_t)NUM(uint32_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT64:
                                                *((uint64_t *)ptr) = (uint64_t)NUM(uint32_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_DOUBLE:
                                                *((double *)ptr)   = (double)NUM(uint32_t, sexp->atom.number.nptr);
                                                break;
                                        }
                                        return (0);
                                case NUM_INT64:
                                        switch (type) {
                                        case NUM_INT64:
                                                *((int64_t *)ptr)  = (int64_t)NUM(int64_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT64:
                                                *((uint64_t *)ptr) = (uint64_t)NUM(int64_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_DOUBLE:
                                                *((double *)ptr)   = (double)NUM(int64_t, sexp->atom.number.nptr);
                                                break;
                                        }
                                        return (0);
                                case NUM_UINT64:
                                        switch (type) {
                                        case NUM_INT64:
                                                *((int64_t *)ptr)  = (int64_t)NUM(uint64_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT64:
                                                *((uint64_t *)ptr) = (uint64_t)NUM(uint64_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_DOUBLE:
                                                *((double *)ptr)   = (double)NUM(uint64_t, sexp->atom.number.nptr);
                                                break;
                                        }
                                        return (0);
                                case NUM_DOUBLE:
                                        switch (type) {
                                        case NUM_DOUBLE:
                                                *((double *)ptr)   = (double)NUM(uint64_t, sexp->atom.number.nptr);
                                                break;
                                        }
                                        return (0);
                                }
                                return (0);
                        } else {
                                errno = EOVERFLOW;
                                return (1);
                        }
                } else {
                        /* not a number */
                        errno = EINVAL;
                        return (1);
                }
        } else {
                /* NULL */
                errno = EFAULT;
                return (1);
        }
}

int SEXP_number_cmp (const SEXP_t *a, const SEXP_t *b)
{
        return (-1);
}

SEXP_t *SEXP_number_newd (int n)
{
        SEXP_t *sexp;

        sexp = SEXP_new ();
        SEXP_SETTYPE(sexp, ATOM_NUMBER);
        NUM_STORE(int, n, sexp->atom.number.nptr);
        sexp->atom.number.type = NUM_INT;
        
        return (sexp);
}

SEXP_t *SEXP_number_newhd (short int n)
{
        SEXP_t *sexp;

        sexp = SEXP_new ();
        SEXP_SETTYPE(sexp, ATOM_NUMBER);
        NUM_STORE(short int, n, sexp->atom.number.nptr);
        sexp->atom.number.type = NUM_SHORTINT;
        
        return (sexp);
}

SEXP_t *SEXP_number_newhhd (char n)
{
        SEXP_t *sexp;

        sexp = SEXP_new ();
        SEXP_SETTYPE(sexp, ATOM_NUMBER);
        NUM_STORE(char, n, sexp->atom.number.nptr);
        sexp->atom.number.type = NUM_CHAR;
        
        return (sexp);
}

SEXP_t *SEXP_number_newld (long int n)
{
        SEXP_t *sexp;

        sexp = SEXP_new ();
        SEXP_SETTYPE(sexp, ATOM_NUMBER);
        NUM_STORE(long int, n, sexp->atom.number.nptr);
        sexp->atom.number.type = NUM_LONGINT;
        
        return (sexp);
}

SEXP_t *SEXP_number_newlld (long long int n)
{
        SEXP_t *sexp;

        sexp = SEXP_new ();
        SEXP_SETTYPE(sexp, ATOM_NUMBER);
        NUM_STORE(long long int, n, sexp->atom.number.nptr);
        sexp->atom.number.type = NUM_LONGLONGINT;

        return (sexp);
}

SEXP_t *SEXP_number_newu (unsigned int n)
{
        SEXP_t *sexp;

        sexp = SEXP_new ();
        SEXP_SETTYPE(sexp, ATOM_NUMBER);
        NUM_STORE(unsigned int, n, sexp->atom.number.nptr);
        sexp->atom.number.type = NUM_UINT;
        
        return (sexp);
}

SEXP_t *SEXP_number_newhu (unsigned short int n)
{
        SEXP_t *sexp;

        sexp = SEXP_new ();
        SEXP_SETTYPE(sexp, ATOM_NUMBER);
        NUM_STORE(unsigned short int, n, sexp->atom.number.nptr);
        sexp->atom.number.type = NUM_USHORTINT;
        
        return (sexp);
}

SEXP_t *SEXP_number_newhhu (unsigned char n)
{
        SEXP_t *sexp;

        sexp = SEXP_new ();
        SEXP_SETTYPE(sexp, ATOM_NUMBER);
        NUM_STORE(unsigned char, n, sexp->atom.number.nptr);
        sexp->atom.number.type = NUM_UCHAR;
        
        return (sexp);
}

SEXP_t *SEXP_number_newlu (unsigned long int n)
{
        SEXP_t *sexp;

        sexp = SEXP_new ();
        SEXP_SETTYPE(sexp, ATOM_NUMBER);
        NUM_STORE(unsigned long int, n, sexp->atom.number.nptr);
        sexp->atom.number.type = NUM_ULONGINT;
        
        return (sexp);
}

SEXP_t *SEXP_number_newllu (unsigned long long int n)
{
        SEXP_t *sexp;

        sexp = SEXP_new ();
        SEXP_SETTYPE(sexp, ATOM_NUMBER);
        NUM_STORE(unsigned long long int, n, sexp->atom.number.nptr);
        sexp->atom.number.type = NUM_ULONGLONGINT;
        
        return (sexp);
}

SEXP_t *SEXP_number_newf (double n)
{
        SEXP_t *sexp;

        sexp = SEXP_new ();
        SEXP_SETTYPE(sexp, ATOM_NUMBER);
        NUM_STORE(double, n, sexp->atom.number.nptr);
        sexp->atom.number.type = NUM_DOUBLE;

        return (sexp);
}

#if 0
SEXP_t *SEXP_number_newlf (long double n)
{
        SEXP_t *sexp;

        sexp = SEXP_new ();
        SEXP_SETTYPE(sexp, ATOM_NUMBER);
        
        return (sexp);
}
#endif /* 0 */

int SEXP_number_getd (const SEXP_t *sexp)
{
        if (sexp != NULL) {
                SEXP_VALIDATE(sexp);
                
                if (SEXP_TYPE(sexp) == ATOM_NUMBER) {
                        switch (sexp->atom.number.type) {
                        case NUM_INT:
                        case NUM_UINT:
                                return NUM(int, sexp->atom.number.nptr);
                        case NUM_INT8: {
                                int8_t n = NUM(int8_t, sexp->atom.number.nptr);
                                return (int)(n);
                        }
                        case NUM_UINT8: {
                                uint8_t n = NUM(uint8_t, sexp->atom.number.nptr);
                                return (int)(n);
                        }
                        case NUM_INT16: {
                                int16_t n = NUM(int16_t, sexp->atom.number.nptr);
                                return (int)(n);
                        }
                        case NUM_UINT16: {
                                uint16_t n = NUM(uint16_t, sexp->atom.number.nptr);
                                return (int)(n);
                        }
                        case NUM_NONE:
                                return (0);
                        default:
                                errno = EOVERFLOW;
                                return (0);
                        }
                } else {
                        errno = EINVAL;
                        return (0);
                }
        } else {
                errno = EFAULT;
                return (0);
        }
}

short int SEXP_number_gethd (const SEXP_t *sexp)
{
        if (sexp != NULL) {
                SEXP_VALIDATE(sexp);

                if (SEXP_TYPE(sexp) == ATOM_NUMBER) {
                        switch (sexp->atom.number.type) {
                        case NUM_INT8: {
                                int8_t n = NUM(int8_t, sexp->atom.number.nptr);
                                return (short int)(n);
                        }
                        case NUM_UINT8: {
                                uint8_t n = NUM(uint8_t, sexp->atom.number.nptr);
                                return (short int)(n);
                        }
                        case NUM_INT16:
                        case NUM_UINT16:
                                return NUM(short int, sexp->atom.number.nptr);
                        case NUM_NONE:
                                return (0);
                        default:
                                errno = EOVERFLOW;
                                return (0);
                        }
                } else {
                        errno = EINVAL;
                        return (0);
                }
        } else {
                errno = EFAULT;
                return (0);
        }
}

char SEXP_number_gethhd (const SEXP_t *sexp)
{
        if (sexp != NULL) {
                SEXP_VALIDATE(sexp);

                if (SEXP_TYPE(sexp) == ATOM_NUMBER) {
                        switch (sexp->atom.number.type) {
                        case NUM_INT8:
                                return NUM(char, sexp->atom.number.nptr);
                        case NUM_UINT8:
                                return (char)NUM(uint8_t, sexp->atom.number.nptr);
                        case NUM_NONE:
                                return (0);
                        default:
                                errno = EOVERFLOW;
                                return (0);
                        }
                } else {
                        errno = EINVAL;
                        return (0);
                }
        } else {
                errno = EFAULT;
                return (0);
        }
}

long int SEXP_number_getld (const SEXP_t *sexp)
{
        if (sexp != NULL) {
                SEXP_VALIDATE(sexp);

                if (SEXP_TYPE(sexp) == ATOM_NUMBER) {
                        switch (sexp->atom.number.type) {
                        case NUM_LONGINT:
                        case NUM_ULONGINT:
                                return NUM(long int, sexp->atom.number.nptr);
                        case NUM_INT8: {
                                int8_t n = NUM(int8_t, sexp->atom.number.nptr);
                                return (long int)(n);
                        }
                        case NUM_UINT8: {
                                uint8_t n = NUM(uint8_t, sexp->atom.number.nptr);
                                return (long int)(n);
                        }
                        case NUM_INT16: {
                                int16_t n = NUM(int16_t, sexp->atom.number.nptr);
                                return (long int)(n);
                        }
                        case NUM_UINT16: {
                                uint16_t n = NUM(uint16_t, sexp->atom.number.nptr);
                                return (long int)(n);
                        }
                        case NUM_NONE:
                                return (0);
                        default:
                                errno = EOVERFLOW;
                                return (0);
                        }
                } else {
                        errno = EINVAL;
                        return (0);
                }
        } else {
                errno = EFAULT;
                return (0);
        }
}

long long int SEXP_number_getlld (const SEXP_t *sexp)
{
        if (sexp != NULL) {
                SEXP_VALIDATE(sexp);

                if (SEXP_TYPE(sexp) == ATOM_NUMBER) {
                        switch (sexp->atom.number.type) {
                        case NUM_LONGLONGINT:
                        case NUM_ULONGLONGINT:
                                return NUM(long long int, sexp->atom.number.nptr);
                        case NUM_INT8: {
                                int8_t n = NUM(int8_t, sexp->atom.number.nptr);
                                return (long long int)(n);
                        }
                        case NUM_UINT8: {
                                uint8_t n = NUM(uint8_t, sexp->atom.number.nptr);
                                return (long long int)(n);
                        }
                        case NUM_INT16: {
                                int16_t n = NUM(int16_t, sexp->atom.number.nptr);
                                return (long long int)(n);
                        }
                        case NUM_UINT16: {
                                uint16_t n = NUM(uint16_t, sexp->atom.number.nptr);
                                return (long long int)(n);
                        }
                        case NUM_INT32: {
                                int32_t n = NUM(int32_t, sexp->atom.number.nptr);
                                return (long long int)(n);
                        }
                        case NUM_UINT32: {
                                uint32_t n = NUM(uint32_t, sexp->atom.number.nptr);
                                return (long long int)(n);
                        }
                        case NUM_DOUBLE:
                                errno = EDOM;
                                return (0);
                        case NUM_NONE:
                                return (0);
                        default:
                                errno = EOVERFLOW;
                                return (0);
                        }
                } else {
                        errno = EINVAL;
                        return (0);
                }
        } else {
                errno = EFAULT;
                return (0);
        }
}

unsigned int SEXP_number_getu (const SEXP_t *sexp)
{
        if (sexp != NULL) {
                SEXP_VALIDATE(sexp);

                if (SEXP_TYPE(sexp) == ATOM_NUMBER) {
                        switch (sexp->atom.number.type) {
                        case NUM_INT:
                        case NUM_UINT:
                                return NUM(unsigned int, sexp->atom.number.nptr);
                        case NUM_INT8: {
                                int8_t n = NUM(int8_t, sexp->atom.number.nptr);
                                return (unsigned int)(n);
                        }
                        case NUM_UINT8: {
                                uint8_t n = NUM(uint8_t, sexp->atom.number.nptr);
                                return (unsigned int)(n);
                        }
                        case NUM_INT16: {
                                int16_t n = NUM(int16_t, sexp->atom.number.nptr);
                                return (unsigned int)(n);
                        }
                        case NUM_UINT16: {
                                uint16_t n = NUM(uint16_t, sexp->atom.number.nptr);
                                return (unsigned int)(n);
                        }
                        case NUM_NONE:
                                return (0);
                        default:
                                errno = EOVERFLOW;
                                return (0);
                        }
                } else {
                        errno = EINVAL;
                        return (0);
                }
        } else {
                errno = EFAULT;
                return (0);
        }
}

unsigned short int SEXP_number_gethu (const SEXP_t *sexp)
{
        if (sexp != NULL) {
                SEXP_VALIDATE(sexp);
                
                if (SEXP_TYPE(sexp) == ATOM_NUMBER) {
                        switch (sexp->atom.number.type) {
                        case NUM_INT16:
                        case NUM_UINT16:
                                return NUM(unsigned short int, sexp->atom.number.nptr);
                        case NUM_INT8: {
                                int8_t n = NUM(int8_t, sexp->atom.number.nptr);
                                return (unsigned short int)(n);
                        }
                        case NUM_UINT8: {
                                uint8_t n = NUM(uint8_t, sexp->atom.number.nptr);
                                return (unsigned short int)(n);
                        }
                        case NUM_NONE:
                                return (0);
                        default:
                                errno = EOVERFLOW;
                                return (0);
                        }
                } else {
                        errno = EINVAL;
                        return (0);
                }
        } else {
                errno = EFAULT;
                return (0);
        }
}

unsigned char SEXP_number_gethhu (const SEXP_t *sexp)
{
        if (sexp != NULL) {
                SEXP_VALIDATE(sexp);

                if (SEXP_TYPE(sexp) == ATOM_NUMBER) {
                        switch (sexp->atom.number.type) {
                        case NUM_INT8:
                                return (unsigned char)NUM(char, sexp->atom.number.nptr);
                        case NUM_UINT8:
                                return NUM(unsigned char, sexp->atom.number.nptr);
                        case NUM_NONE:
                                return (0);
                        default:
                                errno = EOVERFLOW;
                                return (0);
                        }
                } else {
                        errno = EINVAL;
                        return (0);
                }
        } else {
                errno = EFAULT;
                return (0);
        }
}

unsigned long int SEXP_number_getlu (const SEXP_t *sexp)
{
        if (sexp != NULL) {
                SEXP_VALIDATE(sexp);

                if (SEXP_TYPE(sexp) == ATOM_NUMBER) {
                        switch (sexp->atom.number.type) {
                        case NUM_LONGINT:
                        case NUM_ULONGINT:
                                return NUM(unsigned long int, sexp->atom.number.nptr);
                        case NUM_INT8: {
                                int8_t n = NUM(int8_t, sexp->atom.number.nptr);
                                return (unsigned long int)(n);
                        }
                        case NUM_UINT8: {
                                uint8_t n = NUM(uint8_t, sexp->atom.number.nptr);
                                return (unsigned long int)(n);
                        }
                        case NUM_INT16: {
                                int16_t n = NUM(int16_t, sexp->atom.number.nptr);
                                return (unsigned long int)(n);
                        }
                        case NUM_UINT16: {
                                uint16_t n = NUM(uint16_t, sexp->atom.number.nptr);
                                return (unsigned long int)(n);
                        }
                        case NUM_NONE:
                                return (0);
                        default:
                                errno = EOVERFLOW;
                                return (0);
                        }
                } else {
                        errno = EINVAL;
                        return (0);
                }
        } else {
                errno = EFAULT;
                return (0);
        }
}

unsigned long long int SEXP_number_getllu (const SEXP_t *sexp)
{
        if (sexp != NULL) {
                SEXP_VALIDATE(sexp);
                
                if (SEXP_TYPE(sexp) == ATOM_NUMBER) {
                        switch (sexp->atom.number.type) {
                        case NUM_LONGLONGINT:
                        case NUM_ULONGLONGINT:
                                return NUM(unsigned long long int, sexp->atom.number.nptr);
                        case NUM_INT8: {
                                int8_t n = NUM(int8_t, sexp->atom.number.nptr);
                                return (unsigned long long int)(n);
                        }
                        case NUM_UINT8: {
                                uint8_t n = NUM(uint8_t, sexp->atom.number.nptr);
                                return (unsigned long long int)(n);
                        }
                        case NUM_INT16: {
                                int16_t n = NUM(int16_t, sexp->atom.number.nptr);
                                return (unsigned long long int)(n);
                        }
                        case NUM_UINT16: {
                                uint16_t n = NUM(uint16_t, sexp->atom.number.nptr);
                                return (unsigned long long int)(n);
                        }
                        case NUM_INT32: {
                                int32_t n = NUM(int32_t, sexp->atom.number.nptr);
                                return (unsigned long long int)(n);
                        }
                        case NUM_UINT32: {
                                uint32_t n = NUM(uint32_t, sexp->atom.number.nptr);
                                return (unsigned long long int)(n);
                        }
                        case NUM_DOUBLE:
                                errno = EDOM;
                                return (0);
                        case NUM_NONE:
                                return (0);
                        default:
                                errno = EOVERFLOW;
                                return (0);
                        }
                } else {
                        errno = EINVAL;
                        return (0);
                }
        } else {
                errno = EFAULT;
                return (0);
        }
}

double SEXP_number_getf (const SEXP_t *sexp)
{
        if (sexp != NULL) {
                SEXP_VALIDATE(sexp);
 
                if (SEXP_TYPE(sexp) == ATOM_NUMBER) {
                        switch (sexp->atom.number.type) {
                        case NUM_DOUBLE:
                                return NUM(double, sexp->atom.number.nptr);
                        default:
                                errno = EDOM;
                                return (0.0);
                        }
                } else {
                        errno = EINVAL;
                        return (0.0);
                }
        } else {
                errno = EFAULT;
                return (0.0);
        }
}

#if 0
long double SEXP_number_getlf (const SEXP_t *sexp)
{
        if (sexp != NULL) {
                SEXP_VALIDATE(sexp);
                
        } else {
                errno = EFAULT;
                return (0);
        }
}
#endif

SEXP_t *SEXP_string_new (const void *str, size_t len)
{
        SEXP_t *sexp;

        _A(str != NULL);
        /* _A(len > 0); */
        
        sexp = SEXP_new ();
        SEXP_SETTYPE(sexp, ATOM_STRING);
        sexp->atom.string.str = xmemdup (str, len);
        sexp->atom.string.len = len;
        
        return (sexp);
}

SEXP_t *SEXP_string_newf (const char *fmt, ...) {
        va_list ap;
        char *str = NULL;
        SEXP_t *sexp = NULL;
        
        va_start (ap, fmt);
        if (vasprintf (&str, fmt, ap) != -1) {
                sexp = SEXP_new ();
                SEXP_SETTYPE(sexp, ATOM_STRING);
                sexp->atom.string.str = str;
                sexp->atom.string.len = strlen (str);
        }
        va_end (ap);
        
        return (sexp);
}

int SEXP_string_cmp (const SEXP_t *a, const SEXP_t *b)
{
        if (a == NULL || b == NULL) {
                errno = EFAULT;
                return (-1);
        }
        
        SEXP_VALIDATE(a);
        SEXP_VALIDATE(b);

        if (SEXP_TYPE(a) == ATOM_STRING &&
            SEXP_TYPE(b) == ATOM_STRING)
        {
                register char *str_a, *str_b;
                register size_t len_min;

                len_min = (a->atom.string.len < b->atom.string.len ?
                           a->atom.string.len : b->atom.string.len);
                
                str_a = a->atom.string.str;
                str_b = b->atom.string.str;
                
                if (a->atom.string.len < b->atom.string.len) {
                        len_min = a->atom.string.len;
                        
                        while (len_min > 0) {
                                if (*str_a != *str_b)
                                        return (*str_a - *str_b);
                                
                                --len_min;
                                ++str_a;
                                ++str_b;
                        }

                        return ('\0' - *str_b);
                } else {
                        len_min = b->atom.string.len;
                        
                        while (len_min > 0) {
                                if (*str_a != *str_b)
                                        return (*str_a - *str_b);
                                
                                --len_min;
                                ++str_a;
                                ++str_b;
                        }

                        return (*str_a - '\0');
                }
        } else  {
                errno = EINVAL;
                return (-1);
        } 
        
        /* NOTREACHED */
        return (-1);
}

int SEXP_strcmp (const SEXP_t *sexp, const char *str)
{
        _A(str != NULL);
        
        if (sexp != NULL) {
                SEXP_VALIDATE(sexp);

                if (SEXP_TYPE(sexp) == ATOM_STRING) {
                        register char  *str_a, *str_b;
                        register size_t len_a;
                        
                        len_a = sexp->atom.string.len;
                        str_a = sexp->atom.string.str;
                        str_b = (char *)str;
                        
                        while (len_a > 0 && *str_b != '\0') {
                                if (*str_a != *str_b)
                                        return (int)(*str_a - *str_b);
                                --len_a;
                                ++str_a;
                                ++str_b;
                        }
                        
                        if (len_a > 0)
                                return (1);
                        else if (*str_b != '\0')
                                return (-1);
                        else
                                return (0);
                } else {
                        errno = EINVAL;
                        return (1);
                }
        } else {
                errno = EFAULT;
                return (-1);
        }
}

int SEXP_strncmp (const SEXP_t *sexp, const char *str, size_t n)
{
        _A(str != NULL);
        _A(n > 0);
        
        if (sexp != NULL) {
                SEXP_VALIDATE(sexp);
                
                if (SEXP_TYPE(sexp) == ATOM_STRING) {
                        return (n < sexp->atom.string.len ?
                                strncmp (sexp->atom.string.str, str, n):
                                strncmp (sexp->atom.string.str, str, sexp->atom.string.len));
                } else {
                        errno = EINVAL;
                        return (1);
                }
        } else {
                errno = EFAULT;
                return (-1);
        }
}

int SEXP_strncoll (const SEXP_t *sexp, const char *str, size_t n)
{
        _A(str != NULL);
        _A(n > 0);
        
        if (sexp != NULL) {
                SEXP_VALIDATE(sexp);
                
                if (SEXP_TYPE(sexp) == ATOM_STRING) {
                        return xstrncoll (sexp->atom.string.str,
                                          sexp->atom.string.len,
                                          str, n);
                } else {
                        errno = EINVAL;
                        return (1);
                }
        } else {
                /* NULL < str */
                errno = EFAULT;
                return (-1);
        }
}

int SEXP_stringp (const SEXP_t *sexp)
{
        if (sexp != NULL) {
                SEXP_VALIDATE(sexp);
                return (SEXP_TYPE(sexp) == ATOM_STRING);
        } else {
                errno = EFAULT;
                return (0);
        }
}

char *SEXP_string_cstr (const SEXP_t *sexp)
{
        char *str;
        size_t  i;
        
        if (sexp != NULL) {
                SEXP_VALIDATE(sexp);
                
                if (SEXP_TYPE(sexp) == ATOM_STRING) {
                        str = sm_alloc (sizeof (char) * (sexp->atom.string.len + 1));
                        
                        for (i = 0; (i < sexp->atom.string.len &&
                                     isprint(sexp->atom.string.str[i])); ++i)
                        {
                                str[i] = sexp->atom.string.str[i];
                        }
                        
                        str[i] = '\0';
                        
                        if (i < sexp->atom.string.len)
                                return sm_reallocf (str, sizeof (char) * (i + 1));
                        else
                                return (str);
                } else {
                        errno = EINVAL;
                        return (NULL);
                }
        } else {
                errno = EFAULT;
                return (NULL);
        }
}

const char *SEXP_string_cstrp (const SEXP_t *sexp)
{
        if (sexp == NULL) {
                errno = EFAULT;
                return (NULL);
        }
        
        if (SEXP_TYPE(sexp) != ATOM_STRING) {
                errno = EINVAL;
                return (NULL);
        }
        
        return (const char *)(sexp->atom.string.str);
}

SEXP_t *SEXP_string_substr (const SEXP_t *sexp, size_t off, size_t len)
{
        if (sexp == NULL) {
                errno = EFAULT;
                return (NULL);
        }
        
        SEXP_VALIDATE(sexp);
        
        if (SEXP_TYPE(sexp) != ATOM_STRING) {
                errno = EINVAL;
                return (NULL);
        }
        
        if ((off + len) > sexp->atom.string.len) {
                errno = EINVAL;
                return (NULL);
        }
        
        return (SEXP_string_new (sexp->atom.string.str + off, len));
}

char *SEXP_string_subcstr (const SEXP_t *sexp, size_t off, size_t len)
{
        char *substr;

        if (sexp == NULL) {
                errno = EFAULT;
                return (NULL);
        }
        
        SEXP_VALIDATE(sexp);
        
        if (SEXP_TYPE(sexp) != ATOM_STRING) {
                errno = EINVAL;
                return (NULL);
        }

        if ((off + len) > sexp->atom.string.len) {
                errno = EINVAL;
                return (NULL);
        }
        
        substr = sm_alloc (sizeof (char) * (len + 1));
        memcpy (substr, sexp->atom.string.str + off, sizeof (char) * len);
        substr[len] = '\0';
        
        return (substr);
}

size_t SEXP_string_length (const SEXP_t *sexp)
{
        if (sexp != NULL) {
                SEXP_VALIDATE(sexp);

                if (SEXP_TYPE(sexp) == ATOM_STRING) {
                        return ((size_t)(sexp->atom.string.len));
                } else {
                        errno = EINVAL;
                }
        } else {
                errno = EFAULT;
        }
        
        return ((size_t)-1);
}

SEXP_t *SEXP_list_new (void)
{
        SEXP_t *sexp;

        sexp = SEXP_new ();
        SEXP_SETTYPE(sexp, ATOM_LIST);
        sexp->atom.list.memb  = sm_alloc (sizeof (SEXP_t) * LIST_INIT_SIZE);
        sexp->atom.list.count = 0;
        sexp->atom.list.size  = LIST_INIT_SIZE;
        
        return (sexp);
}

SEXP_t *SEXP_list_init (SEXP_t *sexp)
{
        _A(sexp != NULL);

        SEXP_VALIDATE(sexp);
        SEXP_SETTYPE(sexp, ATOM_LIST);
        LIST_init (&(sexp->atom.list));
        
        return (sexp);
}

SEXP_t *SEXP_list_free (SEXP_t *sexp)
{
        SEXP_free (sexp);
        return (NULL);
}

SEXP_t *SEXP_list_free_nr (SEXP_t *sexp)
{
        _A(sexp != NULL);
        SEXP_VALIDATE(sexp);

        if (SEXP_TYPE(sexp) != ATOM_LIST) {
                errno = EINVAL;
                return (sexp);
        }
        
        sexp->atom.list.count = 0;
        return SEXP_list_free (sexp);
}

SEXP_t *LIST_add (LIST_t *list, SEXP_t *sexp)
{
        _A(list != NULL);
        _A(list->count <= list->size);
        _A(list->size > 0);

        SEXP_VALIDATE(sexp);
        
        if (list->count >= list->size) {
                list->size += LIST_GROW_ADD;
                list->memb  = sm_realloc (list->memb, sizeof (SEXP_t) * list->size);
        }
        
        memcpy (SEXP(list->memb) + list->count, sexp, sizeof (SEXP_t));
        
        ++(list->count);
        
        if (SEXP_FREE(sexp))
                sm_free (sexp);
        
        return (SEXP(list->memb) + list->count - 1);
}

SEXP_t *SEXP_list_add (SEXP_t *list, SEXP_t *sexp)
{
        if (list != NULL) {
                SEXP_VALIDATE(sexp);
                
                if (SEXP_listp (list)) {
                        return LIST_add (&(list->atom.list), sexp);
                } else {
                        errno = EINVAL;
                        return (NULL);
                }
        } else {
                errno = EFAULT;
                return (NULL);
        }
}

SEXP_t *SEXP_list_first (const SEXP_t *sexp)
{
        if (sexp != NULL) {
                SEXP_VALIDATE(sexp);
                
                if (SEXP_TYPE(sexp) == ATOM_LIST) {
                        if (sexp->atom.list.count > 0) {
                                return (&(SEXP(sexp->atom.list.memb)[0]));
                        } else {
                                return SEXP_new ();
                        }
                } else {
                        errno = EINVAL;
                        return (NULL);
                }
        } else {
                errno = EFAULT;
                return (NULL);
        }
}

SEXP_t *SEXP_list_last (const SEXP_t *sexp)
{
        if (sexp != NULL) {
                SEXP_VALIDATE(sexp);
                
                if (SEXP_TYPE(sexp) == ATOM_LIST) {
                        if (sexp->atom.list.count > 0) {
                                return (&(SEXP(sexp->atom.list.memb)[sexp->atom.list.count - 1]));
                        } else {
                                return SEXP_new ();
                        }
                } else {
                        errno = EINVAL;
                        return (NULL);
                }
        } else {
                errno = EFAULT;
                return (NULL);
        }
}

int SEXP_listp (const SEXP_t *sexp)
{
        if (sexp != NULL) {
                SEXP_VALIDATE(sexp);
                return (SEXP_TYPE(sexp) == ATOM_LIST);
        } else {
                errno = EFAULT;
                return (0);
        }
}

SEXP_t *SEXP_list_pop (SEXP_t **sexp)
{
        SEXP_t *sexp_ret;

        if (sexp != NULL) {
                _A(*sexp != NULL);
                if (SEXP_TYPE(*sexp) == ATOM_LIST) {
                        if ((*sexp)->atom.list.count > 0) {
                                sexp_ret = SEXP_dup(&(SEXP((*sexp)->atom.list.memb)[0]));
                                
                                if ((*sexp)->atom.list.count > 1) {
                                        --(*sexp)->atom.list.count;
                                        
                                        memmove (SEXP((*sexp)->atom.list.memb),
                                                 SEXP((*sexp)->atom.list.memb) + 1,
                                                 sizeof (SEXP_t) * ((*sexp)->atom.list.count));
                                        
                                        (*sexp)->atom.list.memb = sm_realloc ((*sexp)->atom.list.memb,
                                                                              sizeof (SEXP_t) * ((*sexp)->atom.list.count));
                                } else {
                                        sm_free ((*sexp)->atom.list.memb);
                                        
                                        if (SEXP_FREE(*sexp))
                                                sm_free (*sexp);
                                        
                                        *sexp = NULL;
                                }
                                return (sexp_ret);
                        } else {
                                if (SEXP_FREE(*sexp))
                                        sm_free (*sexp);
                                
                                *sexp = NULL;
                                
                                return (NULL);
                        }
                } else {
                        errno = EINVAL;
                        return (NULL);
                }
        } else {
                errno = EFAULT;
                return (NULL);
        }
}

SEXP_t *SEXP_list_nth (const SEXP_t *sexp, uint32_t n)
{
        _A(n > 0);
        
        if (sexp != NULL) {
                if (SEXP_TYPE(sexp) == ATOM_LIST && n > 0) {
                        _D("nth: %u, %p\n", n, sexp);
                        
                        if (n <= sexp->atom.list.count) {
                                _D("nth: %u, %p, %s\n",
                                   n, &(SEXP(sexp->atom.list.memb)[n - 1]),
                                   SEXP_strtype (&(SEXP(sexp->atom.list.memb)[n - 1])));
                                
                                return &(SEXP(sexp->atom.list.memb)[n - 1]);
                        }

                        return (NULL);
                } else {
                        errno = EINVAL;
                        return (NULL);
                }
        } else {
                errno = EFAULT;
                return (NULL);
        }
}

SEXP_t *SEXP_list_nth_dup (const SEXP_t *sexp, uint32_t n)
{
        _A(n > 0);
        
        if (sexp != NULL) {
                if (SEXP_listp (sexp) && n > 0) {
                        if (sexp->atom.list.count <= n)
                                return SEXP_dup(&(SEXP(sexp->atom.list.memb)[n - 1]));
                        else
                                return (NULL);
                } else {
                        errno = EINVAL;
                        return (NULL);
                }
        } else {
                errno = EFAULT;
                return (NULL);
        }
}

SEXP_t *SEXP_list_nth_deepdup (const SEXP_t *sexp, uint32_t n)
{
        _A(n > 0);
        
        if (sexp != NULL) {
                if (SEXP_listp (sexp) && n > 0) {
                        if (sexp->atom.list.count <= n)
                                return SEXP_deepdup(&(SEXP(sexp->atom.list.memb)[n - 1]));
                        else
                                return (NULL);
                } else {
                        errno = EINVAL;
                        return (NULL);
                }
        } else {
                errno = EFAULT;
                return (NULL);
        }
}

SEXP_t *SEXP_list_join (SEXP_t *a, SEXP_t *b)
{
        if (a != NULL && b != NULL) {
                SEXP_VALIDATE(a);
                SEXP_VALIDATE(b);
                
                if (SEXP_listp (a) && SEXP_listp (b)) {
                        if (a->atom.list.size < b->atom.list.count) {
                                a->atom.list.size = a->atom.list.count + b->atom.list.count;
                                a->atom.list.memb = sm_realloc (a->atom.list.memb,
                                                                sizeof (SEXP_t) * (a->atom.list.count + b->atom.list.count));
                                
                        }
                        
                        memcpy (SEXP(a->atom.list.memb) + a->atom.list.count,
                                b->atom.list.memb, sizeof (SEXP_t) * b->atom.list.count);
                        
                        a->atom.list.count += b->atom.list.count;
                        
                        return (a);
                } else {
                        errno = EINVAL;
                        return (NULL);
                }
        } else {
                errno = EFAULT;
                return (NULL);
        }
}

size_t SEXP_list_length (const SEXP_t *sexp)
{
        if (sexp != NULL) {
                SEXP_VALIDATE(sexp);
                
                if (SEXP_TYPE(sexp) == ATOM_LIST) {
                        return ((size_t)(sexp->atom.list.count));
                } else {
                        errno = EINVAL;
                }
        } else {
                errno = EFAULT;
        }
        
        return ((size_t)-1);
}

SEXP_t *SEXP_list_map (SEXP_t *list, int (*fn) (SEXP_t *, SEXP_t *))
{
        int e;
        SEXP_t *res_list;
        uint32_t i;
        
        if (list != NULL) {
                SEXP_VALIDATE(list);
                
                if (SEXP_TYPE(list) == ATOM_LIST) {
                        res_list = SEXP_new ();
                        SEXP_SETTYPE(res_list, ATOM_LIST);
                        res_list->atom.list.size  = list->atom.list.size;
                        res_list->atom.list.count = list->atom.list.count;
                        res_list->atom.list.memb  = sm_alloc (sizeof (SEXP_t) * res_list->atom.list.size);
                        
                        for (i = 0; i < list->atom.list.count; ++i) {
                                if (fn (&(SEXP(res_list->atom.list.memb)[i]),
                                        &(SEXP(list->atom.list.memb)[i])) != 0)
                                {
                                        e = errno;
                                        goto err;
                                }
                        }
                        
                        return (res_list);
                } else {
                        errno = EINVAL;
                }
        } else {
                errno = EFAULT;
        }
        
        return (NULL);
err:
        SEXP_free (res_list);
        errno = e;
        return (NULL);
}

SEXP_t *SEXP_list_map2 (SEXP_t *list, int (*fn) (SEXP_t *, SEXP_t *, void *), void *ptr)
{
        int e;
        SEXP_t *res_list;
        uint32_t i;
        
        if (list != NULL) {
                SEXP_VALIDATE(list);
                
                if (SEXP_TYPE(list) == ATOM_LIST) {
                        res_list = SEXP_new ();
                        SEXP_SETTYPE(res_list, ATOM_LIST);
                        res_list->atom.list.size  = list->atom.list.size;
                        res_list->atom.list.count = list->atom.list.count;
                        res_list->atom.list.memb  = sm_alloc (sizeof (SEXP_t) * res_list->atom.list.size);
                        
                        for (i = 0; i < list->atom.list.count; ++i) {
                                if (fn (&(SEXP(res_list->atom.list.memb)[i]),
                                        &(SEXP(list->atom.list.memb)[i]),
                                        ptr) != 0)
                                {
                                        e = errno;
                                        goto err;
                                }
                        }
                        
                        return (res_list);
                } else {
                        errno = EINVAL;
                }
        } else {
                errno = EFAULT;
        }
        
        return (NULL);
err:
        SEXP_free (res_list);
        errno = e;
        return (NULL);
}

SEXP_t *SEXP_list_reduce (SEXP_t *list, SEXP_t *(*fn) (const SEXP_t *, const SEXP_t *), int strategy)
{
        SEXP_t *res;
        size_t  i;

        if (list == NULL) {
                errno = EFAULT;
                return (NULL);
        }

        SEXP_VALIDATE(list);

        if (SEXP_TYPE(list) != ATOM_LIST) {
                errno = EINVAL;
                return (NULL);
        }

        switch (list->atom.list.count) {
        case 0:
                res = SEXP_new ();
                SEXP_SETTYPE(res, ATOM_EMPTY);
                
                return (res);
        case 1:
                return SEXP_list_nth_deepdup (list, 1);
        default:
                switch (strategy) {
                case SEXP_REDUCE_LNEIGHBOR:
                        
                        break;
                case SEXP_REDUCE_RNEIGHBOR:
                        
                        break;
                case SEXP_REDUCE_RANDOM: {
                        
                }
                        break;
                case SEXP_REDUCE_PARALLEL:
#if defined(_OPENMP)
                        return (NULL);
#else
                        _D("Invalid reduce strategy: %d: available only if compiled with OpenMP.\n",
                           strategy);
                        errno = EOPNOTSUPP;
                        return (NULL);
#endif
                        break;
                default:
                        _D("Unkwnown reduce strategy: %d\n", strategy);
                        errno = EINVAL;
                        return (NULL);
                }
        }
        
        /* NOTREACHED */
        abort ();
        return (NULL);
}

SEXP_t *SEXP_list_reduce2 (SEXP_t *list, SEXP_t *(*fn) (const SEXP_t *, const SEXP_t *, void *), int strategy, void *ptr)
{
        SEXP_VALIDATE(list);

        return (NULL);
}

void SEXP_list_cb (SEXP_t *list, void (*fn) (SEXP_t *, void *), void *ptr)
{
        SEXP_VALIDATE(list);

        return;
}

SEXP_t *SEXP_list_sort (SEXP_t *list, int (*cmp) (const SEXP_t *, const SEXP_t *))
{
        if (list != NULL) {
                if (SEXP_TYPE(list) == ATOM_LIST) {
                        
                        qsort (list->atom.list.memb,
                               list->atom.list.count,
                               sizeof (SEXP_t),
                               (int (*)(const void *, const void *))(cmp == NULL ? &SEXP_cmp : cmp));
                        
                        return (list);
                } else {
                        errno = EINVAL;
                }
        } else {
                errno = EFAULT;
        }
        
        return (NULL);
}

int SEXP_list_cmp (const SEXP_t *a, const SEXP_t *b)
{
        return (-1);
}

SEXP_t *SEXP_new  (void)
{
        SEXP_t *sexp;

        sexp = sm_talloc (SEXP_t);
        sexp->flags = 0;
        SEXP_SETTYPE(sexp, ATOM_EMPTY);
        SEXP_SETFLAG(sexp, SEXP_FLAGFREE);
        sexp->handler = NULL;
#if !defined(NDEBUG)
        sexp->__magic0 = SEXP_MAGIC0;
        sexp->__magic1 = SEXP_MAGIC1;
#endif
        return (sexp);
}

void SEXP_init (SEXP_t *sexp)
{
        _A(sexp != NULL);
        
        sexp->flags = 0;
        SEXP_SETTYPE(sexp, ATOM_EMPTY);
        sexp->handler = NULL;
#if !defined(NDEBUG)
        sexp->__magic0 = SEXP_MAGIC0;
        sexp->__magic1 = SEXP_MAGIC1;
#endif
        return;
}

void SEXP_free (SEXP_t *sexp)
{
        _A(sexp != NULL);
        
        SEXP_VALIDATE(sexp);
        
        switch (SEXP_TYPE(sexp)) {
        case ATOM_LIST:
        {
                uint32_t i;
                
                for (i = 0; i < (sexp->atom.list.count); ++i) {
                        SEXP_free (&SEXP(sexp->atom.list.memb)[i]);
                }
                
                if (sexp->atom.list.size > 0) {
                        sm_free (sexp->atom.list.memb);
                }
                
        } break;
        case ATOM_NUMBER:
        {
                uint16_t sz;
                
                switch (sexp->atom.number.type) {
                case NUM_INT8:   sz = sizeof (int8_t);
                        break;
                case NUM_UINT8:  sz = sizeof (uint8_t);
                        break;
                case NUM_INT16:  sz = sizeof (int16_t);
                        break;
                case NUM_UINT16: sz = sizeof (uint16_t);
                        break;
                case NUM_INT32:  sz = sizeof (int32_t);
                        break;
                case NUM_UINT32: sz = sizeof (uint32_t);
                        break;
                case NUM_INT64:  sz = sizeof (int64_t);
                        break;
                case NUM_UINT64: sz = sizeof (uint64_t);
                        break;
                case NUM_DOUBLE: sz = sizeof (double);
                        break;
                case NUM_NONE:
#if 0
                case NUM_FRACT: /* Not implemented */
                case NUM_BIGNUM: /* Not implemented */
                        sz = 0;
#endif
                        break;
                default:
                        abort ();
                }

                if (sz > VOIDPTR_SIZE)
                        sm_free (sexp->atom.number.nptr);
                
        } break;
        case ATOM_STRING:
 
                if (sexp->atom.string.len > 0)
                        sm_free (sexp->atom.string.str);
                
                break;
        case ATOM_UNFIN:
        case ATOM_EMPTY:
        case ATOM_INVAL:
                break;
        default:
                abort ();
        }

#if !defined(NDEBUG) || defined(VALIDATE_SEXP)
        memset (sexp, 0, sizeof (SEXP_t));
        sexp->__magic0 = SEXP_MAGIC0_INV;
        sexp->__magic1 = SEXP_MAGIC1_INV;
#endif

        if (SEXP_FREE(sexp))
                sm_free (sexp);
        
        return;
}

SEXP_t *SEXP_deepdup (const SEXP_t *sexp)
{
        SEXP_t *copy;

        if (sexp != NULL) {
                SEXP_VALIDATE(sexp);
                
                copy = SEXP_new ();
                copy->flags = sexp->flags;
                copy->handler = sexp->handler;
                
                switch (SEXP_TYPE(sexp)) {
                case ATOM_UNFIN:
                case ATOM_EMPTY:
                        break;
                case ATOM_NUMBER:
                        
                        break;
                case ATOM_LIST:
                        
                        break;
                case ATOM_STRING:
                        
                        break;
                default:
                        abort ();
                }
                
                return (copy);
        } else {
                errno = EFAULT;
                return (NULL);
        }
}

SEXP_t *SEXP_dup (const SEXP_t *sexp)
{
        SEXP_t *copy;
        
        if (sexp != NULL) {
                SEXP_VALIDATE(sexp);
                
                copy = SEXP_new ();
                copy->flags   = sexp->flags;
                copy->handler = sexp->handler;
                memcpy (&(copy->atom), &(sexp->atom), sizeof sexp->atom);
                
                return (copy);
        } else {
                errno = EFAULT;
                return (NULL);
        }
}

SEXP_t *SEXP_copy (SEXP_t *dst, const SEXP_t *src)
{
        if (dst != NULL && src != NULL) {
                SEXP_VALIDATE(src);
                return memcpy (dst, src, sizeof (SEXP_t));
        } else {
                errno = EFAULT;
                return (NULL);
        }
}

SEXP_t *SEXP_deepcopy (SEXP_t *dst, const SEXP_t *src)
{
        if (dst != NULL && src != NULL) {
                SEXP_VALIDATE(src);
                return (NULL);
        } else {
                errno = EFAULT;
                return (NULL);
        }
}

size_t SEXP_length (const SEXP_t *sexp)
{
        if (sexp != NULL) {
                switch (SEXP_TYPE(sexp)) {
                case ATOM_LIST:
                        return ((size_t)(sexp->atom.list.count));
                case ATOM_STRING:
                        return ((size_t)(sexp->atom.string.len));
                case ATOM_NUMBER:
                default:
                        errno = EINVAL;
                }
        } else {
                errno = EFAULT;
        }

        return ((size_t)-1);
}

int SEXP_cmp (const SEXP_t *a, const SEXP_t *b)
{   
        if ((a == NULL) ^ (b == NULL)) {
                errno = EFAULT;
                return (-1);
        }
        
        if (a == NULL) {
                errno = EFAULT;
                return (0);
        }
        
        SEXP_VALIDATE(a);
        SEXP_VALIDATE(b);
        
        if (SEXP_TYPE(a) != SEXP_TYPE(b)) {
                errno = EINVAL;
                return (-1);
        }
        
        switch (SEXP_TYPE(a)) {
        case ATOM_STRING:
                return SEXP_string_cmp (a, b);
        case ATOM_NUMBER:
                return SEXP_number_cmp (a, b);
        case ATOM_LIST:
                return SEXP_list_cmp (a, b);
        case ATOM_EMPTY:
                return (0);
        default:
                return (1);
        }
        
        /* NOTREACHED */
        abort ();
        return (-1);
}

int SEXP_cmpobj (const SEXP_t *a, const SEXP_t *b)
{
        if ((a == NULL) ^ (b == NULL)) {
                errno = EFAULT;
                return (-1);
        }

        if (a == NULL) {
                errno = EFAULT;
                return (0);
        }

        SEXP_VALIDATE(a);
        SEXP_VALIDATE(b);
        
        return memcmp (a, b, sizeof (SEXP_t));
}

static const char *__sexp_strtype[] = {
        "unfinished",
        "list",
        "number",
        "symbol",
        "string",
        "binary",
        "empty"
};

const char *SEXP_strtype (const SEXP_t *sexp)
{
        if (sexp != NULL) {
                _A(SEXP_TYPE(sexp) >= 0);
                return (SEXP_TYPE(sexp) < 0x06 ? __sexp_strtype[SEXP_TYPE(sexp)] : "unknown");
        } else {
                errno = EFAULT;
                return ("(null)");
        }
}

#if !defined(NDEBUG) || defined(VALIDADE_SEXP)
#include <stdio.h>

inline void __SEXP_VALIDATE(const SEXP_t *ptr, const char *fn, size_t line) {
        if (ptr == NULL) {
                fprintf (stderr, "[%zu:%s]: !!! NULL S-EXP OBJECT !!!\n", line, fn);
        } else {
                if ((ptr->__magic0 == SEXP_MAGIC0) &&
                    (ptr->__magic1 == SEXP_MAGIC1)) {
                        switch (SEXP_TYPE(ptr))
                        {
                        case ATOM_UNFIN:
                        case ATOM_INVAL:
                        case ATOM_EMPTY:
                                return;
                        case ATOM_LIST:
                                if (ptr->atom.list.size > __VALIDATE_TRESH_LIST_SIZE) {
                                        fprintf (stderr, "[%zu:%s] !!! LIST SIZE TRESHOLD EXCEEDED !!!\n", line, fn);
                                        break;
                                }
                                if (ptr->atom.list.count > __VALIDATE_TRESH_LIST_COUNT) {
                                        fprintf (stderr, "[%zu:%s] !!! LIST COUNT TRESHOLD EXCEEDED !!!\n", line, fn);
                                        break;
                                }
                                return;
                        case ATOM_NUMBER:
                                switch (ptr->atom.number.type)
                                {
                                case NUM_NONE:
                                case NUM_INT8:
                                case NUM_UINT8:
                                case NUM_INT16:
                                case NUM_UINT16:
                                case NUM_INT32:
                                case NUM_UINT32:
                                case NUM_INT64:
                                case NUM_UINT64:
                                case NUM_DOUBLE:
#if 0
                                case NUM_FRACT:
                                case NUM_BIGNUM:
#endif
                                        return;
                                }
                                fprintf (stderr, "[%zu:%s] !!! INVALID NUMBER TYPE !!!\n", line, fn);
                                break;
                        case ATOM_STRING:
                                if (ptr->atom.string.len <= __VALIDATE_TRESH_STRING_LEN)
                                        return;
                                fprintf (stderr, "[%zu:%s] !!! STRING LENGTH TRESHOLD EXCEEDED !!!\n", line, fn);
                                break;
                        default:
                                fprintf (stderr, "[%zu:%s] !!! INVALID S-EXP OBJECT TYPE !!!\n", line, fn);
                        }
                }
                fprintf (stderr, "[%zu:%s]: !!! CORRUPTED S-EXP OBJECT !!! (p=%p, t=%u)\n", line, fn, ptr, SEXP_TYPE(ptr));
        }
        abort ();
}
#endif
