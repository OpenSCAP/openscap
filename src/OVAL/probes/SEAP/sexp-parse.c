#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <errno.h>
#include <config.h>
#include "generic/common.h"
#include "public/sm_alloc.h"
#include "_sexp-types.h"
#include "_sexp-manip.h"
#include "_sexp-parse.h"
#include "_sexp-datatype.h"
#include "_sexp-value.h"
#include "generic/xbase64.h"
#include "generic/strto.h"

SEXP_t *SEXP_parse_fd (SEXP_format_t fmt, int fd, size_t max, SEXP_pstate_t **state)
{
        _A(fd >= 0);
        errno = EOPNOTSUPP;
        return (NULL);
}

SEXP_t *SEXP_parse_buf (SEXP_format_t fmt, void *buf, size_t len, SEXP_pstate_t **state)
{
        _A(buf != NULL);
        _A(len  > 0);
        errno = EOPNOTSUPP;
        return (NULL);
}

#define SEXP_PLSTACK_INIT_SIZE      32
#define SEXP_PLSTACK_GROWFAST_TRESH 512
#define SEXP_PLSTACK_GROWSLOW_DIFF  32

SEXP_pstate_t *SEXP_pstate_init (SEXP_pstate_t *state)
{
        _A(state != NULL);

        state->buffer = NULL;
        state->buffer_data_len = 0;
        state->buffer_fail_off = 0;

        state->p_list = SEXP_list_new (NULL);
        state->l_size = SEXP_PLSTACK_INIT_SIZE;
        state->l_real = 1;
        state->l_sref = sm_alloc (sizeof (SEXP_t *) * SEXP_PLSTACK_INIT_SIZE);
        
        state->l_sref[0] = SEXP_softref (state->p_list);
        
        return (state);
}

SEXP_pstate_t *SEXP_pstate_new (void)
{
        return SEXP_pstate_init (sm_talloc (SEXP_pstate_t));
}

SEXP_t *SEXP_pstate_lstack_push (SEXP_pstate_t *state, SEXP_t *ref)
{
        if (state->l_real == state->l_size) {
                if (state->l_size < SEXP_PLSTACK_GROWFAST_TRESH)
                        state->l_size <<= 1;
                else
                        state->l_size  += SEXP_PLSTACK_GROWSLOW_DIFF;
                
                state->l_sref = sm_realloc (state->l_sref, sizeof (SEXP_t *) * state->l_size);
        }
        
        state->l_sref[state->l_real++] = ref;
        
        return (ref);
}

SEXP_t *SEXP_pstate_lstack_pop (SEXP_pstate_t *state)
{
        SEXP_t *ref;
        size_t  diff;
        
        ref  = state->l_sref[--state->l_real];
        diff = state->l_size - state->l_real;
        
        if (state->l_size > SEXP_PLSTACK_GROWFAST_TRESH) {
                if (diff >= SEXP_PLSTACK_GROWSLOW_DIFF) {
                        state->l_size  -= SEXP_PLSTACK_GROWSLOW_DIFF;
                        goto resize;
                }
        } else {
                if (diff >= 2 * state->l_real) {
                        state->l_size >>= 1;
                        goto resize;
                }
        }

        return (ref);
resize:
        state->l_sref = sm_realloc (state->l_sref, sizeof (SEXP_t *) * state->l_size);
        return (ref);
}

SEXP_t *SEXP_pstate_lstack_top (SEXP_pstate_t *state)
{
        return (state->l_sref[state->l_real - 1]);
}

void SEXP_pstate_free (SEXP_pstate_t *p)
{
        return;
}

SEXP_psetup_t *SEXP_psetup_new (void)
{
        SEXP_psetup_t *psetup;

        psetup = sm_talloc (SEXP_psetup_t);
        psetup->fmt    = SEXP_FMT_AUTODETECT;
        psetup->pflags = PF_EOFOK;
        
        return (psetup);
}

void SEXP_psetup_init (SEXP_psetup_t *psetup)
{
        _A(psetup != NULL);

        psetup->fmt = SEXP_FMT_AUTODETECT;
        psetup->pflags = PF_EOFOK;
}

void SEXP_psetup_free (SEXP_psetup_t *p)
{
        return;
}

/*
 *  Parser: label
 */

static inline int isnexttok (int c)
{
        switch (c) {
        case ' ' :
        case '\t':
        case '\n':
        case '\r':
        case '\a':
        case '(' :
        case ')' :
        case '"' :
        case '\'':
                return (1);
        default:
                return (0);
        }
}

#define EXF_EOFOK PF_EOFOK
#define EXF_ALL   0xff

/*
 * Defines a function for parsing the expected type of object
 * that the main parser encountered in the buffer. The string
 * that represents the object (token) in the stream can have
 * any lenght. It is the responbility of the function to find
 * out the lenght of the token and this lenght should be the
 * return value of this function.
 */
#define EXTRACTOR(name)      SEXP_extract_##name
#define DEFEXTRACTOR(what)   static inline int EXTRACTOR(what) (struct SEXP_pext_dsc *dsc)

#define EXTRACTOR_F(name)    SEXP_extract_fixedlen_##name
#define DEFEXTRACTOR_F(what) static inline int EXTRACTOR_F(what) (struct SEXP_pext_dsc *dsc)

DEFEXTRACTOR(si_string);
DEFEXTRACTOR(dq_string);
DEFEXTRACTOR(sq_string);
DEFEXTRACTOR_F(string);

DEFEXTRACTOR(b64_string);
DEFEXTRACTOR_F(b64_string);

DEFEXTRACTOR(datatype);
DEFEXTRACTOR_F(datatype);

DEFEXTRACTOR(hexstring);
DEFEXTRACTOR_F(hexstring);

SEXP_t *SEXP_parse (const SEXP_psetup_t *setup, const char *buf, size_t buflen, SEXP_pstate_t **statep)
{
        static const void *labels[] = {
                /* 000 NUL (Null char.)               */ &&L_NUL,
                /* 001 SOH (Start of Header)          */ &&L_INVALID,
                /* 002 STX (Start of Text)            */ &&L_INVALID,
                /* 003 ETX (End of Text)              */ &&L_INVALID,
                /* 004 EOT (End of Transmission)      */ &&L_INVALID,
                /* 005 ENQ (Enquiry)                  */ &&L_INVALID,
                /* 006 ACK (Acknowledgment)           */ &&L_INVALID,
                /* 007 BEL (Bell)                     */ &&L_INVALID,
                /* 008  BS (Backspace)                */ &&L_INVALID,
                /* 009  HT (Horizontal Tab)           */ &&L_WHITESPACE,
                /* 010  LF (Line Feed)                */ &&L_WHITESPACE,
                /* 011  VT (Vertical Tab)             */ &&L_WHITESPACE,
                /* 012  FF (Form Feed)                */ &&L_WHITESPACE,
                /* 013  CR (Carriage Return)          */ &&L_WHITESPACE,
                /* 014  SO (Shift Out)                */ &&L_INVALID,
                /* 015  SI (Shift In)                 */ &&L_INVALID,
                /* 016 DLE (Data Link Escape)         */ &&L_INVALID,
                /* 017 DC1 (Device Control 1 - XON)   */ &&L_INVALID,
                /* 018 DC2 (Device Control 2)         */ &&L_INVALID,
                /* 019 DC3 (Device Control 3 - XOFF)  */ &&L_INVALID,
                /* 020 DC4 (Device Control 4)         */ &&L_INVALID,
                /* 021 NAK (Negativ Acknowledgemnt)   */ &&L_INVALID,
                /* 022 SYN (Synchronous Idle)         */ &&L_INVALID,
                /* 023 ETB (End of Trans. Block)      */ &&L_INVALID,
                /* 024 CAN (Cancel)                   */ &&L_INVALID,
                /* 025  EM (End of Medium)            */ &&L_INVALID,
                /* 026 SUB (Substitute)               */ &&L_INVALID,
                /* 027 ESC (Escape)                   */ &&L_INVALID,
                /* 028  FS (File Separator)           */ &&L_INVALID,
                /* 029  GS (Group Separator)          */ &&L_INVALID,
                /* 030  RS (Reqst to Send)(Rec. Sep.) */ &&L_INVALID,
                /* 031  US (Unit Separator)           */ &&L_INVALID,
                /* 032  SP (Space)                    */ &&L_WHITESPACE,
                /* 033   ! (exclamation mark)         */ &&L_CHAR,
                /* 034   " (double quote)             */ &&L_DQUOTE,
                /* 035   # (number sign)              */ &&L_HASH,
                /* 036   $ (dollar sign)              */ &&L_CHAR,
                /* 037   % (percent)                  */ &&L_CHAR,
                /* 038   & (ampersand)                */ &&L_CHAR,
                /* 039   ' (single quote)             */ &&L_SQUOTE,
                /* 040   (  (left/open parenthesis)   */ &&L_PAROPEN,
                /* 041   )  (right/closing parenth.)  */ &&L_PARCLOSE,
                /* 042   * (asterisk)                 */ &&L_CHAR,
                /* 043   + (plus)                     */ &&L_PLUS,
                /* 044   , (comma)                    */ &&L_INVALID,
                /* 045   - (minus or dash)            */ &&L_MINUS,
                /* 046   . (dot)                      */ &&L_DOT,
                /* 047   / (forward slash)            */ &&L_CHAR,
                /* 048   0                            */ &&L_NUMBER,
                /* 049   1                            */ &&L_NUMBER,
                /* 050   2                            */ &&L_NUMBER,
                /* 051   3                            */ &&L_NUMBER,
                /* 052   4                            */ &&L_NUMBER,
                /* 053   5                            */ &&L_NUMBER,
                /* 054   6                            */ &&L_NUMBER,
                /* 055   7                            */ &&L_NUMBER,
                /* 056   8                            */ &&L_NUMBER,
                /* 057   9                            */ &&L_NUMBER,
                /* 058   : (colon)                    */ &&L_CHAR,
                /* 059   ; (semi-colon)               */ &&L_CHAR,
                /* 060   < (less than)                */ &&L_CHAR,
                /* 061   = (equal sign)               */ &&L_CHAR,
                /* 062   > (greater than)             */ &&L_CHAR,
                /* 063   ? (question mark)            */ &&L_CHAR,
                /* 064   @ (AT symbol)                */ &&L_CHAR,
                /* 065   A                            */ &&L_CHAR,
                /* 066   B                            */ &&L_CHAR,
                /* 067   C                            */ &&L_CHAR,
                /* 068   D                            */ &&L_CHAR,
                /* 069   E                            */ &&L_CHAR,
                /* 070   F                            */ &&L_CHAR,
                /* 071   G                            */ &&L_CHAR,
                /* 072   H                            */ &&L_CHAR,
                /* 073   I                            */ &&L_CHAR,
                /* 074   J                            */ &&L_CHAR,
                /* 075   K                            */ &&L_CHAR,
                /* 076   L                            */ &&L_CHAR,
                /* 077   M                            */ &&L_CHAR,
                /* 078   N                            */ &&L_CHAR,
                /* 079   O                            */ &&L_CHAR,
                /* 080   P                            */ &&L_CHAR,
                /* 081   Q                            */ &&L_CHAR,
                /* 082   R                            */ &&L_CHAR,
                /* 083   S                            */ &&L_CHAR,
                /* 084   T                            */ &&L_CHAR,
                /* 085   U                            */ &&L_CHAR,
                /* 086   V                            */ &&L_CHAR,
                /* 087   W                            */ &&L_CHAR,
                /* 088   X                            */ &&L_CHAR,
                /* 089   Y                            */ &&L_CHAR,
                /* 090   Z                            */ &&L_CHAR,
                /* 091   [ (left/opening bracket)     */ &&L_BRACKETOPEN,
                /* 092   \ (back slash)               */ &&L_CHAR,
                /* 093   ] (right/closing bracket)    */ &&L_BRACKETCLOSE,
                /* 094   ^ (caret/circumflex)         */ &&L_CHAR,
                /* 095   _ (underscore)               */ &&L_CHAR,
                /* 096   `                            */ &&L_CHAR,
                /* 097   a                            */ &&L_CHAR,
                /* 098   b                            */ &&L_CHAR,
                /* 099   c                            */ &&L_CHAR,
                /* 100   d                            */ &&L_CHAR,
                /* 101   e                            */ &&L_CHAR,
                /* 102   f                            */ &&L_CHAR,
                /* 103   g                            */ &&L_CHAR,
                /* 104   h                            */ &&L_CHAR,
                /* 105   i                            */ &&L_CHAR,
                /* 106   j                            */ &&L_CHAR,
                /* 107   k                            */ &&L_CHAR,
                /* 108   l                            */ &&L_CHAR,
                /* 109   m                            */ &&L_CHAR,
                /* 110   n                            */ &&L_CHAR,
                /* 111   o                            */ &&L_CHAR,
                /* 112   p                            */ &&L_CHAR,
                /* 113   q                            */ &&L_CHAR,
                /* 114   r                            */ &&L_CHAR,
                /* 115   s                            */ &&L_CHAR,
                /* 116   t                            */ &&L_CHAR,
                /* 117   u                            */ &&L_CHAR,
                /* 118   v                            */ &&L_CHAR,
                /* 119   w                            */ &&L_CHAR,
                /* 120   x                            */ &&L_CHAR,
                /* 121   y                            */ &&L_CHAR,
                /* 122   z                            */ &&L_CHAR,
                /* 123   { (left/opening brace)       */ &&L_BRACEOPEN,
                /* 124   | (vertical bar)             */ &&L_VERTBAR,
                /* 125   } (right/closing brace)      */ &&L_BRACECLOSE,
                /* 126   ~ (tilde)                    */ &&L_CHAR,
                /* 127 DEL (delete)                   */ &&L_INVALID,
                /* 128 -------- reserved ------------ */ &&L_INVALID
        };

        int    errnum = 0;
        size_t toklen = 0, i = 0;
        char   c;
        char  *pbuf;

        SEXP_pflags_t exflags, exflags_tmp = 0;
        
        /* Variables for L_NUMBER */
        uint8_t num_type = 0;
        uint8_t    valid = 0; /* This variable is probably useless... */
        uint32_t       d = 0;
                
        SEXP_t *s_exp = NULL;
        struct SEXP_pext_dsc e_dsc;
        int ext_e;
        
        /* Check pstate */
#warning "Inefficient handling of pstate"
        
        if ((*statep) != NULL) {
                pbuf = (*statep)->buffer;
                pbuf = sm_realloc (pbuf, sizeof (char) * (buflen + (*statep)->buffer_data_len));
                
                memcpy (pbuf + (*statep)->buffer_data_len, buf, sizeof (char) * buflen);
                
                buflen += (*statep)->buffer_data_len;
                (*statep)->buffer_data_len = 0;
                (*statep)->buffer          = NULL;
                
        } else {
                (*statep) = SEXP_pstate_new ();
                pbuf      = (char *) buf;
        }
        
        /* Initialize parse flags */
        exflags = (*statep)->l_real > 1 ? (*statep)->pflags : setup->pflags;
        
        /* Main parser loop */
        for (;;) {
                _A(((*statep)->l_real  > 1 && !(exflags & EXF_EOFOK)) ||
                   ((*statep)->l_real == 1 &&  (exflags & EXF_EOFOK)));
                
                s_exp = SEXP_new ();
                
        L_NO_SEXP_ALLOC:
                s_exp->s_flgs = 0;
                
                if (i >= buflen)
                        break;
                
                c = pbuf[i];
                
                _D("LOOP: i=%zu, c=%c, sexp=%p, buflen=%zu\n",
                   i, c, s_exp, buflen);
                
                goto *((c >= 0) ? labels[(uint8_t)c] : labels[128]);
        L_CHAR:
                e_dsc.s_exp = s_exp;
                e_dsc.t_beg = pbuf + i;
                e_dsc.t_len = 0;
                e_dsc.b_len = buflen - i;
                e_dsc.flags = exflags;
                
                ext_e = EXTRACTOR(si_string)(&e_dsc);

                if (ext_e == SEXP_EXT_SUCCESS)
                        goto L_SEXP_ADD;
                
                break;
        L_CHAR_FIXEDLEN:
                e_dsc.s_exp = s_exp;
                e_dsc.t_beg = pbuf + i;
                e_dsc.t_len = toklen;
                e_dsc.b_len = buflen - i;
                e_dsc.flags = exflags;
                
                ext_e = EXTRACTOR_F(string)(&e_dsc);
                
                if (ext_e == SEXP_EXT_SUCCESS)
                        goto L_SEXP_ADD;
                
                break;
        L_DQUOTE:
                e_dsc.s_exp = s_exp;
                e_dsc.t_beg = pbuf + i;
                e_dsc.t_len = 0;
                e_dsc.b_len = buflen - i;
                e_dsc.flags = exflags;
                
                ext_e = EXTRACTOR(dq_string)(&e_dsc);
                
                if (ext_e == SEXP_EXT_SUCCESS)
                        goto L_SEXP_ADD;
                
                break;
        L_SQUOTE:
                e_dsc.s_exp = s_exp;
                e_dsc.t_beg = pbuf + i;
                e_dsc.t_len = 0;
                e_dsc.b_len = buflen - i;
                e_dsc.flags = exflags;
                
                ext_e = EXTRACTOR(sq_string)(&e_dsc);
                                
                if (ext_e == SEXP_EXT_SUCCESS)
                        goto L_SEXP_ADD;
                
                break;
        L_DOT:
                if (!isdigit (pbuf[i+1]))
                        goto L_CHAR;
                else
                        goto finalize_float;
        L_PLUS:
        L_MINUS:
                if (i + 1 < buflen) {
                        if (!isdigit (pbuf[i+1]) && pbuf[i+1] != '.') {
                                goto L_CHAR;
                        }
                } else {
                        if (exflags & EXF_EOFOK)
                                goto L_CHAR;
                        else
                                break;
                }
                
                d = 1;
                num_type = valid = 0;
                goto L_NUMBER_stage1;
        L_NUMBER:
                d = num_type = valid = 0;
        L_NUMBER_stage1:
                
                _A(num_type == 0);
                _A(valid == 0);
                _A(d < 2);
                
                switch (pbuf[i+d]) {
                case '.':
                        valid = 0;
                finalize_float:
                        num_type = NUMCLASS_FLT;
                        ++d;
                        
                        /* only digits are allowed now */
                        while (i + d < buflen) {
                                if (!isdigit (pbuf[i+d])) {
                                        if (isnexttok (pbuf[i+d]))
                                                goto finalize_all;
                                        else {
                                                switch (pbuf[i+d]) {
                                                case 'e':
                                                case 'E':
                                                        goto finalize_exponent;
                                                default:
                                                        SEXP_flag_set (s_exp, SEXP_FLAG_UNFIN);
                                                        goto invalid_number;
                                                }
                                        }
                                        /* NOTREACHED */
                                        abort ();
                                }
                                ++d;
                        }
                        
                        if (exflags & EXF_EOFOK) {
                                goto finalize_all;
                        } else {
                                SEXP_flag_set (s_exp, SEXP_FLAG_UNFIN);
                                goto invalid_number;
                        }
                        /* NOTREACHED */
                        abort ();
                        break;
                default:
                        if (isdigit(pbuf[i+d])) {
                                num_type = NUMCLASS_INT;
                                ++d;
                                
                                while (i + d < buflen) {
                                        if (!isdigit (pbuf[i+d])) {
                                                goto L_NUMBER_stage2;
                                        }
                                        ++d;
                                }
                                
                                if (exflags & EXF_EOFOK)
                                        goto L_NUMBER_stage3;
                        }
                        
                        SEXP_flag_set (s_exp, SEXP_FLAG_UNFIN);
                        goto invalid_number;
                }
                
        L_NUMBER_stage2:
                switch (pbuf[i+d]) {
                case '.':
                        valid = 1;
                        goto finalize_float;
                case 'e':
                case 'E':
                        valid = 0;
                finalize_exponent:
                        num_type = NUMCLASS_EXP;
                        ++d;
                        
                        if (i + d < buflen) {
                                switch (pbuf[i+d]) {
                                case '+':
                                case '-':
                                        ++d;
                                        break;
                                }
                                
                                while (i + d < buflen) {
                                        if (!isdigit (pbuf[i+d])) {
                                                if (isdigit (pbuf[i+d-1]))
                                                        goto L_NUMBER_stage3;
                                                else
                                                        goto invalid_number;
                                        }
                                        ++d;
                                }

                                if (isdigit (pbuf[i+d-1]) && (exflags & EXF_EOFOK))
                                        goto L_NUMBER_stage3;
                        }
                        
                        _D("Invalid number\n");
                        goto invalid_number;
                }
                
        L_NUMBER_stage3:
                /* check if the number is a length prefix */
                if (num_type == NUMCLASS_INT && pbuf[i] != '-') {
                        switch (pbuf[i+d]) {
                        case ':': /* string length */
                                toklen = strtoll (pbuf + i, NULL, 10);
                                i += d + 1; /* We want to skip the colon here */
                                goto L_CHAR_FIXEDLEN;
                        case '|': /* base64 string length */
                                toklen = strtoll (pbuf + i, NULL, 10);
                                i += d;
                                goto L_VERTBAR_FIXEDLEN;
                        case '[': /* datatype */
                                toklen = strtoll (pbuf + i, NULL, 10);
                                i += d;
                                goto L_BRACKETOPEN_FIXEDLEN;
                        case '#': /* hex string length */
                                toklen = strtoll (pbuf + i, NULL, 10);
                                i += d;
                                goto L_HASH_FIXEDLEN;
                        case '{': /* base64 string length */
                                toklen = strtoll (pbuf + i, NULL, 10);
                                i += d;
                                goto L_BRACEOPEN_FIXEDLEN;
                        }
                }
                
                /*
                 * Here should be the end of the number
                 */
                if (i + d < buflen) {
                        if (!isnexttok (pbuf[i+d]))
                                goto invalid_number;
                }
                
        finalize_all:
                if (d > 0) {
                        /* 
                         * Ok, it's a number, let's convert it from string
                         * to a real number taking into account the datatype.
                         */
                        
                        /* STR -> SEXP */
                        switch (num_type) {
                        case NUMCLASS_INT:
                                switch (*(pbuf + i)) {
                                case '-': /* signed */
                                {
                                        SEXP_val_t v_dsc;
                                        int64_t    number;
                                        
                                        number = strto_int64_dec (pbuf + i, d, NULL);
                                        
                                        switch (errno) {
                                        case ERANGE:
                                                _D("Number \"%.*s\" out of range.\n", d, pbuf + i);
                                                goto invalid_number;
                                        case EINVAL:
                                                _D("Invalid number: \"%.*s\"\n", d, pbuf + i);
                                                abort ();
                                        }
                                        
                                        if (number < INT16_MIN) {
                                                if (number < INT32_MIN) {
                                                        /* 64 */
                                                        if (SEXP_val_new (&v_dsc, sizeof (struct SEXP_val_num_i64),
                                                                          SEXP_VALTYPE_NUMBER) != 0)
                                                        {
                                                                /* TODO: handle this */
                                                                abort ();
                                                        }
                                                        
                                                        SEXP_NCASTP(i64,v_dsc.mem)->n = (int64_t)number;
                                                        SEXP_NCASTP(i64,v_dsc.mem)->t = SEXP_NUM_INT64;
                                                } else {
                                                        /* 32 */
                                                        if (SEXP_val_new (&v_dsc, sizeof (struct SEXP_val_num_i32),
                                                                          SEXP_VALTYPE_NUMBER) != 0)
                                                        {
                                                                /* TODO: handle this */
                                                                abort ();
                                                        }
                                                        
                                                        SEXP_NCASTP(i32,v_dsc.mem)->n = (int32_t)number;
                                                        SEXP_NCASTP(i32,v_dsc.mem)->t = SEXP_NUM_INT32;
                                                }
                                        } else {
                                                if (number < INT8_MIN) {
                                                        /* 16 */
                                                        if (SEXP_val_new (&v_dsc, sizeof (struct SEXP_val_num_i16),
                                                                          SEXP_VALTYPE_NUMBER) != 0)
                                                        {
                                                                /* TODO: handle this */
                                                                abort ();
                                                        }
                                                        
                                                        SEXP_NCASTP(i16,v_dsc.mem)->n = (int16_t)number;
                                                        SEXP_NCASTP(i16,v_dsc.mem)->t = SEXP_NUM_INT16;
                                                } else {
                                                        /* 8 */
                                                        if (SEXP_val_new (&v_dsc, sizeof (struct SEXP_val_num_i8),
                                                                          SEXP_VALTYPE_NUMBER) != 0)
                                                        {
                                                                /* TODO: handle this */
                                                                abort ();
                                                        }
                                                        
                                                        SEXP_NCASTP(i8,v_dsc.mem)->n = (int8_t)number;
                                                        SEXP_NCASTP(i8,v_dsc.mem)->t = SEXP_NUM_INT8;
                                                }
                                        }
                                        
                                        s_exp->s_valp = SEXP_val_ptr (&v_dsc);
                                } break;
                                default: /* unsigned */
                                {
                                        SEXP_val_t v_dsc;
                                        uint64_t   number;
                                        
                                        number = strto_uint64_dec (pbuf + i, d, NULL);
                                        
                                        switch (errno) {
                                        case ERANGE:
                                                _D("Number \"%.*s\" out of range.\n", d, pbuf + i);
                                                goto invalid_number;
                                        case EINVAL:
                                                _D("Invalid number: \"%.*s\"\n", d, pbuf + i);
                                                abort ();
                                        }
                                        
                                        if (number > UINT16_MAX) {
                                                if (number > UINT32_MAX) {
                                                        /* 64 */
                                                        if (SEXP_val_new (&v_dsc, sizeof (struct SEXP_val_num_u64),
                                                                          SEXP_VALTYPE_NUMBER) != 0)
                                                        {
                                                                /* TODO: handle this */
                                                                abort ();
                                                        }
                                                        
                                                        SEXP_NCASTP(u64,v_dsc.mem)->n = (uint64_t)number;
                                                        SEXP_NCASTP(u64,v_dsc.mem)->t = SEXP_NUM_UINT64;
                                                } else {
                                                        /* 32 */
                                                        if (SEXP_val_new (&v_dsc, sizeof (struct SEXP_val_num_u32),
                                                                          SEXP_VALTYPE_NUMBER) != 0)
                                                        {
                                                                /* TODO: handle this */
                                                                abort ();
                                                        }
                                                        
                                                        SEXP_NCASTP(u32,v_dsc.mem)->n = (uint32_t)number;
                                                        SEXP_NCASTP(u32,v_dsc.mem)->t = SEXP_NUM_UINT32;
                                                }
                                        } else {
                                                if (number > UINT8_MAX) {
                                                        /* 16 */
                                                        if (SEXP_val_new (&v_dsc, sizeof (struct SEXP_val_num_u16),
                                                                          SEXP_VALTYPE_NUMBER) != 0)
                                                        {
                                                                /* TODO: handle this */
                                                                abort ();
                                                        }
                                                        
                                                        SEXP_NCASTP(u16,v_dsc.mem)->n = (uint16_t)number;
                                                        SEXP_NCASTP(u16,v_dsc.mem)->t = SEXP_NUM_UINT16;
                                                } else {
                                                        /* 8 */
                                                        if (SEXP_val_new (&v_dsc, sizeof (struct SEXP_val_num_u8),
                                                                          SEXP_VALTYPE_NUMBER) != 0)
                                                        {
                                                                /* TODO: handle this */
                                                                abort ();
                                                        }
                                                        
                                                        SEXP_NCASTP(u8,v_dsc.mem)->n = (uint8_t)number;
                                                        SEXP_NCASTP(u8,v_dsc.mem)->t = SEXP_NUM_UINT8;
                                                }
                                        }
                                        
                                        s_exp->s_valp = SEXP_val_ptr (&v_dsc);
                                }}
                                break;
                        case NUMCLASS_FLT:
                        case NUMCLASS_EXP: /* TODO: store double/long double */
                        {
                                SEXP_val_t v_dsc;
                                double     number;
                                
                                number = strto_double (pbuf + i, d, NULL);
                                
                                switch (errno) {
                                case ERANGE:
                                        _D("Number \"%.*s\" out of range.\n", d, pbuf + i);
                                        goto invalid_number;
                                case EINVAL:
                                        _D("Invalid number: \"%.*s\"\n", d, pbuf + i);
                                        abort ();
                                }
                                
                                if (SEXP_val_new (&v_dsc, sizeof (struct SEXP_val_num_f),
                                                  SEXP_VALTYPE_NUMBER) != 0)
                                {
                                        /* TODO: handle this */
                                        abort ();
                                }
                                
                                SEXP_NCASTP(f,v_dsc.mem)->n = (double)number;
                                SEXP_NCASTP(f,v_dsc.mem)->t = SEXP_NUM_UINT8;
                                s_exp->s_valp = SEXP_val_ptr (&v_dsc);
                        }
                        break;
                        default:
                                _D("Unknown number type\n");
                                abort ();
                        }
                        
                        /* STR -> SEXP END */                        
                        //i += d;
                        e_dsc.t_len = d;
                        
                        num_type = 0;
                        valid = 0;
                        d = 0;
                } else {
                invalid_number:
                        ext_e = SEXP_EXT_EINVAL;
                        num_type = 0;
                        valid = 0;
                        d = 0;
                        /* Invalid number format */
                        break;
                }
                /* If we got here, it's definitely just a number */
                ext_e = SEXP_EXT_SUCCESS;
                goto L_SEXP_ADD;
        L_NUL:
                _D("WTF? NUL found.\n");
                errnum = EILSEQ;
                break;
        L_WHITESPACE:
                while (++i < buflen) {
                        if (!isspace(pbuf[i]))
                                break;
                }
                goto L_NO_SEXP_ALLOC;
        L_PAROPEN:
                {
                        SEXP_t *s_list, *s_ref;
                        
                        s_list = SEXP_list_new (NULL);
                        s_ref  = SEXP_softref (s_list);
                        
                        SEXP_list_add (SEXP_pstate_lstack_top (*statep), s_list);
                        SEXP_free (s_list);
                        
                        SEXP_pstate_lstack_push (*statep, s_ref);
                                                
                        if ((*statep)->l_real == 2) {
                                exflags_tmp = exflags;
                                exflags &= ~(EXF_EOFOK);
                        }
                }
                ++i;
                continue;
        L_PARCLOSE:
                ++i;
                
                if ((*statep)->l_real > 1) {
                        SEXP_t *s_list;
                        
                        s_list = SEXP_pstate_lstack_pop (*statep);
                        SEXP_free (s_list);
                        
                        if ((*statep)->l_real == 1) {
                                /*
                                 * We are outside a list, restore
                                 * original exflags
                                 */
                                exflags = exflags_tmp;
                        }
                        
                        ext_e = SEXP_EXT_SUCCESS;
                        goto L_NO_SEXP_ALLOC;
                } else {
                        errnum = EILSEQ;
                        ext_e  = SEXP_EXT_EINVAL;
                }
                
                break;
        L_BRACKETOPEN:
                e_dsc.s_exp = s_exp;
                e_dsc.t_beg = pbuf + i;
                e_dsc.t_len = 0;
                e_dsc.b_len = buflen - i;
                e_dsc.flags = exflags;
                
                ext_e = EXTRACTOR(sq_string)(&e_dsc);
                
                if (ext_e == SEXP_EXT_SUCCESS)
                        goto L_SEXP_ADD;
                
                break;
        L_BRACKETOPEN_FIXEDLEN:
                e_dsc.s_exp = s_exp;
                e_dsc.t_beg = pbuf + i;
                e_dsc.t_len = toklen;
                e_dsc.b_len = buflen - i;
                e_dsc.flags = exflags;
                
                ext_e = EXTRACTOR_F(string)(&e_dsc);
                
                if (ext_e == SEXP_EXT_SUCCESS)
                        goto L_SEXP_ADD;
                
                break;
        L_BRACEOPEN:
                //i += EXTRACTOR(b64_decode)(sexp, pbuf + i, buflen, exflags);
                errnum = EILSEQ;
                break;
        L_BRACEOPEN_FIXEDLEN:
                // EXTRACTOR_F(b64_decode)(sexp, pbuf + i, buflen, toklen, exflags);
                errnum = EILSEQ;
                break;
        L_VERTBAR:
                e_dsc.s_exp = s_exp;
                e_dsc.t_beg = pbuf + i;
                e_dsc.t_len = 0;
                e_dsc.b_len = buflen - i;
                e_dsc.flags = exflags;
                
                ext_e = EXTRACTOR(b64_string)(&e_dsc);

                if (ext_e == SEXP_EXT_SUCCESS)
                        goto L_SEXP_ADD;
                
                break;
        L_VERTBAR_FIXEDLEN:
                e_dsc.s_exp = s_exp;
                e_dsc.t_beg = pbuf + i;
                e_dsc.t_len = toklen;
                e_dsc.b_len = buflen - i;
                e_dsc.flags = exflags;
                
                ext_e = EXTRACTOR_F(b64_string)(&e_dsc);

                if (ext_e == SEXP_EXT_SUCCESS)
                        goto L_SEXP_ADD;
                
                break;
        L_HASH:
                e_dsc.s_exp = s_exp;
                e_dsc.t_beg = pbuf + i;
                e_dsc.t_len = 0;
                e_dsc.b_len = buflen - i;
                e_dsc.flags = exflags;
                
                ext_e = EXTRACTOR(hexstring)(&e_dsc);

                if (ext_e == SEXP_EXT_SUCCESS)
                        goto L_SEXP_ADD;
                
                break;
        L_HASH_FIXEDLEN:
                e_dsc.s_exp = s_exp;
                e_dsc.t_beg = pbuf + i;
                e_dsc.t_len = toklen;
                e_dsc.b_len = buflen - i;
                e_dsc.flags = exflags;
                
                ext_e = EXTRACTOR_F(hexstring)(&e_dsc);
                
                if (ext_e == SEXP_EXT_SUCCESS)
                        goto L_SEXP_ADD;
                
                break;
        L_BRACECLOSE:
        L_BRACKETCLOSE:
        L_INVALID:
                _D("Syntax error: Invalid character: %02x\n", c);
                ext_e  = SEXP_EXT_EINVAL;
                errnum = EILSEQ;
                break;
        L_SEXP_ADD:
                /* Add new expression to list */
                SEXP_list_add (SEXP_pstate_lstack_top (*statep), s_exp);
                SEXP_free (s_exp);
#ifndef NDEBUG
                s_exp = NULL;
#endif
                i += e_dsc.t_len;
                
                continue;
        }
        
        switch (ext_e) {
        case SEXP_EXT_SUCCESS:
                if ((*statep)->l_real == 1) {
                        
                        s_exp = (*statep)->p_list;
                        (*statep)->p_list = NULL;
                        
                        SEXP_pstate_free (*statep);
                        (*statep) = NULL;
                        errno     = 0;
                        
                        return (s_exp);
                }
        case SEXP_EXT_EUNFIN:
                (*statep)->buffer_data_len = buflen - i;
                (*statep)->buffer = xmemdup (pbuf + i, (*statep)->buffer_data_len);
                (*statep)->pflags = exflags;
                
                break;
        case SEXP_EXT_EINVAL:
                break;
        default:
                abort ();
        }
        
        SEXP_free (s_exp);
        errno = errnum;

        return (NULL);
}

DEFEXTRACTOR(si_string)
{
        SEXP_val_t v_dsc;
        register size_t l = 1;
        
        _A(dsc != NULL);
        _D("Parsing simple string\n");
        
        while (!isnexttok (dsc->t_beg[l])) {
                if (l < dsc->b_len)
                        ++l;
                else {
                        if (dsc->flags & EXF_EOFOK)
                                break;
                        else
                                return (SEXP_EXT_EUNFIN);
                }
        }

#if !defined(NDEBUG)
        if (dsc->flags & EXF_EOFOK)
                _D("EOF is ok -> si string complete\n");
        else
                _D("next tok char: \"%c\"\n", dsc->t_beg[l]);
#endif
                
        if (SEXP_val_new (&v_dsc, sizeof (char) * l,
                          SEXP_VALTYPE_STRING) != 0)
        {
                /* TODO: handle this */
                return (SEXP_EXT_EINVAL);
        }
        
        memcpy (v_dsc.mem, dsc->t_beg, sizeof (char) * l);
        dsc->s_exp->s_valp = SEXP_val_ptr (&v_dsc);
        dsc->t_len = l;
        
        return (SEXP_EXT_SUCCESS);
}

DEFEXTRACTOR(dq_string)
{
        SEXP_val_t v_dsc;
        register size_t l = 1;

        _A(dsc != NULL);
        _D("Parsing double-quoted string\n");
        
        while (dsc->t_beg[l] != '"') {
                if (l < dsc->b_len) {
                        if (dsc->t_beg[l] == '\\') {
                                if (l + 1 < dsc->b_len)
                                        ++l;
                                else
                                        return (SEXP_EXT_EUNFIN);
                        }
                } else
                        return (SEXP_EXT_EUNFIN);

                ++l;
        }
        
        dsc->t_len = l + 1;
        
        if (SEXP_val_new (&v_dsc, sizeof (char) * (l - 1),
                          SEXP_VALTYPE_STRING) != 0)
        {
                /* TODO: handle this */
                return (SEXP_EXT_EINVAL);
        }
        
        memcpy (v_dsc.mem, dsc->t_beg + 1, sizeof (char) * (l - 1));
        dsc->s_exp->s_valp = SEXP_val_ptr (&v_dsc);
        
        return (SEXP_EXT_SUCCESS);
}

DEFEXTRACTOR(sq_string)
{
        SEXP_val_t v_dsc;
        register size_t l = 1;
        
        _A(dsc != NULL);
        _D("Parsing single-quoted string\n");
        
        while (dsc->t_beg[l] != '\'') {
                if (l >= dsc->b_len)
                        return (SEXP_EXT_EUNFIN);
                
                ++l;
        }
        
        dsc->t_len = l + 1;
        
        if (SEXP_val_new (&v_dsc, sizeof (char) * (l - 1),
                          SEXP_VALTYPE_STRING) != 0)
        {
                /* TODO: handle this */
                return (SEXP_EXT_EINVAL);
        }
        
        memcpy (v_dsc.mem, dsc->t_beg + 1, sizeof (char) * (l - 1));
        dsc->s_exp->s_valp = SEXP_val_ptr (&v_dsc);
        
        return (SEXP_EXT_SUCCESS);
}

DEFEXTRACTOR_F(string)
{
        SEXP_val_t v_dsc;
        
        _D("Parsing fixed length string\n");

        if (dsc->t_len > dsc->b_len)
                return (SEXP_EXT_EUNFIN);
                
        if (SEXP_val_new (&v_dsc, sizeof (char) * dsc->t_len,
                          SEXP_VALTYPE_STRING) != 0)
        {
                /* TODO: handle this */
                return (SEXP_EXT_EINVAL);
        }
        
        memcpy (v_dsc.mem, dsc->t_beg, sizeof (char) * dsc->t_len);
        dsc->s_exp->s_valp = SEXP_val_ptr (&v_dsc);
        
        return (SEXP_EXT_SUCCESS);
}

DEFEXTRACTOR(b64_string)
{
        SEXP_val_t v_dsc;
        register uint32_t l = 1, slen;
        char *string;
        
        _A(dsc != NULL);
        _D("Extracting b64 string\n");
        
#warning "Strict character checking not implemented"
        
        while (dsc->t_beg[l] != '|') {
                if (isnexttok (dsc->t_beg[l]))
                        return (SEXP_EXT_EINVAL);
                if (l >= dsc->b_len)
                        return (SEXP_EXT_EUNFIN);
                ++l;
        }
        
        dsc->t_len = ++l;
        slen       = base64_decode (dsc->t_beg + 1, (size_t)(l - 2), (uint8_t **)&string);
        
        if (slen == 0) {
                _D("base64_decode failed\n");
                return (SEXP_EXT_EINVAL);
        }
        
        _D("string = \"%.*s\"\n", slen, string);
        
        if (SEXP_val_new (&v_dsc, sizeof (char) * slen,
                          SEXP_VALTYPE_STRING) != 0)
        {
                /* TODO: handle this */
                return (SEXP_EXT_EINVAL);
        }
        
        memcpy  (v_dsc.mem, string, sizeof (char) * slen);
        sm_free (string);
        
        dsc->s_exp->s_valp = SEXP_val_ptr (&v_dsc);
        
        return (SEXP_EXT_SUCCESS);
}

DEFEXTRACTOR_F(b64_string)
{
        SEXP_val_t v_dsc;
        char    *string;
        uint32_t slen;
        
        _A(dsc != NULL);
        _D("Parsing fixed length b64 string, toklen=%u, len=%u\n", dsc->t_len, dsc->b_len);
        
        if (dsc->t_len > dsc->b_len - 2)
                return (SEXP_EXT_EUNFIN);

        slen = base64_decode (dsc->t_beg + 1, dsc->t_len, (uint8_t **)&string);
        
        if (slen == 0)
                return (SEXP_EXT_EINVAL);
        
        dsc->t_len += 2;
        
        if (SEXP_val_new (&v_dsc, sizeof (char) * slen,
                          SEXP_VALTYPE_STRING) != 0)
        {
                /* TODO: handle this */
                return (SEXP_EXT_EINVAL);
        }
        
        memcpy  (v_dsc.mem, string, sizeof (char) * slen);
        sm_free (string);
        
        dsc->s_exp->s_valp = SEXP_val_ptr (&v_dsc);
        
        return (SEXP_EXT_SUCCESS);
}

DEFEXTRACTOR(datatype)
{
        char *name, name_static[128];
        register uint32_t l = 1;
        
        _A(dsc != NULL);
        _A(dsc->s_exp->s_type == NULL);
        _D("Parsing datatype\n");
        
        while (dsc->t_beg[l] != ']') {
                if (l < dsc->b_len) {
                        if (dsc->t_beg[l] == '\\') {
                                if (l + 1 < dsc->b_len)
                                        ++l;
                                else
                                        return (SEXP_EXT_EUNFIN);
                        }
                } else
                        return (SEXP_EXT_EUNFIN);

                ++l;
        }
                
        dsc->t_len = l + 1;

        if ((l - 1) < sizeof name_static) {
                memcpy (name_static,
                        dsc->t_beg + 1, sizeof (char) * (l - 1));
                name_static[l - 1] = '\0';
                name = name_static;
        } else {
                name = sm_alloc (sizeof (char) * l);
                memcpy (name,
                        dsc->t_beg + 1, sizeof (char) * (l - 1));
                name[l - 1] = '\0';
        }
        
        dsc->s_exp->s_type = SEXP_datatype_get (&g_datatypes, name);
        
        if (dsc->s_exp->s_type == NULL) {
                SEXP_datatype_t datatype;
                
                datatype.name     = (name != name_static ? name : strdup (name));
                datatype.name_len = l - 1;        
                datatype.op       = NULL;
                datatype.op_cnt   = 0;
                
                dsc->s_exp->s_type = SEXP_datatype_add (&g_datatypes, &datatype);
        } else {
                if (name != name_static)
                        sm_free (name);
        }
        
        return (SEXP_EXT_SUCCESS);
}

DEFEXTRACTOR_F(datatype)
{
        char *name, name_static[128];
        
        _A(dsc != NULL);
        _A(dsc->s_exp->s_type == NULL);
        _D("Parsing fixed length datatype\n");
        
        if (dsc->t_len > dsc->b_len - 2)
                return (SEXP_EXT_EUNFIN);
        
        if (dsc->t_len < sizeof name_static) {
                memcpy (name_static,
                        dsc->t_beg + 1, sizeof (char) * dsc->t_len);
                name_static[dsc->t_len] = '\0';
                name = name_static;
        } else {
                name = sm_alloc (sizeof (char) * (dsc->t_len + 1));
                memcpy (name,
                        dsc->t_beg + 1, sizeof (char) * dsc->t_len);
                name[dsc->t_len] = '\0';
        }

        dsc->s_exp->s_type = SEXP_datatype_get (&g_datatypes, name);
        
        if (dsc->s_exp->s_type == NULL) {
                SEXP_datatype_t datatype;
                
                datatype.name     = (name != name_static ? name : strdup (name));
                datatype.name_len = dsc->t_len;
                datatype.op       = NULL;
                datatype.op_cnt   = 0;
                
                dsc->s_exp->s_type = SEXP_datatype_add (&g_datatypes, &datatype);
        } else {
                if (name != name_static)
                        sm_free (name);
        }
        
        dsc->t_len += 2;

        return (SEXP_EXT_SUCCESS);
}

const char hex2bin[] = {
        /* 0 */ 0x00,
        /* 1 */ 0x01,
        /* 2 */ 0x02,
        /* 3 */ 0x03,
        /* 4 */ 0x04,
        /* 5 */ 0x05,
        /* 6 */ 0x06,
        /* 7 */ 0x07,
        /* 8 */ 0x08,
        /* 9 */ 0x09,
        /* : */ 0,
        /* ; */ 0,
        /* < */ 0,
        /* = */ 0,
        /* > */ 0,
        /* ? */ 0,
        /* @ */ 0,
        /* A */ 0x0a,
        /* B */ 0x0b,
        /* C */ 0x0c,
        /* D */ 0x0d,
        /* E */ 0x0e,
        /* F */ 0x0f,
        /* G */ 0,
        /* H */ 0,
        /* I */ 0,
        /* J */ 0,
        /* K */ 0,
        /* L */ 0,
        /* M */ 0,
        /* N */ 0,
        /* O */ 0,
        /* P */ 0,
        /* Q */ 0,
        /* R */ 0,
        /* S */ 0,
        /* T */ 0,
        /* U */ 0,
        /* V */ 0,
        /* W */ 0,
        /* X */ 0,
        /* Y */ 0,
        /* Z */ 0,
        /* [ */ 0,
        /* \ */ 0,
        /* ] */ 0,
        /* ^ */ 0,
        /* _ */ 0,
        /* ` */ 0,
        /* a */ 0x0a,
        /* b */ 0x0b,
        /* c */ 0x0c,
        /* d */ 0x0d,
        /* e */ 0x0e,
        /* f */ 0x0f
};
#define B(n) (((n) - 48) % sizeof hex2bin)

DEFEXTRACTOR(hexstring)
{
        SEXP_val_t v_dsc;
        register uint32_t l = 1, i;

        _D("Extracting hex string\n");
        
        while (dsc->t_beg[l] != '#') {
                if (l < dsc->b_len) {
                        switch (dsc->t_beg[l]) {
                        case 'a':
                        case 'A':
                        case 'b':
                        case 'B':
                        case 'c':
                        case 'C':
                        case 'd':
                        case 'D':
                        case 'e':
                        case 'E':
                        case 'f':
                        case 'F':
                                ++l;
                                break;
                        default:
                                if (isdigit (dsc->t_beg[l]))
                                        ++l;
                                else
                                        return (SEXP_EXT_EINVAL);
                        }
                } else
                        return (SEXP_EXT_EUNFIN);
        }
        
        if (l < 2)
                return (SEXP_EXT_EINVAL);
        
        if (SEXP_val_new (&v_dsc, sizeof (char) * (((l - 1) >> 1) + ((l - 1) & 1)),
                          SEXP_VALTYPE_STRING) != 0)
        {
                /* TODO: handle this */
                return (SEXP_EXT_EINVAL);
        }
        
        for (i = 0; i < ((l - 1) >> 1); ++i)
                (((char *)v_dsc.mem)[i]) = (hex2bin[B(dsc->t_beg[2*i + 1])] << 4) | hex2bin[B(dsc->t_beg[2*i + 2])];
        
        if ((l - 1) & 1)
                (((char *)v_dsc.mem)[i]) = hex2bin[B(2*i + 1)] << 4;

        dsc->t_len = l + 1;
        
        return (SEXP_EXT_SUCCESS);
}

DEFEXTRACTOR_F(hexstring)
{
        SEXP_val_t v_dsc;
        register uint32_t i;
        
        _D("Parsing fixed length hex string\n");

        if (dsc->t_len == 0)
                return (SEXP_EXT_EINVAL);
        if (dsc->t_len > dsc->b_len - 2)
                return (SEXP_EXT_EUNFIN);
        
        if (SEXP_val_new (&v_dsc, sizeof (char) * ((dsc->t_len >> 2) + (dsc->t_len & 1)),
                          SEXP_VALTYPE_STRING) != 0)
        {
                /* TODO: handle this */
                return (SEXP_EXT_EINVAL);
        }
        
        for (i = 0; i < (dsc->t_len >> 1); ++i)
                (((char *)v_dsc.mem)[i]) = (hex2bin[B(dsc->t_beg[2*i + 1])] << 4) | hex2bin[B(dsc->t_beg[2*i + 2])];
        
        if (dsc->t_len & 1)
                (((char *)v_dsc.mem)[i]) = hex2bin[B(2*i + 1)] << 4;
        
        dsc->t_len += 2;
        
        return (SEXP_EXT_SUCCESS);
}
#undef B

#undef EXTRACTOR
#undef EXTRACTOR_F
#undef DEFEXTRACTOR
#undef DEFEXTRACTOR_F

/*
 * End parser: label
 */

#if 0
SEXP_t *SEXP_parse (SEXP_psetup_t *psetup, const char *buf, size_t buflen, SEXP_pstate_t **pstate)
{
        _A(buf != NULL);
        _A(buflen > 0);
        _A(pstate != NULL);

        /*
        if (ctx->parser != NULL) {
                return ctx->parser (fmt, buf, buflen, pstate);
        } else {
                return (NULL);
        }
        */
        return PARSER(label)(psetup, buf, buflen, pstate);
}
#endif
