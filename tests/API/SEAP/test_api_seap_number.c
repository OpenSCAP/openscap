
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sexp.h>
#include <string.h>
#include <stdint.h>

int main (void)
{
        SEXP_t *s_exp;

        int8_t   i8 = -1;
        uint8_t  u8 =  1;
        int16_t  i16 = -32000;
        uint16_t u16 =  65000;
        int32_t  i32 = -100000;
        uint32_t u32 =  100000;
        int64_t  i64 = -1 * (1 << 30);
        uint64_t u64 = (uint64_t)1 << 48;
        double   f   = 123.456;

        s_exp = SEXP_number_newi_8 (i8);
        SEXP_fprintfa (stdout, s_exp);
        putc('\n', stdout);
        SEXP_free (s_exp);

        s_exp = SEXP_number_newu_8 (u8);
        SEXP_fprintfa (stdout, s_exp);
        putc('\n', stdout);
        SEXP_free (s_exp);

        s_exp = SEXP_number_newi_16 (i16);
        SEXP_fprintfa (stdout, s_exp);
        putc('\n', stdout);
        SEXP_free (s_exp);

        s_exp = SEXP_number_newu_16 (u16);
        SEXP_fprintfa (stdout, s_exp);
        putc('\n', stdout);
        SEXP_free (s_exp);

        s_exp = SEXP_number_newi_32 (i32);
        SEXP_fprintfa (stdout, s_exp);
        putc('\n', stdout);
        SEXP_free (s_exp);

        s_exp = SEXP_number_newu_32 (u32);
        SEXP_fprintfa (stdout, s_exp);
        putc('\n', stdout);
        SEXP_free (s_exp);

        s_exp = SEXP_number_newi_64 (i64);
        SEXP_fprintfa (stdout, s_exp);
        putc('\n', stdout);
        SEXP_free (s_exp);

        s_exp = SEXP_number_newu_64 (u64);
        SEXP_fprintfa (stdout, s_exp);
        putc('\n', stdout);
        SEXP_free (s_exp);

        s_exp = SEXP_number_newf (f);
        SEXP_fprintfa (stdout, s_exp);
        putc('\n', stdout);
        SEXP_free (s_exp);

        return (0);
}
