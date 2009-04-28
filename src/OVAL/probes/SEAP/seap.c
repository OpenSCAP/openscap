#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <config.h>
#include "xmalloc.h"
#include "common.h"
#include "sexp-handler.h"
#include "seap.h"
#include "xbase64.h"
#include "bitmap.h"

static void SEAP_CTX_initdefault (SEAP_CTX_t *ctx)
{
        _A(ctx != NULL);
        
        ctx->parser  = PARSER(label);
        ctx->pflags  = PF_EOFOK;
        ctx->fmt_in  = FMT_CANONICAL;
        ctx->fmt_out = FMT_CANONICAL;

        /* Initialize descriptor table */
        ctx->sd_table.sd = NULL;
        ctx->sd_table.sdsize = 0;
        bitmap_init (&(ctx->sd_table.bitmap), SEAP_MAX_OPENDESC);
        return;
}

SEAP_CTX_t *SEAP_CTX_new (void)
{
        SEAP_CTX_t *ctx;
        
        ctx = xmalloc (sizeof (SEAP_CTX_t));
        SEAP_CTX_initdefault (ctx);
        
        return (ctx);
}

void SEAP_CTX_init (SEAP_CTX_t *ctx)
{
        _A(ctx != NULL);
        SEAP_CTX_initdefault (ctx);
        return;
}

void SEAP_CTX_free (SEAP_CTX_t *ctx)
{
        _A(ctx != NULL);

        /* TODO: free sd_table */
        bitmap_free (&(ctx->sd_table.bitmap));
        xfree ((void **)&ctx);
        return;
}

SEXP_t *SEAP_sexp_new  (void)
{
        SEXP_t *sexp;

        sexp = xmalloc (sizeof (SEXP_t));
        sexp->flags = 0;
        SEXP_SETTYPE(sexp, ATOM_EMPTY);
        SEXP_SETFLAG(sexp, SEXP_FLAGFREE);
        sexp->handler = NULL;
        
        return (sexp);
}

void SEAP_sexp_init (SEXP_t  *sexp)
{
        _A(sexp != NULL);
        
        sexp->flags = 0;
        SEXP_SETTYPE(sexp, ATOM_EMPTY);
        sexp->handler = NULL;
        return;
}

void SEAP_sexp_free (SEXP_t **sexpp)
{
        _A(sexpp  != NULL);
        /* _A(*sexpp != NULL); */

#warning "FIXME: SEAP_sexp_free not implemented"
        
        if (*sexpp == NULL) {
                _D("WARNING: NULL free\n");
                return;
        }
        
        switch (SEXP_TYPE(*sexpp)) {
        case ATOM_LIST:
        case ATOM_NUMBER:
        case ATOM_SYMBOL:
        case ATOM_STRING:
        case ATOM_BINARY:
        case ATOM_UNFIN:
        case ATOM_EMPTY:
                break;
        default:
                abort ();
        }

        if (SEXP_FREE(*sexpp))
                xfree ((void **)(sexpp));
        
        return;
}

const char *__sexp_strtype[] = {
        "unfinished",
        "list",
        "number",
        "symbol",
        "string",
        "binary",
        "empty"
};

const char *SEAP_sexp_strtype (const SEXP_t *sexp)
{
        _A(SEXP_TYPE(sexp) >= 0);
        return (SEXP_TYPE(sexp) < 0x06 ? __sexp_strtype[SEXP_TYPE(sexp)] : "unknown");
}

SEXP_t *SEAP_parse_fd (SEAP_CTX_t *ctx, int fd, size_t max, SEXP_pstate_t **state)
{
        _A(ctx != NULL);
        _A(fd >= 0);
        
        return (NULL);
}

SEXP_t *SEAP_parse_buf (SEAP_CTX_t *ctx, void *buf, size_t len, SEXP_pstate_t **state)
{
        _A(ctx != NULL);
        _A(buf != NULL);
        _A(len  > 0);
        
        return (NULL);
}

SEXP_pstate_t *SEAP_pstate_new (void)
{
        SEXP_pstate_t *new;

        new = xmalloc (sizeof (SEXP_pstate_t));
        new->buffer = NULL;
        new->buffer_data_len = 0;
        /*
          new->buffer_size = 0;
          new->buffer_free = 0;
        */
        LIST_stack_init (&(new->lstack));
        LIST_stack_push (&(new->lstack), LIST_new ());
        
        return (new);
}

SEXP_pstate_t *SEAP_pstate_init (SEXP_pstate_t *state)
{
        _A(state != NULL);

        state->buffer = NULL;
        state->buffer_data_len = 0;
        /*
          state->buffer_size = 0;
          state->buffer_free = 0;
        */
        LIST_stack_init (&(state->lstack));
        LIST_stack_push (&(state->lstack), LIST_new ());

        return (state);
}

static SEXP_t *SEXP_list_new (void)
{
        SEXP_t *sexp;

        sexp = xmalloc (sizeof (SEXP_t));
        SEXP_SETTYPE(sexp, ATOM_LIST);
        SEXP_SETFLAG(sexp, SEXP_FLAGFREE);
        sexp->atom.list.memb  = xmalloc (sizeof (SEXP_t) * LIST_INIT_SIZE);
        sexp->atom.list.count = 0;
        sexp->atom.list.size  = LIST_INIT_SIZE;
        
        return (sexp);
}

static SEXP_t *SEXP_list_init (SEXP_t *sexp)
{
        _A(sexp != NULL);

        SEXP_SETTYPE(sexp, ATOM_LIST);
        LIST_init (&(sexp->atom.list));
        
        return (sexp);
}


SEXP_t *SEXP_list_first (SEXP_t *sexp)
{
        SEXP_t *empty;

        _A(sexp != NULL);
        if (SEXP_TYPE(sexp) == ATOM_LIST) {
                if (sexp->atom.list.count > 0) {
                        return (&(SEXP(sexp->atom.list.memb)[0]));
                } else {
                        empty = SEAP_sexp_new ();
                        SEXP_SETTYPE(sexp, ATOM_EMPTY);
                        return (empty);
                }
        } else {
                return (NULL);
        }
}

SEXP_t *SEXP_list_last (SEXP_t *sexp)
{
        SEXP_t *empty;
        
        _A(sexp != NULL);
        if (SEXP_TYPE(sexp) == ATOM_LIST) {
                if (sexp->atom.list.count > 0) {
                        return (&(SEXP(sexp->atom.list.memb)[sexp->atom.list.count - 1]));
                } else {
                        empty = SEAP_sexp_new ();
                        SEXP_SETTYPE(empty, ATOM_EMPTY); /* FIXME: this is redundant */
                        return (empty);
                }
        } else {
                return (NULL);
        }       
}

SEXP_t *SEXP_copy (SEXP_t *sexp)
{
        SEXP_t *copy;
        
        copy = SEAP_sexp_new ();
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
}

SEXP_t *SEXP_copyobj (SEXP_t *sexp)
{
        SEXP_t *copy;
        
        copy = SEAP_sexp_new ();
        copy->flags   = sexp->flags;
        copy->handler = sexp->handler;
        memcpy (&(copy->atom), &(sexp->atom), sizeof sexp->atom);
        
        return (copy);
}

int SEXP_listp (SEXP_t *sexp)
{
        return (sexp != NULL ? (SEXP_TYPE(sexp) == ATOM_LIST) : 0);
}

SEXP_t *SEXP_list_pop (SEXP_t **sexp)
{
        SEXP_t *sexp_ret;

        _A(sexp != NULL);
        _A(*sexp != NULL);
        
        if (SEXP_TYPE(*sexp) == ATOM_LIST) {
                if ((*sexp)->atom.list.count > 0) {
                        sexp_ret = SEXP_copyobj(&(SEXP((*sexp)->atom.list.memb)[0]));
                        
                        if ((*sexp)->atom.list.count > 1) {
                                --(*sexp)->atom.list.count;
                                
                                memmove (SEXP((*sexp)->atom.list.memb),
                                         SEXP((*sexp)->atom.list.memb) + 1,
                                         sizeof (SEXP_t) * ((*sexp)->atom.list.count));
                                
                                (*sexp)->atom.list.memb = xrealloc ((*sexp)->atom.list.memb,
                                                                    sizeof (SEXP_t) * ((*sexp)->atom.list.count));
                        } else {
                                xfree (&((*sexp)->atom.list.memb));
                                
                                if (SEXP_FREE(*sexp))
                                        xfree ((void **)sexp);
                                
                                *sexp = NULL;
                        }
                        return (sexp_ret);
                } else {
                        if (SEXP_FREE(*sexp))
                                xfree ((void **)sexp);
                        
                        *sexp = NULL;

                        return (NULL);
                }
        } else {
                return (NULL);
        }
}

int SEXP_strcmp (SEXP_t *sexp, const char *str)
{
        _A(sexp != NULL);
        _A(str != NULL);

        return (-1);
}

int SEXP_strncmp (SEXP_t *sexp, const char *str, size_t n)
{
        _A(sexp != NULL);
        _A(str != NULL);
        _A(n > 0);
        _A(SEXP_TYPE(sexp) == ATOM_STRING);
        
        return (n < sexp->atom.string.len ?
                strncmp (sexp->atom.string.str, str, n):
                strncmp (sexp->atom.string.str, str, sexp->atom.string.len));
}

static SEXP_t *LIST_add (LIST_t *list, SEXP_t *sexp, int freemem)
{
        _A(list != NULL);
        _A(list->count <= list->size);
        _A(list->size > 0);
        
        if (list->count >= list->size) {
                list->size += LIST_GROW_ADD;
                list->memb  = xrealloc (list->memb, sizeof (SEXP_t) * list->size);
        }
        
        memcpy (SEXP(list->memb) + list->count, sexp, sizeof (SEXP_t));

        ++(list->count);
        
        if (freemem != 0) {
                xfree ((void **)(&sexp));
        }
        
        return (SEXP(list->memb) + list->count - 1);
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

#define EXTRACTOR(name)    SEXP_extract_##name
#define DEFEXTRACTOR(what) static inline size_t EXTRACTOR(what) (void *out, const char *str, size_t len, uint8_t flags)

#define EXTRACTOR_F(name)    SEXP_extract_fixedlen_##name
#define DEFEXTRACTOR_F(what) static inline void EXTRACTOR_F(what) (void *out, const char *str, size_t len, size_t toklen, uint8_t flags)

DEFEXTRACTOR(si_string);
DEFEXTRACTOR(dq_string);
DEFEXTRACTOR(sq_string);
DEFEXTRACTOR_F(string);

//DEFEXTRACTOR(number);

DEFEXTRACTOR(b64_string);
DEFEXTRACTOR_F(b64_string);

DEFEXTRACTOR(datatype);
DEFEXTRACTOR_F(datatype);

DEFEXTRACTOR(hexstring);
DEFEXTRACTOR_F(hexstring);

DEFPARSER(label)
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
        
        size_t  i = 0, toklen = 0;
        SEXP_pflags_t exflags, exflags_tmp = 0;
        char    c;
        char   *pbuf;
        SEXP_t *sexp = NULL;
        
        /* Variables for L_NUMBER */
        uint8_t num_type = 0;
        uint8_t    valid = 0; /* This variable is probably useless... */
        uint32_t       d = 0;
        
#define PSTATE(pptr) (*(pptr))
        
        /* Check pstate */
#warning "Inefficient handling of pstate"

        if (PSTATE(pstatep) != NULL) {
                _D("Found pstate, merging buffers...\n");
                
                _D("old: \"%.*s\"\n",
                   PSTATE(pstatep)->buffer_data_len, PSTATE(pstatep)->buffer);
                _D("new: \"%.*s\"\n", buflen, buf);
                
                pbuf = PSTATE(pstatep)->buffer;
                pbuf = xrealloc (pbuf, sizeof (char) * (PSTATE(pstatep)->buffer_data_len + buflen));
                memcpy (pbuf + PSTATE(pstatep)->buffer_data_len, buf, buflen);

                /* Update buflen; buffer_data_len from pstate will be needed for extractors */
                buflen += PSTATE(pstatep)->buffer_data_len;

                PSTATE(pstatep)->buffer_data_len = 0;
                PSTATE(pstatep)->buffer = NULL;
        } else {
                PSTATE(pstatep) = SEAP_pstate_new ();
                pbuf = (char *)buf;
        }
        
        /* Initialize parse flags */
        exflags = PSTATE(pstatep)->lstack.LIST_stack_cnt > 1 ? PSTATE(pstatep)->pflags : ctx->pflags;
        
        /* Main parser loop */
        for (;;) {
                _A((PSTATE(pstatep)->lstack.LIST_stack_cnt  > 1 && !(exflags & EXF_EOFOK)) ||
                   (PSTATE(pstatep)->lstack.LIST_stack_cnt == 1 &&  (exflags & EXF_EOFOK)));

                sexp = SEAP_sexp_new ();
                
        L_NO_SEXP_ALLOC:
                if (i >= buflen) {
                        _D("EOF, i=%u, buflen=%u, LIST_stack_cnt=%u\n",
                           i, buflen, PSTATE(pstatep)->lstack.LIST_stack_cnt);
                        break;
                }

                c = pbuf[i];
                
                _D("LOOP: i=%zu, c=%c, sexp=%p, buflen=%zu\n",
                   i, c, sexp, buflen);
                
                goto *((c >= 0) ? labels[(uint8_t)c] : labels[128]);
        L_CHAR:
                i += EXTRACTOR(si_string)(sexp, pbuf + i, buflen - i, exflags);
                
                if (SEXP_TYPE(sexp) == ATOM_UNFIN) {
                        _D("Invalid si string\n");
                        break;
                }

                goto L_SEXP_ADD;
        L_CHAR_FIXEDLEN:
                EXTRACTOR_F(string)(sexp, pbuf + i, buflen - i, toklen, exflags);

                if (SEXP_TYPE(sexp) == ATOM_UNFIN) {
                        _D("Invalid si string\n");
                        break;
                }

                i += toklen;
                goto L_SEXP_ADD;
        L_DQUOTE:
                i += EXTRACTOR(dq_string)(sexp, pbuf + i, buflen - i, exflags);

                if (SEXP_TYPE(sexp) == ATOM_UNFIN) {
                        _D("Invalid dq string\n");
                        break;
                }

                goto L_SEXP_ADD;
        L_SQUOTE:
                i += EXTRACTOR(sq_string)(sexp, pbuf + i, buflen - i, exflags);

                if (SEXP_TYPE(sexp) == ATOM_UNFIN) {
                        _D("Invalid sq string\n");
                        break;
                }

                goto L_SEXP_ADD;
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
                
                
                ++d;
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
                        num_type = NUMTYPE_FLT;
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
                                                        SEXP_SETTYPE(sexp, ATOM_UNFIN);
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
                                SEXP_SETTYPE(sexp, ATOM_UNFIN);
                                goto invalid_number;
                        }
                        /* NOTREACHED */
                        abort ();
                        break;
                default:
                        if (isdigit(pbuf[i+d])) {
                                num_type = NUMTYPE_INT;
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
                        
                        SEXP_SETTYPE(sexp, ATOM_UNFIN);
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
                        num_type = NUMTYPE_EXP;
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
                if (num_type == NUMTYPE_INT && pbuf[i] != '-') {
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
                        case NUMTYPE_INT:
                                switch (*(pbuf + i)) {
                                case '-': /* signed */
                                {
                                        int64_t number;
                                        
                                        number = (int64_t) strtoll (pbuf + i, NULL, 10);
                                        
                                        if (number < INT16_MIN) {
                                                if (number < INT32_MIN) {
                                                        /* 64 */
                                                        NUM_STORE(int64_t, number, sexp->atom.number.nptr);
                                                        sexp->atom.number.type = NUM_INT64;
                                                } else {
                                                        /* 32 */
                                                        NUM_STORE(int32_t, number, sexp->atom.number.nptr);
                                                        sexp->atom.number.type = NUM_INT32;
                                                }
                                        } else {
                                                if (number < INT8_MIN) {
                                                        /* 16 */
                                                        NUM_STORE(int16_t, number, sexp->atom.number.nptr);
                                                        sexp->atom.number.type = NUM_INT16;
                                                } else {
                                                        /* 8 */
                                                        NUM_STORE(int8_t, number, sexp->atom.number.nptr);
                                                        sexp->atom.number.type = NUM_INT8;
                                                }
                                        }
                                } break;
                                default: /* unsigned */
                                {
                                        uint64_t number;
                                        
                                        number = (uint64_t) strtoull (pbuf + i, NULL, 10);
                                        
                                        if (number > UINT16_MAX) {
                                                if (number > UINT32_MAX) {
                                                        /* 64 */
                                                        NUM_STORE(uint64_t, number, sexp->atom.number.nptr);
                                                        sexp->atom.number.type = NUM_UINT64;
                                                } else {
                                                        /* 32 */
                                                        NUM_STORE(uint32_t, number, sexp->atom.number.nptr);
                                                        sexp->atom.number.type = NUM_UINT32;
                                                }
                                        } else {
                                                if (number > UINT8_MAX) {
                                                        /* 16 */
                                                        NUM_STORE(uint16_t, number, sexp->atom.number.nptr);
                                                        sexp->atom.number.type = NUM_UINT16;
                                                } else {
                                                        /* 8 */
                                                        NUM_STORE(uint8_t, number, sexp->atom.number.nptr);
                                                        sexp->atom.number.type = NUM_UINT8;
                                                }
                                        }
                                }}
                                break;
                        case NUMTYPE_FLT:
                        case NUMTYPE_EXP: /* TODO: store double/long double */
                        {
                                double number;
                                
                                number = strtod (pbuf + i, NULL);
                                NUM_STORE(double, number, sexp->atom.number.nptr);
                                sexp->atom.number.type = NUM_DOUBLE;
                        }
                        break;
                        case NUMTYPE_FRA:
                                _D("Fractions not supported yet\n");
                                abort ();
                                break;
                        default:
                                _D("Unknown number type\n");
                                abort ();
                        }
                        
                        SEXP_SETTYPE(sexp, ATOM_NUMBER);
                        /* STR -> SEXP END */
                        
                        i += d;
                        
                        num_type = 0;
                        valid = 0;
                        d = 0;
                } else {
                invalid_number:
                        SEXP_SETTYPE(sexp, ATOM_UNFIN);
                        num_type = 0;
                        valid = 0;
                        d = 0;
                        /* Invalid number format */
                        break;
                }
                /* If we got here, it's definitely just a number */
                goto L_SEXP_ADD;
        L_NUL:
                _D("WTF? NUL found.\n");
                break;
        L_WHITESPACE:
                while (isspace(pbuf[++i]));
                goto L_NO_SEXP_ALLOC;
        L_PAROPEN:
                {       LIST_t *subl;
                        
                        SEXP_list_init (sexp);
                        //subl = &(sexp->atom.list);
                        
                        /* NUL is not a valid token end inside a list */
                        if (PSTATE(pstatep)->lstack.LIST_stack_cnt == 1) {
                                exflags_tmp = exflags;
                                exflags &= ~(EXF_EOFOK);
                        }
                        
                        _A(!(exflags & EXF_EOFOK));
                        
                        sexp = LIST_add (LIST_stack_top(&(PSTATE(pstatep)->lstack)), sexp, 1);
                        LIST_stack_push (&(PSTATE(pstatep)->lstack), &(sexp->atom.list));
                }
                ++i;
                continue;
        L_PARCLOSE:
                ++i;
                if (PSTATE(pstatep)->lstack.LIST_stack_cnt > 1) {
                        LIST_stack_dec (&(PSTATE(pstatep)->lstack));

                        if (PSTATE(pstatep)->lstack.LIST_stack_cnt == 1) {
                                /*
                                 * We are outside a list, restore
                                 * original exflags
                                 */
                                exflags = exflags_tmp;
                        }
                        
                        goto L_NO_SEXP_ALLOC;
                } else {
                        _D("Syntax error: Unexpected close parenthesis\n");
                        return (NULL);
                }
                continue;
        L_BRACKETOPEN:
                i += EXTRACTOR(datatype)(sexp, pbuf + i, buflen - i, exflags);
                
                if (sexp->handler == NULL) {
                        _D("Invalid datatype\n");
                        SEXP_SETTYPE(sexp, ATOM_UNFIN);
                        break;
                }
                
                goto L_NO_SEXP_ALLOC;
        L_BRACKETOPEN_FIXEDLEN:
                EXTRACTOR_F(datatype)(sexp, pbuf + i, buflen - i, toklen, exflags);

                if (sexp->handler == NULL) {
                        _D("Invalid datatype\n");
                        SEXP_SETTYPE(sexp, ATOM_UNFIN);
                        break;
                }
                
                i += toklen + 2; /* +2 => [] */
                
                goto L_NO_SEXP_ALLOC;
        L_BRACEOPEN:
                //i += EXTRACTOR(b64_decode)(sexp, pbuf + i, buflen, exflags);
                continue;
        L_BRACEOPEN_FIXEDLEN:
                // EXTRACTOR_F(b64_decode)(sexp, pbuf + i, buflen, toklen, exflags);
                continue;
        L_VERTBAR:
                i += EXTRACTOR(b64_string)(sexp, pbuf + i, buflen - i, exflags);
                
                if (SEXP_TYPE(sexp) == ATOM_UNFIN) {
                        _D("Invalid b64 string\n");
                        break;
                }
                
                goto L_SEXP_ADD;
        L_VERTBAR_FIXEDLEN:
                EXTRACTOR_F(b64_string)(sexp, pbuf + i, buflen - i, toklen, exflags);

                if (SEXP_TYPE(sexp) == ATOM_UNFIN) {
                        _D("Invalid b64 string\n");
                        break;
                }
                
                i += toklen + 2; /* +2 => || */
                
                goto L_SEXP_ADD;
        L_HASH:
                i += EXTRACTOR(hexstring)(sexp, pbuf + i, buflen - i, exflags);
                
                if (SEXP_TYPE(sexp) == ATOM_UNFIN) {
                        _D("Invalid hex string\n");
                        break;
                }
                
                goto L_SEXP_ADD;
        L_HASH_FIXEDLEN:
                EXTRACTOR_F(hexstring)(sexp, pbuf + i, buflen - i, toklen, exflags);
                
                if (SEXP_TYPE(sexp) == ATOM_UNFIN) {
                        _D("Invalid hex string\n");
                        break;
                }
                
                i += toklen + 2; /* +2 => ## */
                
                goto L_SEXP_ADD;
        L_BRACECLOSE:
        L_BRACKETCLOSE:
        L_INVALID:
                _D("Syntax error: Invalid character: %02x\n", c);
                return (NULL);
        L_SEXP_ADD:
                /* Add new expression to list */
                LIST_add (LIST_stack_top(&(PSTATE(pstatep)->lstack)), sexp, 1);
#ifndef NDEBUG
                sexp = NULL;
#endif
        }
        
        if (PSTATE(pstatep)->lstack.LIST_stack_cnt == 1 && SEXP_TYPE(sexp) != ATOM_UNFIN) {
                _A(sexp != NULL);
                _A(SEXP_TYPE(sexp) != ATOM_UNFIN);
                _A(SEXP_TYPE(sexp) == ATOM_EMPTY);

                SEXP_SETTYPE (sexp, ATOM_LIST);
                memcpy (&(sexp->atom.list),
                        LIST_stack_bottom (&(PSTATE(pstatep)->lstack)), sizeof (LIST_t));
                
                //LIST_stack_top (PSTATE(pstatep)), sizeof (LIST_t));

                /*
                  PSTATE(pstatep)->buffer_data_len = 0;
                  PSTATE(pstatep)->buffer = NULL;
                  PSTATE(pstatep)->pflags = 0;
                */
                
                PSTATE(pstatep) = NULL;
                
                _D("ret: sexp@%p\n", sexp);
                return (sexp);
        } else {
                _A(SEXP_TYPE(sexp) == ATOM_UNFIN || SEXP_TYPE(sexp) == ATOM_EMPTY);
                /* save the unparsed part of buf */
                
                PSTATE(pstatep)->buffer_data_len = buflen - i;
                PSTATE(pstatep)->buffer = xmemdup (pbuf + i,
                                                   PSTATE(pstatep)->buffer_data_len);
                PSTATE(pstatep)->pflags = exflags;
                
                _D("pstate buf: \"%.*s\"\n",
                   PSTATE(pstatep)->buffer_data_len, PSTATE(pstatep)->buffer);
                
                SEAP_sexp_free (&sexp);

                _D("ret: NULL\n");
                return (NULL);
        }
}

DEFEXTRACTOR(si_string)
{
        register size_t l = 1;
        
        _A(out != NULL);
        _A(str != NULL);
        
        _D("Parsing si string\n");

        while (!isnexttok (str[l])) {
                if (l < len) {
                        ++l;
                } else {
                        if (flags & EXF_EOFOK)
                                break;
                        else {
                                SEXP_SETTYPE(SEXP(out), ATOM_UNFIN);
                                return (0);
                        }
                }
        }

#if !defined(NDEBUG)
        if (flags & EXF_EOFOK)
                _D("EOF is ok -> si string complete\n");
        else
                _D("next tok char: \"%c\"\n", str[l]);
#endif

        SEXP_SETTYPE(SEXP(out), ATOM_STRING);
        SEXP(out)->atom.string.len = l;
        SEXP(out)->atom.string.str = xmemdup (str, l);
        
        return (l);
}

DEFEXTRACTOR(dq_string)
{
        register size_t l = 1;

        _A(out != NULL);
        _A(str != NULL);
        _A(len > 0);

        _D("Parsing dq string\n");

        while (str[l] != '"') {
                if (l < len) {
                        if (str[l] == '\\') {
                                if (l+1 < len) {
                                        ++l;
                                } else {
                                        _D("EOF after slash\n");
                                        goto exit_unfin;
                                }
                        }
                } else {
                        _D("EOF before end of string\n");
                        goto exit_unfin;
                }
                
                ++l;
        }
        
        ++l;
        
        SEXP_SETTYPE(SEXP(out), ATOM_STRING);
        SEXP(out)->atom.string.len = l - 2; /* Don't count in beg/end quote */
        SEXP(out)->atom.string.str = xmemdup (str + 1, SEXP(out)->atom.string.len);
        return (l);
exit_unfin:
        SEXP_SETTYPE(SEXP(out), ATOM_UNFIN);
        return (0);
}

DEFEXTRACTOR(sq_string)
{
        register size_t l = 1;
        
        _A(out != NULL);
        _A(str != NULL);

        _D("Parsing sq string\n");
        
        while (str[l] != '\'') {
                if (l >= len) {
                        SEXP_SETTYPE(SEXP(out), ATOM_UNFIN);
                        return (0);
                }
                
                ++l;
        }
        
        ++l;
        
        SEXP_SETTYPE(SEXP(out), ATOM_STRING);
        SEXP(out)->atom.string.len = l - 2; /* Don't count in beg/end quote */
        SEXP(out)->atom.string.str = xmemdup (str + 1, SEXP(out)->atom.string.len);
        return (l);
}

DEFEXTRACTOR_F(string)
{
        _D("Parsing fixed length string\n");

        if (toklen <= len) {
                SEXP_SETTYPE(SEXP(out), ATOM_STRING);
                SEXP(out)->atom.string.len = toklen;
                SEXP(out)->atom.string.str = xmemdup (str, toklen);
        } else {
                _D("toklen > len\n");
                SEXP_SETTYPE(SEXP(out), ATOM_UNFIN);
        }
        /* Hmm, that was simple... */
        return;
}

DEFEXTRACTOR(b64_string)
{
        register uint32_t l = 1, slen;
        char *string;
        
        _A(out != NULL);
        _A(str != NULL);

        _D("Extracting b64 string\n");
        
#warning "Strict character checking not implemented"

        while (str[l] != '|') {
                if (isnexttok (str[l]) || l >= len) {
                        SEXP_SETTYPE(SEXP(out), ATOM_UNFIN);
                        return 0;
                }
                ++l;
        }
        
        if ((slen = base64_decode (str + 1, (size_t)l - 1, (uint8_t **)&string)) == 0) {
                _D("base64_decode failed\n");
                SEXP_SETTYPE(SEXP(out), ATOM_UNFIN);
                return 0;
        } else {
                _D("string = \"%.*s\"\n", slen, string);

                SEXP_SETTYPE(SEXP(out), ATOM_STRING);
                SEXP(out)->atom.string.len = slen;
                SEXP(out)->atom.string.str = string;
                return (l + 1);
        }
}

DEFEXTRACTOR_F(b64_string)
{
        char    *string;
        uint32_t slen;
        
        _D("Parsing fixed length b64 string, toklen=%u, len=%u\n", toklen, len);
        
        if (toklen <= len - 2) {
                _D("b64: len=%u, \"%.*s\"\n", toklen, toklen, str + 1);
                if ((slen = base64_decode (str + 1, toklen, (uint8_t **)&string)) == 0) {
                        _D("base64_decode failed\n");
                        SEXP_SETTYPE(SEXP(out), ATOM_UNFIN);
                } else {
                        SEXP_SETTYPE(SEXP(out), ATOM_STRING);
                        SEXP(out)->atom.string.len = slen;
                        SEXP(out)->atom.string.str = string;
                }
        } else {
                SEXP_SETTYPE(SEXP(out), ATOM_UNFIN);
        }
        
        return;
}

DEFEXTRACTOR(datatype)
{
        register uint32_t l = 1;

        _A(out != NULL);
        _A(str != NULL);
        _A(SEXP(out)->handler == NULL);
        
        _D("Parsing datatype\n");

        while (str[l] != ']') {
                if (l < len) {
                        if (str[l] == '\\') {
                                if (l+1 < len) {
                                        ++l;
                                } else {
                                        goto exit_unfin;
                                }
                        }
                } else {
                        goto exit_unfin;
                }
                
                ++l;
        }
        
        ++l;

        /* Try to find handler */
        SEXP(out)->handler = SEXP_gethandler_g(str + 1, l - 2);
        
        if (SEXP(out)->handler == NULL) {
                /* create a empty handler */
                SEXP_handler_t new;
                
                new.typestr = xmemdup (str + 1, l - 2);
                new.typelen = l - 2;
                new.fprint   = NULL;
                new.dprint   = NULL;
                new.dread    = NULL;
                new.fread    = NULL;
                new.mem2sexp = NULL;
                new.sexp2mem = NULL;
                
                SEXP(out)->handler = SEXP_reghandler_g(&new);
        }
        
        return (l);
exit_unfin:
        SEXP_SETTYPE(SEXP(out), ATOM_UNFIN);
        return (0);
}

DEFEXTRACTOR_F(datatype)
{
        _A(SEXP(out)->handler == NULL);
        
        _D("Parsing fixed length datatype\n");

        if (toklen <= len - 2) {
                /* Try to find handler */
                SEXP(out)->handler = SEXP_gethandler_g(str + 1, len - 2);
                
                if (SEXP(out)->handler == NULL) {
                        /* create a empty handler */
                        SEXP_handler_t new;
                        
                        new.typestr = xmemdup (str + 1, len - 2);
                        new.typelen = len - 2;
                        new.fprint   = NULL;
                        new.dprint   = NULL;
                        new.dread    = NULL;
                        new.fread    = NULL;
                        new.mem2sexp = NULL;
                        new.sexp2mem = NULL;
                        
                        SEXP(out)->handler = SEXP_reghandler_g(&new);
                }                
        } else {
                SEXP_SETTYPE(SEXP(out), ATOM_UNFIN);
        }

        return;
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
        register uint32_t l = 1, i;

        _D("Extracting hex string\n");
        
        while (str[l] != '#') {
                if (l < len) {
                        switch (str[l]) {
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
                                if (isdigit (str[l]))
                                        ++l;
                                else {
                                        /* Not a valid hex string character */
                                        SEXP_SETTYPE(SEXP(out), ATOM_UNFIN);
                                        return (0);
                                }
                        }
                } else {
                        /* EOF not allowed here */
                        SEXP_SETTYPE(SEXP(out), ATOM_UNFIN);
                        return (0);
                }
        }
        
        if (l > 1) {
                SEXP_SETTYPE(SEXP(out), ATOM_STRING);
                SEXP(out)->atom.string.len = ((l - 1) >> 1) + ((l - 1) & 1);
                SEXP(out)->atom.string.str = xmalloc (sizeof (char) * SEXP(out)->atom.string.len);
        
                for (++str, i = 0; i < ((l - 1) >> 1); ++i) {
                        SEXP(out)->atom.string.str[i] = (hex2bin[B(*str)] << 4) | hex2bin[B(*(str + 1))];
                        ++str;
                        ++str;
                }
                
                if ((l - 1) & 1)
                        SEXP(out)->atom.string.str[i] = hex2bin[B(*str)] << 4;
                
                _D("string = \"%.*s\"\n", SEXP(out)->atom.string.len, SEXP(out)->atom.string.str);
                
                return (l + 1);
        } else {
                /* Hexstring "##" is not allowed */
                SEXP_SETTYPE(SEXP(out), ATOM_UNFIN);
                return (0);
        }
}

DEFEXTRACTOR_F(hexstring)
{
        register uint32_t i;

        _D("Parsing fixed length hex string\n");

        if (toklen <= len - 2) {
                if (toklen > 0) {
                        SEXP_SETTYPE(SEXP(out), ATOM_STRING);
                        SEXP(out)->atom.string.len = (toklen >> 1) + (toklen & 1);
                        SEXP(out)->atom.string.str = xmalloc (sizeof (char) * SEXP(out)->atom.string.len);

                        for (++str, i = 0; i < (toklen >> 1); ++i) {
                                SEXP(out)->atom.string.str[i] = (hex2bin[B(*str)] << 4) | hex2bin[B(*(str + 1))];
                                ++str;
                                ++str;
                        }

                        if (toklen & 1)
                                SEXP(out)->atom.string.str[i] = hex2bin[B(*str)] << 4;

                        _D("slen=%u\n", SEXP(out)->atom.string.len);
                } else {
                        SEXP_SETTYPE(SEXP(out), ATOM_UNFIN);
                }
        } else {
                SEXP_SETTYPE(SEXP(out), ATOM_UNFIN);
        }
        
        return;
}
#undef B

#undef EXTRACTOR
#undef EXTRACTOR_F
#undef DEFEXTRACTOR
#undef DEFEXTRACTOR_F

/*
 * End parser: label
 */

SEXP_t *SEAP_SEXP_parse (SEAP_CTX_t *ctx, const char *buf, size_t buflen, SEXP_pstate_t **pstate)
{
        _A(ctx != NULL);
        _A(buf != NULL);
        _A(buflen > 0);
        _A(pstate != NULL);

        if (ctx->parser != NULL) {
                return ctx->parser (ctx, buf, buflen, pstate);
        } else {
                return (NULL);
        }
}

int SEAP_desc_add (SEAP_desctable_t *sd_table, SEXP_pstate_t *pstate,
                         SEAP_scheme_t scheme, void *scheme_data)
{
        bitmap_bitn_t sd;
        
        sd = bitmap_setfree (&(sd_table->bitmap));
        
        if (sd >= 0) {
                if (sd >= sd_table->sdsize) {
                        /* sd araay is to small -> realloc */
                        sd_table->sdsize = sd + SDTABLE_REALLOC_ADD;
                        sd_table->sd = xrealloc (sd_table->sd, sizeof (SEAP_desc_t) * sd_table->sdsize);
                }
                
                sd_table->sd[sd].sexpbuf = NULL;
                /* sd_table->sd[sd].sexpcnt = 0; */
                sd_table->sd[sd].pstate  = pstate;
                sd_table->sd[sd].scheme  = scheme;
                sd_table->sd[sd].scheme_data = scheme_data;
                sd_table->sd[sd].ostate  = NULL;

                return ((int)sd);
        }

        return (-1);
}

int SEAP_desc_del (SEAP_desctable_t *sd_table, int sd)
{
        
        return (0);
}

SEAP_desc_t *SEAP_desc_get (SEAP_desctable_t *sd_table, int sd)
{
        return (sd_table->sdsize > sd ? &(sd_table->sd[sd]) : NULL);
}

#include "scheme.h"

/* KEEP THIS ARRAY SORTED! */
const SEAP_schemefn_t __schtbl[] = {
#define SCH_CONS 0
        { "cons",
          sch_cons_connect, sch_cons_openfd,
          sch_cons_openfd2, sch_cons_recv,
          sch_cons_send, sch_cons_close, sch_cons_sendsexp },

#define SCH_DUMMY 1
        { "dummy",
          sch_dummy_connect, sch_dummy_openfd,
          sch_dummy_openfd2, sch_dummy_recv,
          sch_dummy_send, sch_dummy_close, sch_dummy_sendsexp },

#define SCH_GENERIC 2
        { "generic",
          sch_generic_connect, sch_generic_openfd,
          sch_generic_openfd2, sch_generic_recv,
          sch_generic_send, sch_generic_close, sch_generic_sendsexp },

#define SCH_PIPE 3
        { "pipe",
          sch_pipe_connect, sch_pipe_openfd,
          sch_pipe_openfd2, sch_pipe_recv,
          sch_pipe_send, sch_pipe_close, sch_pipe_sendsexp }

#define SCH_NONE 255
        
};

#define SCHTBLSIZE ((sizeof __schtbl)/sizeof (SEAP_schemefn_t))

SEAP_scheme_t SEAP_scheme_search (SEAP_schemefn_t fntable[SCHTBLSIZE], const char *sch, size_t schlen)
{
        SEAP_scheme_t w, s;
        int cmp;
        
        w = SCHTBLSIZE;
        s = 0;

        while (w > 0) {
                cmp = strncmp (sch, fntable[s + w/2].schstr, schlen);
                
                if (cmp > 0) {
                        s += w/2 + 1;
                        w  = w - w/2 - 1;
                } else if (cmp < 0) {
                        w = w/2;
                } else {
                        return (s + w/2);
                }
        }
        
        return (SCH_NONE);
}

#define SCH_CONNECT(idx, ...) __schtbl[idx].sch_connect (__VA_ARGS__)
#define SCH_OPENFD(idx, ...) __schtbl[idx].sch_openfd (__VA_ARGS__)
#define SCH_OPENFD2(idx, ...) __schtbl[idx].sch_openfd2 (__VA_ARGS__)
#define SCH_RECV(idx, ...) __schtbl[idx].sch_recv (__VA_ARGS__)
#define SCH_SEND(idx, ...) __schtbl[idx].sch_send (__VA_ARGS__)
#define SCH_CLOSE(idx, ...) __schtbl[idx].sch_close (__VA_ARGS__)
#define SCH_SENDSEXP(idx, ...) __schtbl[idx].sch_sendsexp (__VA_ARGS__)

int SEAP_connect (SEAP_CTX_t *ctx, const char *uri, uint32_t flags)
{
        SEAP_scheme_t scheme;
        size_t schstr_len = 0;
        int sd;

        while (uri[schstr_len] != ':') {
                if (uri[schstr_len] == '\0') {
                        errno = EINVAL;
                        return (-1);
                }
                ++schstr_len;
        }

        scheme = SEAP_scheme_search (__schtbl, uri, schstr_len);
        if (scheme == SCH_NONE) {
                /* scheme not found */
                errno = EPROTONOSUPPORT;
                return (-1);
        }

        if (uri[schstr_len + 1] == '/') {
                if (uri[schstr_len + 2] == '/') {
                        ++schstr_len;
                        ++schstr_len;
                } else {
                        errno = EINVAL;
                        return (-1);
                }
        } else {
                errno = EINVAL;
                return (-1);
        }
        
        sd = SEAP_desc_add (&(ctx->sd_table), NULL, scheme, NULL);
        if (sd < 0) {
                _D("Can't create/add new SEAP descriptor\n");
                return (-1);
        }

        if (SCH_CONNECT(scheme, &(ctx->sd_table.sd[sd]),
                        uri + schstr_len + 1, flags) != 0)
        {
                /* FIXME: delete SEAP descriptor */
                _D("SCH_CONNECT failed\n");
                return (-1);
        }
        
        return (sd);
}

int SEAP_open (SEAP_CTX_t *ctx, const char *path, uint32_t flags)
{
        errno = EOPNOTSUPP;
        return (-1);
}

int SEAP_openfd (SEAP_CTX_t *ctx, int fd, uint32_t flags)
{
        errno = EOPNOTSUPP;
        return (-1);
}

int SEAP_openfd2 (SEAP_CTX_t *ctx, int ifd, int ofd, uint32_t flags)
{
        int sd;

        sd = SEAP_desc_add (&(ctx->sd_table), NULL, SCH_GENERIC, NULL);
        if (sd < 0) {
                _D("Can't create/add new SEAP descriptor\n");
                return (-1);
        }

        if (SCH_OPENFD2(SCH_GENERIC, &(ctx->sd_table.sd[sd]),
                        ifd, ofd, flags) != 0)
        {
                _D("SCH_OPENFD2 failed\n");
                return (-1);
        }

        return (sd);
}

#if 0
int SEAP_openfp (SEAP_CTX_t *ctx, FILE *fp, uint32_t flags)
{
        errno = EOPNOTSUPP;
        return (-1);
}
#endif /* 0 */

int SEAP_recvsexp (SEAP_CTX_t *ctx, int sd, SEXP_t **sexp)
{
        SEAP_msg_t *msg = NULL;

        if (SEAP_recvmsg (ctx, sd, &msg) == 0) {
                /* TODO: free msg obj */
                *sexp = msg->sexp;
                return (0);
        } else {
                *sexp = NULL;
                return (-1);
        }
}

SEAP_msg_t *SEAP_msg_new (void)
{
        SEAP_msg_t *new;

        new = xmalloc (sizeof (SEAP_msg_t));
        new->id = 0;
        new->attrs = NULL;
        new->sexp = NULL;
        
        return (new);
}

int SEAP_recvmsg (SEAP_CTX_t *ctx, int sd, SEAP_msg_t **seap_msg)
{
        SEAP_desc_t *desc;
        char  *buffer;
        size_t buflen, recvlen;
        SEXP_t *sexp_msg;
        SEAP_msg_t *msg;
        
        if (sd >= 0 && sd < ctx->sd_table.sdsize) {
                desc = &(ctx->sd_table.sd[sd]);
                
                _A(desc->scheme < (sizeof __schtbl / sizeof (SEAP_schemefn_t)));
        
                /* message loop */
                for (;;) {
                        if (desc->sexpbuf == NULL) {
                                /* receive & parse loop */
                                for (;;) {
                                        buffer = xmalloc (sizeof (char) * SEAP_BUFFER_SIZE); /* TODO: make buffer size configurable */
                                        buflen = SEAP_BUFFER_SIZE;
                                        
                                        /* Receive raw data */
                                        if ((recvlen = SCH_RECV(desc->scheme, desc, buffer, buflen, 0)) <= 0) {
                                                /* check errno here (non-blocking mode?) */
                                                xfree ((void **)&buffer);
                                                return (-1);
                                        }
                                 
                                        _D("received data -> recvlen=%u\n", recvlen);
       
                                        /* Try to parse the buffer */
                                        desc->sexpbuf = SEAP_SEXP_parse (ctx, buffer, recvlen, &(desc->pstate));
                                        
                                        if (desc->sexpbuf != NULL) {
                                                sexp_msg = SEXP_list_pop (&(desc->sexpbuf));
                                                break;
                                        }
                                }
                        } else {
                                sexp_msg = SEXP_list_pop (&(desc->sexpbuf));
                        }
                        
                        /* check if the received S-exp is valid SEAP msg */
                        if (!SEXP_listp (sexp_msg)) {
                                _D("invalid msg received - not a list\n");
                                continue;
                        }
                        
                        if (SEXP_strncmp (SEXP_list_first (sexp_msg), "seap.msg", 8) != 0) {
                                _D("invalid msg received - first s-exp != \"msg\"\n");
                                continue;
                        }
                        
                        if (SEXP_listp (SEXP_list_last (sexp_msg))) {
                                msg = SEAP_msg_new ();
                                msg->sexp = SEXP_list_last (sexp_msg);
                                *seap_msg = msg;
                                
                                return (0);
                        } else {
                                /* invalid msg - last s-exp is not a list */
                                _D("invalid msg received - last s-exp is not a list\n");
                                continue;
                        }
                }
        } else {
                errno = EBADF;
                return (-1);
        }
}

int SEAP_sendsexp (SEAP_CTX_t *ctx, int sd, SEXP_t *sexp)
{
        /* construct message & send */
        return (0);
}

int SEAP_sendmsg (SEAP_CTX_t *ctx, int sd, SEAP_msg_t *seap_msg)
{
        SEAP_desc_t *desc;

        _A(ctx =! NULL);
        _A(seap_msg != NULL);

        if (sd >= 0 && sd < ctx->sd_table.sdsize) {
                desc = &(ctx->sd_table.sd[sd]);

                _A(desc->scheme < (sizeof __schtbl / sizeof (SEAP_schemefn_t)));
                
                
                
                return (0);
        } else {
                errno = EBADF;
                return (-1);
        }
}

SEXP_t *SEAP_read (SEAP_CTX_t *ctx, int sd)
{
        errno = EOPNOTSUPP;
        return (NULL);
}

int SEAP_write (SEAP_CTX_t *ctx, int sd, SEXP_t *sexp)
{
        errno = EOPNOTSUPP;
        return (-1);
}

int SEAP_close (SEAP_CTX_t *ctx, int sd)
{
        SEAP_desc_t *desc;
        int ret = 0;
        
        _A(ctx != NULL);
        
        if (sd > 0) {
                desc = &(ctx->sd_table.sd[sd]);
                _A(desc->scheme < (sizeof __schtbl / sizeof (SEAP_schemefn_t)));
                
                ret = SCH_CLOSE(desc->scheme, desc, 0); /* TODO: Are flags usable here? */
                
                if (SEAP_desc_del (&(ctx->sd_table), sd) != 0) {
                        /* something very bad happened */
                        _D("SEAP_desc_del failed\n");
                        if (ret > 0)
                                ret = -1;
                }
                
                return (ret);
        } else {
                _D("Negative SEAP descriptor\n");
                return (-1);
        }
}
