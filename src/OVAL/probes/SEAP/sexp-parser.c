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


#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "common/assume.h"
#include "generic/common.h"
#include "public/sm_alloc.h"
#include "_sexp-types.h"
#include "_sexp-manip.h"
#include "_sexp-parser.h"
#include "_sexp-datatype.h"
#include "_sexp-value.h"
#include "_sexp-rawptr.h"
#include "generic/xbase64.h"
#include "generic/strto.h"
#include "public/strbuf.h"

SEXP_pstate_t *SEXP_pstate_new (void)
{
        SEXP_pstate_t *pstate;

        pstate = sm_talloc (SEXP_pstate_t);
        pstate->p_buffer = NULL;
        pstate->p_bufoff = 0;
        pstate->p_explen = 0;
        pstate->p_flags  = 0;
        pstate->p_flags0 = 0;
        pstate->p_sexp   = NULL;
        pstate->sp_data  = NULL;
        pstate->sp_free  = NULL;

        for(int i = 0; i < SEXP_PFUNC_COUNT; ++i) {
                pstate->sp_shptr[i]  = NULL;
                pstate->sp_shfree[i] = NULL;
        }

        pstate->p_label    = 128;
        pstate->p_numclass = SEXP_NUMCLASS_INV;
        pstate->p_numbase  = 0;
        pstate->p_numstage = 255;

        pstate->v_bool[0] = 0;
        pstate->v_bool[1] = 0;
	pstate->p_error   = SEXP_PRET_EUNDEF;

        return (pstate);
}

void SEXP_pstate_free (SEXP_pstate_t *pstate)
{
        register uint32_t i;

        assume_d (pstate != NULL, /* void */);

        if (pstate->p_buffer != NULL) {
                if (pstate->p_flags & SEXP_PFLAG_FREEBUF)
                        spb_free (pstate->p_buffer, SPB_FLAG_FREE);
                else
                        spb_free (pstate->p_buffer, 0);
        }

        if (pstate->p_sexp != NULL)
                SEXP_free (pstate->p_sexp);

        SEXP_lstack_destroy (&pstate->l_stack);

        /*
         * Free the subparser data using the sp_free function
         * if set.
         */
        if (pstate->sp_data != NULL) {
                if (pstate->sp_free != NULL)
                        pstate->sp_free (pstate->sp_data);
        }

        /*
         * Free the subparser shared pointer using functions
         * from the sp_shfree array (if != NULL)
         */
        for(i = 0; i < SEXP_PFUNC_COUNT; ++i) {
                if (pstate->sp_shptr[i] != NULL && pstate->sp_shfree[i] != NULL)
                        pstate->sp_shfree[i](pstate->sp_shptr[i]);
        }

        /*
         * Free cached values
         */
        for (i = 0; i < 2; ++i) {
                if (pstate->v_bool[i] != 0) {
                        if (SEXP_rawval_decref (pstate->v_bool[i])) {
                                SEXP_val_t v_dsc;

                                SEXP_val_dsc (&v_dsc, pstate->v_bool[i]);
                                sm_free (v_dsc.hdr);
                        }
                }
        }

#ifndef NDEBUG
        memset (pstate, 0, sizeof (SEXP_pstate_t));
#endif
        sm_free (pstate);

        return;
}

int SEXP_psetup_setflags (SEXP_psetup_t *psetup, SEXP_pflags_t flags)
{
        assume_d (psetup != NULL, -1);
        assume_r (flags & SEXP_PFLAG_ALL, -1);

        psetup->p_flags |= flags;

        return (0);
}

int SEXP_psetup_unsetflags (SEXP_psetup_t *psetup, SEXP_pflags_t flags)
{
        assume_d (psetup != NULL, -1);
        assume_r (flags & SEXP_PFLAG_ALL, -1);

        psetup->p_flags &= ~flags;

        return (0);
}

SEXP_psetup_t *SEXP_psetup_new (void)
{
        SEXP_psetup_t *psetup;

        psetup = sm_talloc (SEXP_psetup_t);
        psetup->p_format = 0;
        psetup->p_flags  = SEXP_PFLAG_EOFOK;

        psetup->p_funcp[SEXP_PFUNC_UL_STRING_SI] =  &SEXP_parse_ul_string_si;
        psetup->p_funcp[SEXP_PFUNC_UL_STRING_DQ] =  &SEXP_parse_ul_string_dq;
        psetup->p_funcp[SEXP_PFUNC_UL_STRING_SQ] =  &SEXP_parse_ul_string_sq;
        psetup->p_funcp[SEXP_PFUNC_KL_STRING] =     &SEXP_parse_kl_string;
        psetup->p_funcp[SEXP_PFUNC_UL_STRING_B64] = &SEXP_parse_ul_string_b64;
        psetup->p_funcp[SEXP_PFUNC_KL_STRING_B64] = &SEXP_parse_kl_string_b64;
        psetup->p_funcp[SEXP_PFUNC_UL_DATATYPE] =   &SEXP_parse_ul_datatype;
        psetup->p_funcp[SEXP_PFUNC_KL_DATATYPE] =   &SEXP_parse_kl_datatype;
        /* psetup->p_funcp[SEXP_PFUNC_BOOL] =          &SEXP_parse_bool; */

        return (psetup);
}

int SEXP_psetup_setpfunc(SEXP_psetup_t *psetup, int pfunctype, SEXP_pfunc_t *pfunc)
{
        assume_r(psetup != NULL, -1, errno = EFAULT;);
        assume_r(pfunc  != NULL, -1, errno = EINVAL;);
        assume_r(pfunctype >= 0 && pfunctype < SEXP_PFUNC_COUNT, -1, errno = EINVAL;);

        psetup->p_funcp[pfunctype] = pfunc;

        return(0);
}

void SEXP_psetup_free (SEXP_psetup_t *psetup)
{
        sm_free (psetup);
        return;
}

static inline bool isnextexp (int c)
{
        register uint8_t r = (uint8_t)c;

        /*
         * characters that denote the beginning
         * or ending of an S-expression
         */
        if (r  < 40) {
                if (r < 20) {
                        switch (r) {
                        case '\n': /* 10 */
                        case '\t': /* 09 */
                        case '\r': /* 13 */
                        case '\a': /* 07 */
                                return (true);
                        }
                } else {
                        switch (r) {
                        case ' ' : /* 32 */
                        case '"' : /* 34 */
                        case '#' : /* 35 */
                        case '\'': /* 39 */
                                return (true);
                        }
                }
        } else {
                if (r < 100) {
                        switch (r) {
                        case '(' : /* 40 */
                        case ')' : /* 41 */
                        case '[' : /* 91 */
                        case ']' : /* 93 */
                                return (true);
                        }
                } else {
                        switch (r) {
                        case '|' : /* 124 */
                        case '{' : /* 123 */
                        case '}' : /* 125 */
                                return (true);
                        }
                }
        }
        return (false);
}

enum SEXP_parser_states {
        S_START,
        S_NO_SEXP_ALLOC,
        S_NO_CURC_UPDATE,
        S_CHAR,
        S_CHAR_FIXEDLEN,
        S_DQUOTE,
        S_SQUOTE,
        S_DOT,
        S_PLUS,
        S_MINUS,
        S_NUMBER,
        S_HASH,
        S_WHITESPACE,
        S_PAROPEN,
        S_PARCLOSE,
        S_BRACKETOPEN,
        S_BRACKETOPEN_FIXEDLEN,
        S_BRACEOPEN,
        S_BRACEOPEN_FIXEDLEN,
        S_VERTBAR,
        S_VERTBAR_FIXEDLEN,
        S_NUL,
        S_BRACECLOSE,
        S_BRACKETCLOSE,
        S_INVALID,
        S_SEXP_ADD,
};

/**
 * The S-expression parser
 * @param psetup parser settings (this argument is ignored is *pstate != NULL)
 * @param buffer buffer containg an S-expression in textual form
 * @param length length of the buffer
 * @param pstate pointer to SEXP_pstate_t pointer where the parser state will be allocated
 */
SEXP_t *SEXP_parse (const SEXP_psetup_t *psetup, char *buffer, size_t buflen, SEXP_pstate_t **pstate)
{
        const void *d_labels[] = {
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
                /* 128 -------- reserved ------------ */ &&L_INVALID,
                /* 129                                */ &&L_CHAR_FIXEDLEN,
                /* 130                                */ &&L_BRACKETOPEN_FIXEDLEN,
                /* 131                                */ &&L_VERTBAR_FIXEDLEN,
                /* 132                                */ &&L_BRACEOPEN_FIXEDLEN
        };

        const int d_states[] = {
                /* 000 NUL (Null char.)               */ S_NUL,
                /* 001 SOH (Start of Header)          */ S_INVALID,
                /* 002 STX (Start of Text)            */ S_INVALID,
                /* 003 ETX (End of Text)              */ S_INVALID,
                /* 004 EOT (End of Transmission)      */ S_INVALID,
                /* 005 ENQ (Enquiry)                  */ S_INVALID,
                /* 006 ACK (Acknowledgment)           */ S_INVALID,
                /* 007 BEL (Bell)                     */ S_INVALID,
                /* 008  BS (Backspace)                */ S_INVALID,
                /* 009  HT (Horizontal Tab)           */ S_WHITESPACE,
                /* 010  LF (Line Feed)                */ S_WHITESPACE,
                /* 011  VT (Vertical Tab)             */ S_WHITESPACE,
                /* 012  FF (Form Feed)                */ S_WHITESPACE,
                /* 013  CR (Carriage Return)          */ S_WHITESPACE,
                /* 014  SO (Shift Out)                */ S_INVALID,
                /* 015  SI (Shift In)                 */ S_INVALID,
                /* 016 DLE (Data Link Escape)         */ S_INVALID,
                /* 017 DC1 (Device Control 1 - XON)   */ S_INVALID,
                /* 018 DC2 (Device Control 2)         */ S_INVALID,
                /* 019 DC3 (Device Control 3 - XOFF)  */ S_INVALID,
                /* 020 DC4 (Device Control 4)         */ S_INVALID,
                /* 021 NAK (Negativ Acknowledgemnt)   */ S_INVALID,
                /* 022 SYN (Synchronous Idle)         */ S_INVALID,
                /* 023 ETB (End of Trans. Block)      */ S_INVALID,
                /* 024 CAN (Cancel)                   */ S_INVALID,
                /* 025  EM (End of Medium)            */ S_INVALID,
                /* 026 SUB (Substitute)               */ S_INVALID,
                /* 027 ESC (Escape)                   */ S_INVALID,
                /* 028  FS (File Separator)           */ S_INVALID,
                /* 029  GS (Group Separator)          */ S_INVALID,
                /* 030  RS (Reqst to Send)(Rec. Sep.) */ S_INVALID,
                /* 031  US (Unit Separator)           */ S_INVALID,
                /* 032  SP (Space)                    */ S_WHITESPACE,
                /* 033   ! (exclamation mark)         */ S_CHAR,
                /* 034   " (double quote)             */ S_DQUOTE,
                /* 035   # (number sign)              */ S_HASH,
                /* 036   $ (dollar sign)              */ S_CHAR,
                /* 037   % (percent)                  */ S_CHAR,
                /* 038   & (ampersand)                */ S_CHAR,
                /* 039   ' (single quote)             */ S_SQUOTE,
                /* 040   (  (left/open parenthesis)   */ S_PAROPEN,
                /* 041   )  (right/closing parenth.)  */ S_PARCLOSE,
                /* 042   * (asterisk)                 */ S_CHAR,
                /* 043   + (plus)                     */ S_PLUS,
                /* 044   , (comma)                    */ S_INVALID,
                /* 045   - (minus or dash)            */ S_MINUS,
                /* 046   . (dot)                      */ S_DOT,
                /* 047   / (forward slash)            */ S_CHAR,
                /* 048   0                            */ S_NUMBER,
                /* 049   1                            */ S_NUMBER,
                /* 050   2                            */ S_NUMBER,
                /* 051   3                            */ S_NUMBER,
                /* 052   4                            */ S_NUMBER,
                /* 053   5                            */ S_NUMBER,
                /* 054   6                            */ S_NUMBER,
                /* 055   7                            */ S_NUMBER,
                /* 056   8                            */ S_NUMBER,
                /* 057   9                            */ S_NUMBER,
                /* 058   : (colon)                    */ S_CHAR,
                /* 059   ; (semi-colon)               */ S_CHAR,
                /* 060   < (less than)                */ S_CHAR,
                /* 061   = (equal sign)               */ S_CHAR,
                /* 062   > (greater than)             */ S_CHAR,
                /* 063   ? (question mark)            */ S_CHAR,
                /* 064   @ (AT symbol)                */ S_CHAR,
                /* 065   A                            */ S_CHAR,
                /* 066   B                            */ S_CHAR,
                /* 067   C                            */ S_CHAR,
                /* 068   D                            */ S_CHAR,
                /* 069   E                            */ S_CHAR,
                /* 070   F                            */ S_CHAR,
                /* 071   G                            */ S_CHAR,
                /* 072   H                            */ S_CHAR,
                /* 073   I                            */ S_CHAR,
                /* 074   J                            */ S_CHAR,
                /* 075   K                            */ S_CHAR,
                /* 076   L                            */ S_CHAR,
                /* 077   M                            */ S_CHAR,
                /* 078   N                            */ S_CHAR,
                /* 079   O                            */ S_CHAR,
                /* 080   P                            */ S_CHAR,
                /* 081   Q                            */ S_CHAR,
                /* 082   R                            */ S_CHAR,
                /* 083   S                            */ S_CHAR,
                /* 084   T                            */ S_CHAR,
                /* 085   U                            */ S_CHAR,
                /* 086   V                            */ S_CHAR,
                /* 087   W                            */ S_CHAR,
                /* 088   X                            */ S_CHAR,
                /* 089   Y                            */ S_CHAR,
                /* 090   Z                            */ S_CHAR,
                /* 091   [ (left/opening bracket)     */ S_BRACKETOPEN,
                /* 092   \ (back slash)               */ S_CHAR,
                /* 093   ] (right/closing bracket)    */ S_BRACKETCLOSE,
                /* 094   ^ (caret/circumflex)         */ S_CHAR,
                /* 095   _ (underscore)               */ S_CHAR,
                /* 096   `                            */ S_CHAR,
                /* 097   a                            */ S_CHAR,
                /* 098   b                            */ S_CHAR,
                /* 099   c                            */ S_CHAR,
                /* 100   d                            */ S_CHAR,
                /* 101   e                            */ S_CHAR,
                /* 102   f                            */ S_CHAR,
                /* 103   g                            */ S_CHAR,
                /* 104   h                            */ S_CHAR,
                /* 105   i                            */ S_CHAR,
                /* 106   j                            */ S_CHAR,
                /* 107   k                            */ S_CHAR,
                /* 108   l                            */ S_CHAR,
                /* 109   m                            */ S_CHAR,
                /* 110   n                            */ S_CHAR,
                /* 111   o                            */ S_CHAR,
                /* 112   p                            */ S_CHAR,
                /* 113   q                            */ S_CHAR,
                /* 114   r                            */ S_CHAR,
                /* 115   s                            */ S_CHAR,
                /* 116   t                            */ S_CHAR,
                /* 117   u                            */ S_CHAR,
                /* 118   v                            */ S_CHAR,
                /* 119   w                            */ S_CHAR,
                /* 120   x                            */ S_CHAR,
                /* 121   y                            */ S_CHAR,
                /* 122   z                            */ S_CHAR,
                /* 123   { (left/opening brace)       */ S_BRACEOPEN,
                /* 124   | (vertical bar)             */ S_VERTBAR,
                /* 125   } (right/closing brace)      */ S_BRACECLOSE,
                /* 126   ~ (tilde)                    */ S_CHAR,
                /* 127 DEL (delete)                   */ S_INVALID,
                /* 128 -------- reserved ------------ */ S_INVALID,
                /* 129                                */ S_CHAR_FIXEDLEN,
                /* 130                                */ S_BRACKETOPEN_FIXEDLEN,
                /* 131                                */ S_VERTBAR_FIXEDLEN,
                /* 132                                */ S_BRACEOPEN_FIXEDLEN
        };

#define laddr(i) *(void *)(d_labels[i])

#define SEXP_LABELNUM_CHAR        65
#define SEXP_LABELNUM_CHAR_FIXED  129
#define SEXP_LABELNUM_NUMBER      48
#define SEXP_LABELNUM_DTYPE       91
#define SEXP_LABELNUM_DTYPE_FIXED 130
#define SEXP_LABELNUM_B64S        124
#define SEXP_LABELNUM_B64S_FIXED  131
#define SEXP_LABELNUM_B64E        123
#define SEXP_LABELNUM_B64E_FIXED  132

        SEXP_pstate_t *state;
        spb_size_t     spb_len;
        SEXP_t        *ref_l;

        uint8_t cur_c = 128;
        int     ret_p = SEXP_PRET_EUNDEF;

        struct SEXP_pext_dsc e_dsc;
        uint8_t _nbuffer[512];
        uint8_t *nbuffer;
        int dfa_state = S_START;

        /*
         * First check the parser state. In case it already exists update the internal
         * state variables as appropriate. If not then just allocate a new parser state.
         */
        if (*pstate != NULL) {
                state = *pstate;

                assume_d (state->p_buffer != NULL, NULL);

                if (spb_add (state->p_buffer, buffer, buflen) != 0) {
                        /* XXX */
                        return (NULL);
                }
        } else {
                state = *pstate = SEXP_pstate_new ();
                state->p_buffer = spb_new (buffer, buflen, 0);
                state->p_bufoff = 0;
                state->p_flags  = psetup->p_flags;
                SEXP_lstack_init (&state->l_stack);
        }

        assume_d (state != NULL, NULL);

        /* Initialize e_dsc before the main loop */
        e_dsc.p_buffer = state->p_buffer;
        e_dsc.p_bufoff = state->p_bufoff;
        e_dsc.p_explen = state->p_explen;
        e_dsc.p_flags  = state->p_flags;
        e_dsc.s_exp    = state->p_sexp;
        state->p_sexp  = NULL;
        e_dsc.sp_data  = state->sp_data;
        e_dsc.sp_free  = state->sp_free;

        for(int i = 0; i < SEXP_PFUNC_COUNT; ++i) {
                e_dsc.sp_shptr[i]  = state->sp_shptr[i];
                e_dsc.sp_shfree[i] = state->sp_shfree[i];
        }

        e_dsc.p_label    = state->p_label;
        e_dsc.p_numclass = state->p_numclass;
        e_dsc.p_numbase  = state->p_numbase;
        e_dsc.p_numstage = state->p_numstage;
        e_dsc.v_bool     = state->v_bool;

        assume_d (e_dsc.p_buffer != NULL, NULL);

        /* Get total buffer length */
        spb_len = spb_size (e_dsc.p_buffer);

        if (spb_len == 0) {
                ret_p = SEXP_PRET_SUCCESS;
                goto SKIP_LOOP;
        }

#ifndef NDEBUG
        if (e_dsc.p_label != SEXP_LABELNUM_CHAR_FIXED  &&
            e_dsc.p_label != SEXP_LABELNUM_DTYPE_FIXED &&
            e_dsc.p_label != SEXP_LABELNUM_B64S_FIXED  &&
            e_dsc.p_label != SEXP_LABELNUM_B64E_FIXED)
        {
                assume_d (e_dsc.p_bufoff + e_dsc.p_explen < spb_len, NULL);
        }
#endif

        ref_l = SEXP_lstack_top (&state->l_stack);

        if (e_dsc.p_explen > 0) {
                assume_d (e_dsc.s_exp   != NULL, NULL);
                assume_r (e_dsc.p_label != 128, NULL);

                dfa_state = d_states[e_dsc.p_label];
        } else if (e_dsc.s_exp != NULL) {
                dfa_state = S_NO_SEXP_ALLOC;
        }

        for (;;) {
                switch (dfa_state) {
                case S_START:
                        /*
                         * Allocate an empty S-exp. The value or type will be assigned in the
                         * subparser.
                         */
                        assume_d (e_dsc.s_exp == NULL, NULL); /* no leaks */
                        e_dsc.s_exp = SEXP_new ();
                        dfa_state = S_NO_SEXP_ALLOC;
                        break;

                L_NO_SEXP_ALLOC:
                case S_NO_SEXP_ALLOC:
                        if (e_dsc.p_bufoff >= spb_len)
                                goto SKIP_LOOP;

                        ret_p = SEXP_PRET_EUNDEF;
                        cur_c = spb_octet (e_dsc.p_buffer, e_dsc.p_bufoff);
                        dfa_state = S_NO_CURC_UPDATE;
                        break;

                L_NO_CURC_UPDATE:
                case S_NO_CURC_UPDATE:
                        assume_d (e_dsc.s_exp != NULL, NULL);
                        assume_d (ret_p == SEXP_PRET_EUNDEF, NULL);
                        assume_d (cur_c != 128, NULL);
                        /*
                         * Jump to subparser block. cur_c holds the first character of the next
                         * expression.
                         */
                        if (__predict(cur_c < 128, 1))
                                dfa_state = d_states[cur_c];
                        else
                                dfa_state = S_INVALID;
                        break;

                L_CHAR:
                case S_CHAR:
                        e_dsc.p_label = SEXP_LABELNUM_CHAR;

                        if ((ret_p = psetup->p_funcp[SEXP_PFUNC_UL_STRING_SI](&e_dsc)) != SEXP_PRET_SUCCESS)
                                goto SKIP_LOOP;
                        dfa_state = S_SEXP_ADD;
                        break;

                L_CHAR_FIXEDLEN:
                case S_CHAR_FIXEDLEN:
                        e_dsc.p_label = SEXP_LABELNUM_CHAR_FIXED;

                        if (e_dsc.p_bufoff + e_dsc.p_explen > spb_len) {
                                ret_p = SEXP_PRET_EUNFIN;
                                goto SKIP_LOOP;
                        }
                        if ((ret_p = psetup->p_funcp[SEXP_PFUNC_KL_STRING](&e_dsc)) != SEXP_PRET_SUCCESS)
                                goto SKIP_LOOP;
                        dfa_state = S_SEXP_ADD;
                        break;

                L_DQUOTE:
                case S_DQUOTE:
                        if ((ret_p = psetup->p_funcp[SEXP_PFUNC_UL_STRING_DQ](&e_dsc)) != SEXP_PRET_SUCCESS)
                                goto SKIP_LOOP;
                        dfa_state = S_SEXP_ADD;
                        break;

                L_SQUOTE:
                case S_SQUOTE:
                        if ((ret_p = psetup->p_funcp[SEXP_PFUNC_UL_STRING_SQ](&e_dsc)) != SEXP_PRET_SUCCESS)
                                goto SKIP_LOOP;
                        dfa_state = S_SEXP_ADD;
                        break;

                L_DOT:
                case S_DOT:

#define SEXP_NUMSTAGE_CONT_INT   0
#define SEXP_NUMSTAGE_FINAL_EXP  1
#define SEXP_NUMSTAGE_FINAL_EXP2 2
#define SEXP_NUMSTAGE_FINAL_FLT  3
#define SEXP_NUMSTAGE_3          4

                        if (e_dsc.p_bufoff + 1 < spb_len) {
                                ++e_dsc.p_explen;

                                if (isdigit (spb_octet (e_dsc.p_buffer, e_dsc.p_bufoff + e_dsc.p_explen))) {
                                        e_dsc.p_numclass = SEXP_NUMCLASS_FLT;
                                        e_dsc.p_label    = SEXP_LABELNUM_NUMBER;
                                        goto L_NUMBER_final_flt;
                                } else
                                        goto L_CHAR;
                        } else {
                                if (e_dsc.p_flags & SEXP_PFLAG_EOFOK)
                                        goto L_CHAR;
                                else {
                                        ret_p = SEXP_PRET_EUNFIN;
                                        goto SKIP_LOOP;
                                }
                        }
                        /* NOTREACHED */

                L_PLUS:
                L_MINUS:
                case S_PLUS:
                case S_MINUS:
                        if (e_dsc.p_bufoff + 1 < spb_len) {
                                register uint8_t tmp_c;

                                tmp_c = spb_octet (e_dsc.p_buffer, e_dsc.p_bufoff + 1);

                                if (isdigit (tmp_c)) {
                                        e_dsc.p_numclass = SEXP_NUMCLASS_INT;
                                        e_dsc.p_numstage = SEXP_NUMSTAGE_CONT_INT;
                                        e_dsc.p_label    = SEXP_LABELNUM_NUMBER;

                                        ++e_dsc.p_explen;

                                        goto L_NUMBER_cont_int;
                                } else if (tmp_c == '.') {
                                        if (e_dsc.p_bufoff + 2 < spb_len) {
                                                ++e_dsc.p_explen;
                                                ++e_dsc.p_explen;

                                                if (isdigit (spb_octet (e_dsc.p_buffer, e_dsc.p_bufoff + 2))) {
                                                        e_dsc.p_numclass = SEXP_NUMCLASS_FLT;
                                                        e_dsc.p_label    = SEXP_LABELNUM_NUMBER;
                                                        goto L_NUMBER_final_flt;
                                                } else
                                                        goto L_CHAR;
                                        } else {
                                                if (e_dsc.p_flags & SEXP_PFLAG_EOFOK) {
                                                        ++e_dsc.p_explen;
                                                        ++e_dsc.p_explen;

                                                        goto L_CHAR;
                                                } else {
                                                        ret_p = SEXP_PRET_EUNFIN;
                                                        goto SKIP_LOOP;
                                                }
                                        }
                                } else {
                                        ++e_dsc.p_explen;
                                        goto L_CHAR;
                                }
                        } else {
                                if (e_dsc.p_flags & SEXP_PFLAG_EOFOK) {
                                        ++e_dsc.p_explen;
                                        goto L_CHAR;
                                } else {
                                        ret_p = SEXP_PRET_EUNFIN;
                                        goto SKIP_LOOP;
                                }
                        }
                        goto SKIP_LOOP;
                        /* NOTREACHED */

                L_NUMBER:
                case S_NUMBER:
                        e_dsc.p_label = SEXP_LABELNUM_NUMBER;

                        /*
                         * Numbers are handled in a special way because they can have the meaning of
                         * a "length prefix". Length prefix is always related to a expression and in
                         * our case it's always the very next expression in the buffer. In some cases
                         * the lenght prefix and the expression are separated with a colon character.
                         */
                        if (e_dsc.p_explen > 0) {
                                const void *n_labels[] = {
                                        &&L_NUMBER_cont_int,
                                        &&L_NUMBER_final_exp,
                                        &&L_NUMBER_final_exp2,
                                        &&L_NUMBER_final_flt,
                                        &&L_NUMBER_stage3
                                };

                                assume_d (e_dsc.p_numstage < (sizeof n_labels/sizeof (void *)), SEXP_PRET_EUNDEF);

                                goto *(void *)(n_labels[e_dsc.p_numstage]);
                        }
                        //L_NUMBER_stage1:

                        if (cur_c == '.') {
                                ++e_dsc.p_explen;
                        L_NUMBER_final_flt:
                                e_dsc.p_numclass = SEXP_NUMCLASS_FLT;

                                spb_iterate (e_dsc.p_buffer, e_dsc.p_bufoff + e_dsc.p_explen, cur_c,
                                             if (!isdigit (cur_c)) {
                                                     if (isnextexp (cur_c))
                                                             goto L_NUMBER_stage3;
                                                     else {
                                                             if (cur_c == 'e' || cur_c == 'E') {
                                                                    ++e_dsc.p_explen;
                                                                    goto L_NUMBER_final_exp;
                                                             } else {
                                                                    goto L_NUMBER_invalid;
                                                             }
                                                     }
                                             }

                                             ++e_dsc.p_explen;
                                        );

                                if (e_dsc.p_flags & SEXP_PFLAG_EOFOK)
                                        goto L_NUMBER_stage3;
                                else {
                                        ret_p = SEXP_PRET_EUNFIN;
                                        e_dsc.p_numstage = SEXP_NUMSTAGE_FINAL_FLT;

                                        goto SKIP_LOOP;
                                }
                        } else {
                                if (isdigit (cur_c)) {
                                        if (e_dsc.p_numclass != SEXP_NUMCLASS_INT)
                                                e_dsc.p_numclass = SEXP_NUMCLASS_UINT;

                                        ++e_dsc.p_explen;
                                L_NUMBER_cont_int:
                                        spb_iterate (e_dsc.p_buffer, e_dsc.p_bufoff + e_dsc.p_explen, cur_c,
                                                     if (!isdigit (cur_c))
                                                             goto L_NUMBER_stage2;

                                                     ++e_dsc.p_explen;
                                                );

                                        if (e_dsc.p_flags & SEXP_PFLAG_EOFOK)
                                                goto L_NUMBER_stage3;
                                        else {
                                                ret_p = SEXP_PRET_EUNFIN;
                                                e_dsc.p_numstage = SEXP_NUMSTAGE_CONT_INT;

                                                goto SKIP_LOOP;
                                        }
                                }

                                goto L_NUMBER_invalid;
                        }
                        goto SKIP_LOOP;
                L_NUMBER_stage2:

                        if (cur_c == '.') {
                                ++e_dsc.p_explen;
                                goto L_NUMBER_final_flt;
                        }
                        if (cur_c == 'e' || cur_c == 'E') {
                                ++e_dsc.p_explen;
                        L_NUMBER_final_exp:
                                e_dsc.p_numclass = SEXP_NUMCLASS_EXP;

                                if (e_dsc.p_bufoff + e_dsc.p_explen < spb_len) {
                                        cur_c = spb_octet (e_dsc.p_buffer, e_dsc.p_bufoff + e_dsc.p_explen);

                                        if (cur_c == '+' || cur_c == '-') {
                                                ++e_dsc.p_explen;
                                        }
                                L_NUMBER_final_exp2:
                                        spb_iterate (e_dsc.p_buffer, e_dsc.p_bufoff + e_dsc.p_explen, cur_c,
                                                     if (!isdigit (cur_c)) {
                                                             if (isdigit (spb_octet (e_dsc.p_buffer,
                                                                                     e_dsc.p_bufoff + e_dsc.p_explen - 1)))
                                                             {
                                                                     /*
                                                                      * We've reached some non-digit character but the previous
                                                                      * one was a digit - we consider this to be the end of the
                                                                      * exponent
                                                                      */
                                                                     goto L_NUMBER_stage3;
                                                             } else {
                                                                     /*
                                                                      * Only digits are allowed right after the sign of exponent
                                                                      * characters
                                                                      */
                                                                     goto L_NUMBER_invalid;
                                                             }
                                                     }
                                                     ++e_dsc.p_explen;
                                                );

                                        if (e_dsc.p_flags & SEXP_PFLAG_EOFOK)
                                                goto L_NUMBER_stage3;
                                        else {
                                                ret_p = SEXP_PRET_EUNFIN;
                                                e_dsc.p_numstage = SEXP_NUMSTAGE_FINAL_EXP2;

                                                goto SKIP_LOOP;
                                        }
                                } else {
                                        if (e_dsc.p_flags & SEXP_PFLAG_EOFOK)
                                                goto L_NUMBER_invalid;
                                }

                                if (e_dsc.p_flags & SEXP_PFLAG_EOFOK)
                                        goto L_NUMBER_stage3;
                                else {
                                        ret_p = SEXP_PRET_EUNFIN;
                                        e_dsc.p_numstage = SEXP_NUMSTAGE_FINAL_EXP;

                                        goto SKIP_LOOP;
                                }
                        }
                L_NUMBER_stage3:
                        /*
                         * Find out whether the number parsed in the previous stages
                         * in a length prefix. Length prefix a non-negative integer
                         * followed by an expression.
                         *
                         * We need the number to be stored in a continues memory region
                         * because the string to number conversion API doesn't support
                         * sparse buffers currently. However, if the whole number happens
                         * to be stored in just one buffer, we can get a pointer to it
                         * using the spb_direct function. In other case, we try to copy
                         * the number to a statically allocated buffer and if it's too
                         * large we allocate a new buffer on the heap.
                         */

                        e_dsc.sp_data = (void *)spb_direct (e_dsc.p_buffer, e_dsc.p_bufoff, e_dsc.p_explen);
                        e_dsc.sp_free = NULL;

                        if (e_dsc.sp_data == NULL) {
                                if (e_dsc.p_explen <= (sizeof _nbuffer / sizeof (uint8_t)))
                                        e_dsc.sp_data = (void *)_nbuffer;
                                else {
                                        e_dsc.sp_data = sm_alloc (sizeof (uint8_t) * e_dsc.p_explen);
                                        e_dsc.sp_free = sm_free;

                                        if (e_dsc.sp_data == NULL) {
                                                e_dsc.p_numstage = SEXP_NUMSTAGE_3;
                                                ret_p = SEXP_PRET_EUNFIN;
                                                goto SKIP_LOOP;
                                        }
                                }

                                if (spb_pick (e_dsc.p_buffer, e_dsc.p_bufoff,
                                              e_dsc.p_explen, e_dsc.sp_data) != 0)
                                {
                                        ret_p = SEXP_PRET_EUNDEF;
                                        goto SKIP_LOOP;
                                }
                        }

                        nbuffer = (uint8_t *)e_dsc.sp_data;
                        cur_c   = spb_octet (e_dsc.p_buffer, e_dsc.p_bufoff + e_dsc.p_explen);

                        if (e_dsc.p_numclass == SEXP_NUMCLASS_UINT) {
                                uint64_t explen = strto_uint64_dec ((char *)nbuffer, e_dsc.p_explen, NULL);

                                if (explen == 0 && (errno == EINVAL || errno == ERANGE)) {
                                        ret_p = SEXP_PRET_EINVAL;
                                        goto SKIP_LOOP;
                                }

                                if (cur_c == ':') {
                                        e_dsc.p_bufoff  += e_dsc.p_explen + 1 /* skip colon */;
                                        e_dsc.p_explen   = (spb_size_t)explen;
                                        e_dsc.p_numclass = SEXP_NUMCLASS_PRE;
                                        goto L_CHAR_FIXEDLEN;
                                } else if (cur_c == '|') {
                                        e_dsc.p_bufoff  += e_dsc.p_explen;
                                        e_dsc.p_explen   = (spb_size_t)explen;
                                        e_dsc.p_numclass = SEXP_NUMCLASS_PRE;
                                        goto L_VERTBAR_FIXEDLEN;
                                } else if (cur_c == '[') {
                                        e_dsc.p_bufoff  += e_dsc.p_explen;
                                        e_dsc.p_explen   = (spb_size_t)explen;
                                        e_dsc.p_numclass = SEXP_NUMCLASS_PRE;
                                        goto L_BRACKETOPEN_FIXEDLEN;
                                } else if (cur_c == '{') {
                                        e_dsc.p_bufoff  += e_dsc.p_explen;
                                        e_dsc.p_explen   = (spb_size_t)explen;
                                        e_dsc.p_numclass = SEXP_NUMCLASS_PRE;
                                        goto L_BRACEOPEN_FIXEDLEN;
                                }
                        }

                        if (e_dsc.p_bufoff + e_dsc.p_explen < spb_len) {
                                if (!isnextexp (cur_c))
                                        goto L_NUMBER_invalid;
                        }

                        if (e_dsc.p_explen > 0) {
                                SEXP_val_t v_dsc;

                                if (e_dsc.p_numclass == SEXP_NUMCLASS_INT) {
                                        int64_t number = strto_int64_dec ((char *)nbuffer, e_dsc.p_explen, NULL);

                                        if (errno == ERANGE) {
                                                goto L_NUMBER_invalid;
                                        }
                                        if (errno == EINVAL) {
#ifndef NDEBUG
                                                abort ();
#endif
                                                goto L_NUMBER_invalid;
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
                                } else if (e_dsc.p_numclass == SEXP_NUMCLASS_UINT) {
                                        uint64_t number = strto_uint64_dec ((char *)nbuffer, e_dsc.p_explen, NULL);

                                        if (errno == ERANGE) {
                                                goto L_NUMBER_invalid;
                                        }
                                        if (errno == EINVAL) {
#ifndef NDEBUG
                                                abort ();
#endif
                                                goto L_NUMBER_invalid;
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
                                } else if (e_dsc.p_numclass == SEXP_NUMCLASS_FLT || e_dsc.p_numclass == SEXP_NUMCLASS_EXP) {
                                        double number;

                                        number = strto_double ((char *)nbuffer, e_dsc.p_explen, NULL);

                                        if (errno == ERANGE) {
                                                goto L_NUMBER_invalid;
                                        }
                                        if (errno == EINVAL) {
#ifndef NDEBUG
                                                abort ();
#endif
                                                goto L_NUMBER_invalid;
                                        }

                                        if (SEXP_val_new (&v_dsc, sizeof (struct SEXP_val_num_f),
                                                          SEXP_VALTYPE_NUMBER) != 0)
                                        {
                                                /* TODO: handle this */
                                                abort ();
                                        }
                                        SEXP_NCASTP(f,v_dsc.mem)->n = (double)number;
                                        SEXP_NCASTP(f,v_dsc.mem)->t = SEXP_NUM_DOUBLE;
                                } else {
                                        /* Unknown number class */
#ifndef NDEBUG
                                        abort ();
#endif
                                        goto L_NUMBER_invalid;
                                }
                                assume_d (e_dsc.s_exp != NULL, SEXP_PRET_EUNDEF);
                                /* update the value pointer */
                                e_dsc.s_exp->s_valp = SEXP_val_ptr (&v_dsc);
                                ret_p = SEXP_PRET_SUCCESS;

                                dfa_state = S_SEXP_ADD;
                                break;
                        }
                L_NUMBER_invalid:
                        ret_p = SEXP_PRET_EINVAL;
                        goto SKIP_LOOP;

                L_HASH:
                case S_HASH:
                        /*
                         * #<1:T><1..n:number>
                         *
                         *  T - bool, true
                         *  F - bool, false
                         *  b - binary
                         *  o - octal
                         *  d - decimal
                         *  x - hexadecimal
                         *
                         * Some examples: decimal 255 as...
                         *  #xff       - hexadecimal (base 16)
                         *  #d255      - decimal (base 10)
                         *  #o377      - octal   (base 8)
                         *  #b11111111 - binary  (base 2)
                         */
                        if (e_dsc.p_bufoff + 1 < spb_len) {
                                uint8_t cur_oct = spb_octet(e_dsc.p_buffer, e_dsc.p_bufoff + 1);
                                if (cur_oct == 'T') {
                                        ++e_dsc.p_bufoff;

                                        if ((ret_p = SEXP_parse_bool (&e_dsc, true)) != SEXP_PRET_SUCCESS)
                                                goto SKIP_LOOP;
                                        else {
                                                dfa_state = S_SEXP_ADD;
                                                break;
                                        }
                                } else if (cur_oct == 'F') {
                                        ++e_dsc.p_bufoff;

                                        if ((ret_p = SEXP_parse_bool (&e_dsc, false)) != SEXP_PRET_SUCCESS)
                                                goto SKIP_LOOP;
                                        else {
                                                dfa_state = S_SEXP_ADD;
                                                break;
                                        }
                                } else if (cur_oct == 'b') {
                                        e_dsc.p_numbase = 2;
                                } else if (cur_oct == 'd') {
                                        e_dsc.p_numbase = 10;
                                } else if (cur_oct == 'x') {
                                        e_dsc.p_numbase = 16;
                                } else if (cur_oct == 'o') {
                                        e_dsc.p_numbase = 8;
                                } else {
                                        ret_p = SEXP_PRET_EINVAL;
                                        goto SKIP_LOOP;
                                }

                                if (e_dsc.p_bufoff + 2 < spb_len) {
                                        ++e_dsc.p_bufoff;
                                        ++e_dsc.p_bufoff;

                                        cur_c = spb_octet (e_dsc.p_buffer, e_dsc.p_bufoff);

                                        if (cur_c == '+' || cur_c == '-' || cur_c == '.') {
                                                goto laddr(cur_c);
                                        }
                                        if (isdigit(cur_c)) {
                                                goto laddr(cur_c);
                                        }

                                        ret_p = SEXP_PRET_EINVAL;
                                        goto SKIP_LOOP;
                                }
                        }

                        ret_p = SEXP_PRET_EUNFIN;
                        e_dsc.p_label = '#';

                        goto SKIP_LOOP;

                L_WHITESPACE:
                case S_WHITESPACE:
                        spb_iterate (e_dsc.p_buffer, e_dsc.p_bufoff, cur_c,
                                     if (!isspace (cur_c))
                                             goto L_NO_CURC_UPDATE;
                                     ++e_dsc.p_bufoff;
                                );

                        ret_p = SEXP_PRET_SUCCESS;
                        goto SKIP_LOOP;

                L_PAROPEN:
                case S_PAROPEN:
                        {
                                SEXP_t *ref_h, *ref_s;

                                ref_h = SEXP_list_new (NULL);

                                /*
                                 * Update the s_type & s_flgs fields. This ensures
                                 * that a previously processed type/hint doesn't get
                                 * lost.
                                 */
                                ref_h->s_type = e_dsc.s_exp->s_type;
                                e_dsc.s_exp->s_type = NULL;

                                ref_s = SEXP_softref (ref_h);
                                SEXP_list_add (ref_l, ref_h);
                                SEXP_free (ref_h);
                                SEXP_lstack_push (&state->l_stack, ref_s);
                                ref_l = ref_s;

                                /*
                                 * Unset the EOFOK flag if we are inside a list
                                 */
                                if (SEXP_lstack_depth (&state->l_stack) == 2) {
                                        state->p_flags0 = e_dsc.p_flags;
                                        e_dsc.p_flags  &= ~(SEXP_PFLAG_EOFOK);
                                }
                        }

                        ret_p = SEXP_PRET_EUNFIN;
                        ++e_dsc.p_bufoff;

                        goto L_NO_SEXP_ALLOC;

                L_PARCLOSE:
                case S_PARCLOSE:
                        if (e_dsc.s_exp->s_type != NULL) {
                                ret_p = SEXP_PRET_EINVAL;
                                goto SKIP_LOOP;
                        }

                        if (SEXP_lstack_depth (&state->l_stack) > 1) {
                                SEXP_t *ref_t;

                                ref_t = SEXP_lstack_pop (&state->l_stack);
                                SEXP_free (ref_t);
                                ref_l = SEXP_lstack_top (&state->l_stack);

                                if (SEXP_lstack_depth (&state->l_stack) == 1)
                                        e_dsc.p_flags = state->p_flags0;

                                ++e_dsc.p_bufoff;
                                ret_p = SEXP_PRET_SUCCESS;

                                goto L_NO_SEXP_ALLOC;
                        } else {
                                ret_p = SEXP_PRET_EINVAL;
                                goto SKIP_LOOP;
                        }
                        /* NOTREACHED */

                L_BRACKETOPEN:
                case S_BRACKETOPEN:
                        e_dsc.p_label = SEXP_LABELNUM_DTYPE;

                        if ((ret_p = psetup->p_funcp[SEXP_PFUNC_UL_DATATYPE](&e_dsc)) != SEXP_PRET_SUCCESS)
                                goto SKIP_LOOP;

                        e_dsc.p_bufoff += e_dsc.p_explen;
                        e_dsc.p_explen  = 0;
                        ret_p = SEXP_PRET_EUNFIN;

                        goto L_NO_SEXP_ALLOC;

                L_BRACKETOPEN_FIXEDLEN:
                case S_BRACKETOPEN_FIXEDLEN:
                        e_dsc.p_label = SEXP_LABELNUM_DTYPE_FIXED;

                        if (e_dsc.p_bufoff + e_dsc.p_explen + 2 /* [] */ > spb_len) {
                                ret_p = SEXP_PRET_EUNFIN;
                                goto SKIP_LOOP;
                        } else {
                                if ((ret_p = psetup->p_funcp[SEXP_PFUNC_KL_DATATYPE](&e_dsc)) != SEXP_PRET_SUCCESS)
                                        goto SKIP_LOOP;

                                e_dsc.p_bufoff += e_dsc.p_explen;
                                e_dsc.p_explen  = 0;
                                ret_p = SEXP_PRET_EUNFIN;

                                goto L_NO_SEXP_ALLOC;
                        }
                        /* NOTREACHED */

                L_BRACEOPEN:
                case S_BRACEOPEN:
                        e_dsc.p_label = SEXP_LABELNUM_B64E;
                        /* b64 decode - not supported yet */
                        ret_p = SEXP_PRET_EINVAL;
                        goto SKIP_LOOP;

                L_BRACEOPEN_FIXEDLEN:
                case S_BRACEOPEN_FIXEDLEN:
                        e_dsc.p_label = SEXP_LABELNUM_B64E_FIXED;
                        /* b64 decode - not supported yet */
                        ret_p = SEXP_PRET_EINVAL;
                        goto SKIP_LOOP;

                L_VERTBAR:
                case S_VERTBAR:
                        e_dsc.p_label = SEXP_LABELNUM_B64S;

                        if ((ret_p = psetup->p_funcp[SEXP_PFUNC_UL_STRING_B64](&e_dsc)) != SEXP_PRET_SUCCESS)
                                goto SKIP_LOOP;
                        dfa_state = S_SEXP_ADD;
                        break;

                L_VERTBAR_FIXEDLEN:
                case S_VERTBAR_FIXEDLEN:
                        e_dsc.p_label = SEXP_LABELNUM_B64S_FIXED;

                        if ((ret_p = psetup->p_funcp[SEXP_PFUNC_KL_STRING_B64](&e_dsc)) != SEXP_PRET_SUCCESS)
                                goto SKIP_LOOP;
                        dfa_state = S_SEXP_ADD;
                        break;

                L_NUL:
                L_BRACECLOSE:
                L_BRACKETCLOSE:
                L_INVALID:
                case S_NUL:
                case S_BRACECLOSE:
                case S_BRACKETCLOSE:
                case S_INVALID:
                        /*
                         * Denied parser state
                         */
#ifndef NDEBUG
                        abort ();
#endif
                        goto SKIP_LOOP;

                case S_SEXP_ADD:
                        /*
                         * Add new S-exp to the list at the top of the list stack
                         */
                        SEXP_list_add (ref_l, e_dsc.s_exp);
                        SEXP_free (e_dsc.s_exp);

                        e_dsc.s_exp = NULL;
                        e_dsc.p_bufoff += e_dsc.p_explen;
                        e_dsc.p_explen  = 0;

                        e_dsc.p_numclass = SEXP_NUMCLASS_INV;
                        e_dsc.p_numbase  = 0;
                        e_dsc.p_numstage = 255;

                        dfa_state = S_START;
                }
        } /* for (;;) */

SKIP_LOOP:
        assume_d (SEXP_lstack_depth (&state->l_stack) > 0, SEXP_PRET_EUNDEF);

        switch (ret_p) {
        case SEXP_PRET_SUCCESS:
                if (SEXP_lstack_depth (&state->l_stack) == 1) {
                        SEXP_t *s_list;
                        /*
                         * Save the reference to the top-level list and free parser state.
                         */
                        s_list = SEXP_lstack_list (&state->l_stack);
                        SEXP_pstate_free (state);
                        *pstate = NULL;

                        if (e_dsc.s_exp != NULL)
                                SEXP_free (e_dsc.s_exp);

                        return (s_list);
                }
                /* FALLTHROUGH */
        case SEXP_PRET_EUNFIN:
                /*
                 * The last S-expression parsed was considered incomplete and we
                 * need more data to successfully parse it. An invocation of the
                 * parser with the state saved at pstate and with a new buffer
                 * containing the rest of the (valid) S-expression may succeed.
                 */
                state->p_buffer = e_dsc.p_buffer;
                state->p_bufoff = e_dsc.p_bufoff;
                state->p_explen = e_dsc.p_explen;
                state->p_flags  = e_dsc.p_flags;
                state->p_sexp   = e_dsc.s_exp;
                state->sp_data  = e_dsc.sp_data;
                state->sp_free  = e_dsc.sp_free;

                for(int i = 0; i < SEXP_PFUNC_COUNT; ++i) {
                        state->sp_shptr[i]  = e_dsc.sp_shptr[i];
                        state->sp_shfree[i] = e_dsc.sp_shfree[i];
                }

                state->p_label    = e_dsc.p_label;
                state->p_numclass = e_dsc.p_numclass;
                state->p_numbase  = e_dsc.p_numbase;
                state->p_numstage = e_dsc.p_numstage;
		state->p_error    = SEXP_PRET_EUNFIN;

                return (NULL);
        case SEXP_PRET_EINVAL:
		state->p_error = SEXP_PRET_EINVAL;
                /*
                 * The parser encoutered an invalid sequence of octets
                 */
                return (NULL);
        case SEXP_PRET_EUNDEF:
		state->p_error = SEXP_PRET_EUNDEF;
                /*
                 * Undefined error (i.e. we don't know how to handle the error
                 * that caused the undefined state or we do not expect that such
                 * errors happen in a "normal" environment - their probability is
                 * very low).
                 */
                return (NULL);
        }

        /* NOTREACHED */
        return (NULL);
}

/**
 * Parse a simple string expression of unknown length
 * @param dsc state description structure
 */
__PARSE_RT SEXP_parse_ul_string_si (__PARSE_PT(dsc))
{
        spb_size_t itb;
        register spb_size_t cnt;
        register uint8_t    oct;

        assume_d (dsc != NULL, SEXP_PRET_EUNDEF);
        assume_d (dsc->p_buffer != NULL, SEXP_PRET_EUNDEF);

        /*
         * From where to start the iteration over
         * octets of the sparse buffer.
         */

        if (dsc->p_explen == 0) {
                itb = dsc->p_bufoff + 1 /* count in the first char */;
                cnt = 1;
        } else {
                itb = dsc->p_bufoff + dsc->p_explen;
                cnt = 0;
        }

        spb_iterate (dsc->p_buffer, itb, oct,
                     if (isnextexp (oct))
                             goto found;
                     ++cnt;
                );

        /*
         * === Implementation note #1 ===
         *
         * We've reached EOB before reaching the end of the string.
         * What happens now depends on parser flags. If we are inside
         * a list (not counting in the implicit top level list) then
         * we can't decide whether the EOB is also end of the string
         * and we should return EUNFIN here.
         *  In the other case we consider EOB as end of the string.
         */

        if (!(dsc->p_flags & SEXP_PFLAG_EOFOK)) {
                /* update length of the processed expression */
                dsc->p_explen += cnt;

                return (SEXP_PRET_EUNFIN);
        }
found:
        /*
         * update length of the processed expression - after this
         * step the e_length contains the whole length of the textual
         * representation of the expression
         */
        dsc->p_explen += cnt;

        /* the buffer can't be smaller than the expression */
        assume_d (spb_size (dsc->p_buffer) >= dsc->p_explen, SEXP_PRET_EUNDEF);
        assume_r (spb_size (dsc->p_buffer) >= dsc->p_bufoff + dsc->p_explen, SEXP_PRET_EUNDEF);

        {/******************************************************************************************/
                /*
                 * If there isn't a datatype (hint) assigned to the S-exp
                 * then we can just create the value.
                 */
                SEXP_val_t v_dsc; /* XXX: set to zero in debugging mode (compiler flags?) */

                if (SEXP_val_new (&v_dsc, sizeof (char) * dsc->p_explen,
                                  SEXP_VALTYPE_STRING) != 0)
                {
                        /*
                         * SEXP_val_new should always succeed. In case it doesn't
                         * it's an undefined error.
                         */
                        return (SEXP_PRET_EUNDEF);
                }

                assume_d (v_dsc.mem != NULL, NULL);

                if (spb_pick (dsc->p_buffer, dsc->p_bufoff, dsc->p_explen, v_dsc.mem) != 0)
                {
                        return (SEXP_PRET_EUNDEF);
                }

                /*
                 * Update the S-exp value pointer to the newly created value
                 */
                assume_d(dsc->s_exp != NULL, SEXP_PRET_EUNDEF);
                dsc->s_exp->s_valp = SEXP_val_ptr (&v_dsc);
        }/******************************************************************************************/

        /*
         * === Implementation note #2 ===
         *
         * Before we exit from an atomic expression parser function after a
         * complete parser cycle (i.e. returing SEXP_PRET_SUCCESS) we have to
         * ensure that the state description structure is updated to hold values
         * expressing the state after all processing we've done in this function.
         *  Especially the e_length value should containg the length of the
         * expression in it's textual representation so that the main parser can
         * update it's state correctly.
         */
        return (SEXP_PRET_SUCCESS);
}

__PARSE_RT SEXP_parse_ul_string_dq (__PARSE_PT(dsc))
{
        spb_size_t itb;
        strbuf_t  *strbuf;

        register spb_size_t noesc_s, noesc_l;
        register bool       esc = false;
        register uint8_t    oct;

        assume_d (dsc != NULL, SEXP_PRET_EUNDEF);
        assume_d (dsc->p_buffer != NULL, SEXP_PRET_EUNDEF);

        /* count in the starting quote if this is the first invocation */
        if (dsc->p_explen == 0) {
                ++dsc->p_explen;
                itb = dsc->p_bufoff + dsc->p_explen;

                /* initialize the string buffer */
                strbuf = strbuf_new (SEAP_STRBUF_MAX);

                if (strbuf == NULL)
                        return (SEXP_PRET_EUNDEF);
                else {
                        dsc->sp_data = (void *)strbuf;
                        dsc->sp_free = (void (*)(void *))&strbuf_free;
                }
        } else {
                itb = dsc->p_bufoff + dsc->p_explen;
                strbuf = (strbuf_t *)dsc->sp_data;
        }

        /*
         * Iterate thru the buffer searching for escaped characters or an unescaped
         * double quote character (end of the string). During the iteration we copy
         * blocks without escaped character into the string buffer at dsc->sp_data
         * and we do this either when an escaped character or the end of string is
         * found. For each such subblock we keep it's relative starting index and
         * the length.
         */

        noesc_s = 0;
        noesc_l = 0;

        spb_iterate (dsc->p_buffer, itb, oct,
                     /* Use branch prediction? */
                     if (__predict(!esc, 1)) {
                             switch (oct) {
                             case '\\':
                                     esc = true;
                                     break;
                             case '"':
                                     goto found;
                             default:
                                     ++noesc_l;
                             }
                     } else {
                             /* Copy noescape block into strbuf */
                             if (noesc_l > 0) {
                                     if (spb_pick_cb (dsc->p_buffer, itb + noesc_s, noesc_l,
                                                      (void *(*)(void *, void *, size_t)) &strbuf_add, (void *)strbuf) != 0)
                                     {
                                             return (SEXP_PRET_EUNDEF);
                                     }
                             }

                             /* Handle escape character */
                             switch (oct) {
                             case 'n': /* New line */
                                     oct = '\n';
                                     break;
                             case 't': /* Horizontal tab */
                                     oct = '\t';
                                     break;
                             case 'r': /* Cariage return */
                                     oct = '\r';
                                     break;
                             case '0': /* Null byte */
                                     oct = '\0';
                                     break;
                             case 'x': /* Hexadecimal - two more character needed */
                                     abort ();
                             case 'a': /* Alert (beep) */
                                     oct = '\a';
                                     break;
                             case 'b': /* Backspace */
                                     oct = '\b';
                                     break;
                             case 'v': /* Vertical tab */
                                     oct = '\v';
                                     break;
                             case 'f': /* Form feed */
                                     oct = '\f';
                                     break;
                             }

                             if (strbuf_addc (strbuf, oct) != 0)
                                     return (SEXP_PRET_EUNDEF);

                             dsc->p_explen += noesc_l + 2 /* backslash + char */;
                             noesc_s       += noesc_l + 2;
                             noesc_l        = 0;
                             esc            = false;
                     }
                );

        if (noesc_l > 0) {
                if (spb_pick_cb (dsc->p_buffer, itb + noesc_s, noesc_l,
                                 (void *(*)(void *, void *, size_t))&strbuf_add, (void *)strbuf) != 0)
                {
                        return (SEXP_PRET_EUNDEF);
                }

                dsc->p_explen += noesc_l;
        }

        return (SEXP_PRET_EUNFIN);
found:
        if (noesc_l > 0) {
                if (spb_pick_cb (dsc->p_buffer, itb + noesc_s, noesc_l,
                                 (void *(*)(void *, void *, size_t))&strbuf_add, (void *)strbuf) != 0)
                {
                        return (SEXP_PRET_EUNDEF);
                }

                dsc->p_explen += noesc_l + 1;
        } else
                ++dsc->p_explen; /* count in the last quote */;

        assume_d (spb_size (dsc->p_buffer) >= dsc->p_explen, SEXP_PRET_EUNDEF);

        {/******************************************************************************************/
                SEXP_val_t v_dsc;
                size_t     sz;

                sz = strbuf_size (strbuf);

                assume_r (spb_size (dsc->p_buffer) >= itb + sz, SEXP_PRET_EUNDEF);

                if (SEXP_val_new (&v_dsc, sizeof (char) * sz,
                                  SEXP_VALTYPE_STRING) != 0)
                {
                        /*
                         * SEXP_val_new should always succeed. In case it doesn't
                         * it's an undefined error.
                         */
                        return (SEXP_PRET_EUNDEF);
                }

                if (strbuf_copy (strbuf, v_dsc.mem, sz) == NULL) {
                        /* XXX: leak - v_dsc */
                        return (SEXP_PRET_EUNDEF);
                }

                assume_d(dsc->s_exp != NULL, SEXP_PRET_EUNDEF);
                dsc->s_exp->s_valp = SEXP_val_ptr (&v_dsc);
        }/******************************************************************************************/

        strbuf_free (strbuf);
        dsc->sp_data = NULL;
        dsc->sp_free = NULL;

        return (SEXP_PRET_SUCCESS);
}

__PARSE_RT SEXP_parse_ul_string_sq (__PARSE_PT(dsc))
{
        spb_size_t itb;
        register spb_size_t cnt;
        register uint8_t    oct;

        assume_d (dsc != NULL, SEXP_PRET_EUNDEF);
        assume_d (dsc->p_buffer != NULL, SEXP_PRET_EUNDEF);

        /* count in the starting quote if this is the first invocation */
        if (dsc->p_explen == 0) {
                itb = dsc->p_bufoff + 1;
                cnt = 1;
        } else {
                itb = dsc->p_bufoff + dsc->p_explen;
                cnt = 0;
        }

        spb_iterate (dsc->p_buffer, itb, oct,
                     if (oct == '\'')
                             goto found;
                     ++cnt;
                );

        dsc->p_explen += cnt;
        return (SEXP_PRET_EUNFIN);
found:
        dsc->p_explen += cnt + 1 /* count in the last quote */;

        assume_d (spb_size (dsc->p_buffer) >= dsc->p_explen, SEXP_PRET_EUNDEF);
        assume_r (spb_size (dsc->p_buffer) >= dsc->p_bufoff + dsc->p_explen, SEXP_PRET_EUNDEF);

        {/******************************************************************************************/
                SEXP_val_t v_dsc;

                if (SEXP_val_new (&v_dsc, sizeof (char) * (dsc->p_explen - 2 /* without quotes */),
                                  SEXP_VALTYPE_STRING) != 0)
                {
                        /*
                         * SEXP_val_new should always succeed. In case it doesn't
                         * it's an undefined error.
                         */
                        return (SEXP_PRET_EUNDEF);
                }

                assume_d (v_dsc.mem != NULL, NULL);

                if (spb_pick (dsc->p_buffer, dsc->p_bufoff + 1 /* skip the quote */,
                              dsc->p_explen - 2, v_dsc.mem) != 0)
                {
                        return (SEXP_PRET_EUNDEF);
                }

                /*
                 * Update the S-exp value pointer to the newly created value
                 */
                assume_d(dsc->s_exp != NULL, SEXP_PRET_EUNDEF);
                dsc->s_exp->s_valp = SEXP_val_ptr (&v_dsc);
        }/******************************************************************************************/

        return (SEXP_PRET_SUCCESS);
}

__PARSE_RT SEXP_parse_kl_string (__PARSE_PT(dsc))
{
        SEXP_val_t v_dsc;

        assume_d (dsc != NULL, SEXP_PRET_EUNDEF);
        assume_d (dsc->p_buffer != NULL, SEXP_PRET_EUNDEF);

        if (SEXP_val_new (&v_dsc, sizeof (char) * dsc->p_explen,
                          SEXP_VALTYPE_STRING) != 0)
        {
                return (SEXP_PRET_EINVAL);
        }

        if (spb_pick (dsc->p_buffer, dsc->p_bufoff,
                      dsc->p_explen, v_dsc.mem) != 0)
        {
                /* XXX: value leak */
                return (SEXP_PRET_EUNDEF);
        }

        dsc->s_exp->s_valp = SEXP_val_ptr (&v_dsc);

        return (SEXP_PRET_SUCCESS);
}

__PARSE_RT SEXP_parse_ul_string_b64 (__PARSE_PT(dsc))
{
        register uint8_t    oct;
        register spb_size_t cnt;
        spb_size_t itb;

        assume_d (dsc != NULL, SEXP_PRET_EUNDEF);
        assume_d (dsc->p_buffer != NULL, SEXP_PRET_EUNDEF);

        if (dsc->p_explen == 0) {
                itb = dsc->p_bufoff + 1;
                cnt = 1;
        } else {
                itb = dsc->p_bufoff + dsc->p_explen;
                cnt = 0;
        }

        spb_iterate (dsc->p_buffer, itb, oct,
                     if (oct == '|')
                             goto found;
                     if ((oct >= '0' && oct <= '9') ||
                         (oct >= 'A' && oct <= 'Z') ||
                         (oct >= 'a' && oct <= 'z') ||
                         (oct == '=' || oct == '+' || oct == '/'))
                             ++cnt;
                     else
                             return (SEXP_PRET_EINVAL);
                );

        dsc->p_explen += cnt;
        return (SEXP_PRET_EUNFIN);
found:
        dsc->p_explen += cnt + 1;

        {
                SEXP_val_t v_dsc;
                char    *b_enc, _b_enc[1024];
                bool     b_encfree;
                uint8_t *b_dec;
                size_t   b_declen;

                b_enc = (char *)spb_direct (dsc->p_buffer, dsc->p_bufoff + 1, dsc->p_explen - 2);

                if (b_enc == NULL) {
                        if (dsc->p_explen - 2 <= sizeof (_b_enc)) {
                                b_enc = _b_enc;
                                b_encfree = false;
                        } else {
                                b_enc = sm_alloc (sizeof (char) * (dsc->p_explen));
                                b_encfree = true;
                        }

			if (spb_pick (dsc->p_buffer, dsc->p_bufoff + 1, dsc->p_explen - 2, b_enc) != 0) {
				if (b_encfree)
					sm_free (b_enc);
                                return (SEXP_PRET_EUNDEF);
			}
                } else
                        b_encfree = false;

                b_dec    = NULL;
                b_declen = base64_decode (b_enc, dsc->p_explen - 2, &b_dec);

                if (b_declen == 0) {
                        if (b_encfree)
                                sm_free (b_enc);

                        if (b_dec != NULL)
                                sm_free (b_dec);

                        return (SEXP_PRET_EINVAL);
                }

                if (SEXP_val_new (&v_dsc, sizeof (char) * b_declen,
                                  SEXP_VALTYPE_STRING) != 0)
                {
                        sm_free (b_dec);

                        if (b_encfree)
                                sm_free (b_enc);

                        return (SEXP_PRET_EUNDEF);
                }

                memcpy (v_dsc.mem, b_dec, sizeof (char) * b_declen);
                sm_free (b_dec);

                if (b_encfree)
                        sm_free (b_enc);

                dsc->s_exp->s_valp = SEXP_val_ptr (&v_dsc);
        }

        return (SEXP_PRET_SUCCESS);
}

__PARSE_RT SEXP_parse_kl_string_b64 (__PARSE_PT(dsc))
{
        SEXP_val_t v_dsc;
        char    *b_enc, _b_enc[1024];
        bool     b_encfree;
        uint8_t *b_dec;
        size_t   b_declen;

        assume_d (dsc != NULL, SEXP_PRET_EUNDEF);
        assume_d (dsc->p_buffer != NULL, SEXP_PRET_EUNDEF);

        b_enc = (char *)spb_direct (dsc->p_buffer, dsc->p_bufoff + 1, dsc->p_explen);

        if (b_enc == NULL) {
                if (dsc->p_explen <= sizeof (_b_enc)) {
                        b_enc = _b_enc;
                        b_encfree = false;
                } else {
                        b_enc = sm_alloc (sizeof (char) * (dsc->p_explen));
                        b_encfree = true;
                }

		if (spb_pick (dsc->p_buffer, dsc->p_bufoff + 1, dsc->p_explen, b_enc) != 0) {
			if (b_encfree)
				sm_free (b_enc);
                        return (SEXP_PRET_EUNDEF);
		}
        } else
                b_encfree = false;

        b_dec    = NULL;
        b_declen = base64_decode (b_enc, dsc->p_explen, &b_dec);

        if (b_declen == 0) {
                if (b_encfree)
                        sm_free (b_enc);

                if (b_dec != NULL)
                        sm_free (b_dec);

                return (SEXP_PRET_EINVAL);
        }

        if (SEXP_val_new (&v_dsc, sizeof (char) * b_declen,
                          SEXP_VALTYPE_STRING) != 0)
        {
                sm_free (b_dec);

                if (b_encfree)
                        sm_free (b_enc);

                return (SEXP_PRET_EUNDEF);
        }

        memcpy (v_dsc.mem, b_dec, sizeof (char) * b_declen);
        sm_free (b_dec);

        if (b_encfree)
                sm_free (b_enc);

        dsc->s_exp->s_valp = SEXP_val_ptr (&v_dsc);

        ++dsc->p_explen;
        ++dsc->p_explen;

        return (SEXP_PRET_SUCCESS);
}

__PARSE_RT SEXP_parse_ul_datatype (__PARSE_PT(dsc))
{
        spb_size_t itb;
        register spb_size_t cnt;
        register uint8_t    oct;

        assume_d (dsc != NULL, SEXP_PRET_EUNDEF);
        assume_d (dsc->p_buffer != NULL, SEXP_PRET_EUNDEF);
        assume_d (SEXP_rawptr_mask(dsc->s_exp->s_type, SEXP_DATATYPEPTR_MASK) == NULL, SEXP_PRET_EUNDEF);

        /* count in the starting quote if this is the first invocation */
        if (dsc->p_explen == 0) {
                itb = dsc->p_bufoff + 1;
                cnt = 1;
        } else {
                itb = dsc->p_bufoff + dsc->p_explen;
                cnt = 0;
        }

        spb_iterate (dsc->p_buffer, itb, oct,
                     if (oct == ']')
                             goto found;
                     ++cnt;
                );

        dsc->p_explen += cnt;
        return (SEXP_PRET_EUNFIN);
found:
        dsc->p_explen += cnt + 1 /* count in the last bracket */;

        assume_d (spb_size (dsc->p_buffer) >= dsc->p_explen, SEXP_PRET_EUNDEF);
        assume_r (spb_size (dsc->p_buffer) >= dsc->p_bufoff + dsc->p_explen, SEXP_PRET_EUNDEF);

        {
                char *name, name_static[128];

                if ((dsc->p_explen - 2) < sizeof name_static)
                        name = name_static;
                else
                        name = sm_alloc (sizeof (char) * (dsc->p_explen - 1));

                if (spb_pick (dsc->p_buffer, dsc->p_bufoff + 1, dsc->p_explen - 2, name) != 0) {
                        if (name != name_static)
                                sm_free (name);

                        return (SEXP_PRET_EUNDEF);
                }

                name[dsc->p_explen - 2] = '\0';
                dsc->s_exp->s_type = SEXP_datatype_get (&g_datatypes, name);

                if (dsc->s_exp->s_type == NULL) {
                        if (name == name_static)
                                name = strdup(name);

                        dsc->s_exp->s_type = SEXP_datatype_add (&g_datatypes, name, NULL, NULL);

                        if (dsc->s_exp->s_type == NULL) {
                                sm_free(name);
                                return(SEXP_PRET_EUNDEF);
                        }
                } else {
                        if (name != name_static)
                                sm_free (name);
                }
        }

        return (SEXP_PRET_SUCCESS);
}

__PARSE_RT SEXP_parse_kl_datatype (__PARSE_PT(dsc))
{
        char *name, name_static[128];

        assume_d (dsc != NULL, SEXP_PRET_EUNDEF);
        assume_d (dsc->p_buffer != NULL, SEXP_PRET_EUNDEF);

        if (dsc->p_explen < sizeof name_static)
                name = name_static;
        else
                name = sm_alloc (sizeof (char) * (dsc->p_explen + 1));

        if (spb_pick (dsc->p_buffer, dsc->p_bufoff + 1, dsc->p_explen, name) != 0) {
                if (name != name_static)
                        sm_free (name);

                return (SEXP_PRET_EUNDEF);
        }

        name[dsc->p_explen] = '\0';
        dsc->s_exp->s_type = SEXP_datatype_get (&g_datatypes, name);

        if (dsc->s_exp->s_type == NULL) {
                if (name == name_static)
                        name = strdup(name);

                dsc->s_exp->s_type = SEXP_datatype_add (&g_datatypes, name, NULL, NULL);

                if (dsc->s_exp->s_type == NULL) {
                        sm_free(name);
                        return(SEXP_PRET_EUNDEF);
                }
        } else {
                if (name != name_static)
                        sm_free (name);
        }

        ++dsc->p_explen;
        ++dsc->p_explen;

        return (SEXP_PRET_SUCCESS);
}

__PARSE_RT SEXP_parse_bool (__PARSE_PT(dsc), bool val)
{
        SEXP_val_t v_dsc;

        assume_d ((true  & 1) == 1, SEXP_PRET_EUNDEF);
        assume_d ((false & 1) == 0, SEXP_PRET_EUNDEF);

        if (dsc->v_bool[val & 1] == 0) {
                if (SEXP_val_new (&v_dsc, sizeof (SEXP_numtype_t) + sizeof (bool),
                                  SEXP_VALTYPE_NUMBER) != 0)
                {
                        return (SEXP_PRET_EUNDEF);
                }

                SEXP_NCASTP(b,v_dsc.mem)->t = SEXP_NUM_BOOL;
                SEXP_NCASTP(b,v_dsc.mem)->n = val;
                dsc->v_bool[val & 1] = SEXP_val_ptr (&v_dsc);
        }

        assume_d (dsc->v_bool[val & 1] != 0, SEXP_PRET_EUNDEF);

        dsc->s_exp->s_valp = SEXP_rawval_incref (dsc->v_bool[val & 1]);
        dsc->p_explen = 1;

        return (SEXP_PRET_SUCCESS);
}

bool SEXP_pstate_errorp(SEXP_pstate_t *pstate)
{
	if (pstate == NULL)
		return (true);

	switch (pstate->p_error) {
	case SEXP_PRET_SUCCESS:
	case SEXP_PRET_EUNFIN:
		return (false);
	default:
		return (true);
	}
}
