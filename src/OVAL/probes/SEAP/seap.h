#pragma once
#ifndef SEAP_H
#define SEAP_H

#include <string.h>
#include <sys/types.h>
#include <stdint.h>

#define _STR(x) #x
#define  STR(x) _STR(x)

#ifndef _D
# ifndef NDEBUG
#  define _D(...) do {                                          \
                fprintf (stderr, "%s: ", __PRETTY_FUNCTION__);  \
                fprintf (stderr, __VA_ARGS__);                  \
        } while(0)
# else
#  define _D(...)
# endif /* NDEBUG */
#endif /* _D */

#ifndef _A
# ifndef NDEBUG
#  include <assert.h>
#  define _A(x) assert(x)
# else
#  define _A(x)
# endif /* NDEBUG */
#endif /* _A */

/* Atom types */
typedef uint8_t ATOM_type_t;

#define ATOM_UNFIN   0x00
#define ATOM_LIST    0x01
#define ATOM_NUMBER  0x02
#define ATOM_SYMBOL  0x03
#define ATOM_STRING  0x04
#define ATOM_BINARY  0x05
#define ATOM_EMPTY   0xff

/* Number types */
typedef uint8_t NUM_type_t;

#define S_INT8   0x00
#define U_INT8   0x01
#define S_INT16  0x02
#define U_INT16  0x03
#define S_INT32  0x04
#define U_INT32  0x05
#define S_INT64  0x06
#define U_INT64  0x07
#define DOUBLE   0x08
#define FRACT    0x09
#define BIGNUM   0x0a

#define NUMTYPE_INV 0
#define NUMTYPE_INT 1
#define NUMTYPE_FLT 2
#define NUMTYPE_FRA 3
#define NUMTYPE_EXP 4

typedef struct {
        NUM_type_t type;
        //NUM_data_t num;
} NUM_t;

#define DEFNUMTYPE(type, st_type)               \
        typedef struct {                        \
                NUM_type_t type;                \
                st_type    number;              \
        } NUM_##type

DEFNUMTYPE(S_INT8_t,    int8_t);
DEFNUMTYPE(U_INT8_t,   uint8_t);
DEFNUMTYPE(S_INT16_t,  int16_t);
DEFNUMTYPE(U_INT16_t, uint16_t);
DEFNUMTYPE(S_INT32_t,  int32_t);
DEFNUMTYPE(U_INT32_t, uint32_t);
DEFNUMTYPE(S_INT64_t,  int64_t);
DEFNUMTYPE(U_INT64_t, uint64_t);
DEFNUMTYPE(DOUBLE_t,    double);
DEFNUMTYPE(FRACT_t,      NUM_t);
DEFNUMTYPE(BIGNUM_t,  intmax_t);

/*
typedef struct {
        char *str;
} SYM_t;
*/
typedef struct {
        size_t len;
        char  *str;
} STR_t;

typedef struct {
        size_t  len;
        void  *data;
} BIN_t;

typedef struct {
        void    *memb;
        uint32_t count;
        uint32_t size;
} LIST_t;

typedef struct {
        char *strid;
        /* TODO */
} SEXP_handler_t;

typedef struct __SEXP_t {
        ATOM_type_t    type;
        SEXP_handler_t *handler;
        int freeobj;
        union {
                LIST_t list;
                STR_t  string;
                NUM_t  number;
                BIN_t  binary;
        } atom;
} SEXP_t;

const char *SEAP_sexp_strtype (const SEXP_t *sexp);

#define SEXP(ptr) ((SEXP_t *)(ptr))

typedef uint8_t bool_t;
typedef uint8_t SEXP_pflags_t;

#define PF_EOFOK 0x01
#define PF_ALL   0xff

/* S-expression parsing state */
typedef struct {
        char    *buffer;
        size_t   buffer_data_len;
        size_t   buffer_fail_off;
        /*
          size_t   buffer_size;
          bool_t   buffer_free;
        */
        LIST_t **LIST_stack;
        uint16_t LIST_stack_cnt;
        uint16_t LIST_stack_size;

        SEXP_pflags_t pflags;
} SEXP_pstate_t;

/* S-expression parse function pointer */
//typedef SEXP_t * (*SEXP_parser_t) (SEAP_CTX_t *, const char *, size_t, SEXP_pstate_t **);

typedef uint8_t SEAP_scheme_t;

typedef struct {
        SEXP_t       *sexpbuf; /* S-exp buffer */
        /*uint16_t      sexpcnt; Number of S-exps in buffer */

        SEXP_pstate_t *pstate;  /* Parser state */
        SEAP_scheme_t  scheme;  /* Scheme used for this descriptor */
        void          *scheme_data; /* Scheme related data */
} SEAP_desc_t;

#define DESC_FDIN  0x00000001
#define DESC_FDOUT 0x00000002

typedef struct {
        const char *schstr;
        int (*sch_connect)  (SEAP_desc_t *, const char *, uint32_t);
        int (*sch_openfd)   (SEAP_desc_t *, int, uint32_t);
        int (*sch_openfd2)  (SEAP_desc_t *, int, int, uint32_t);
        ssize_t (*sch_recv) (SEAP_desc_t *, void *, size_t, uint32_t);
        ssize_t (*sch_send) (SEAP_desc_t *, void *, size_t, uint32_t);
        int (*sch_close)    (SEAP_desc_t *, uint32_t);
} SEAP_schemefn_t;

#include "bitmap.h"

typedef struct {
        SEAP_desc_t *sd;
        uint16_t     sdsize;
        bitmap_t     bitmap;
} SEAP_desctable_t;

#define SEAP_BUFFER_SIZE 4096
#define SEAP_MAX_OPENDESC 128
#define SDTABLE_REALLOC_ADD 4

int SEAP_desc_add (SEAP_desctable_t *sd_table, SEXP_pstate_t *pstate, SEAP_scheme_t scheme, void *scheme_data);
int SEAP_desc_del (SEAP_desctable_t *sd_table, int sd);
SEAP_desc_t *SEAP_desc_get (SEAP_desctable_t *sd_table, int sd);

/* S-expression format */
typedef uint8_t SEXP_format_t;

#define FMT_TRANSPORT  1
#define FMT_CANONICAL  2
#define FMT_ADVANCED   3
#define FMT_AUTODETECT 4

/* SEAP context */
typedef struct __SEAP_CTX_t {
        SEXP_t * (*parser) (struct __SEAP_CTX_t *, const char *, size_t, SEXP_pstate_t **);
        SEXP_pflags_t pflags;
        SEXP_format_t fmt_in;
        SEXP_format_t fmt_out;
        SEAP_desctable_t sd_table;
} SEAP_CTX_t;

SEAP_CTX_t *SEAP_CTX_new  (void);
void        SEAP_CTX_init (SEAP_CTX_t *ctx);
void        SEAP_CTX_free (SEAP_CTX_t *ctx);

SEXP_t     *SEAP_sexp_new  (void);
void        SEAP_sexp_init (SEXP_t  *sexp);
void        SEAP_sexp_free (SEXP_t **sexpp);

SEXP_t *SEXP_copy (SEXP_t *sexp);
SEXP_t *SEXP_list_first (SEXP_t *sexp);
SEXP_t *SEXP_list_last (SEXP_t *sexp);
int SEXP_listp (SEXP_t *sexp);
SEXP_t *SEXP_list_pop (SEXP_t **sexp);
int SEXP_strcmp (SEXP_t *sexp, const char *str);
int SEXP_strncmp (SEXP_t *sexp, const char *str, size_t n);

SEXP_t *SEAP_SEXP_parse (SEAP_CTX_t *ctx, const char *buf, size_t buflen, SEXP_pstate_t **pstate);

#define PARSER(name) SEXP_parser_##name
#define DEFPARSER(name) SEXP_t * PARSER(name) (SEAP_CTX_t *ctx, const char *buf, size_t buflen, SEXP_pstate_t **pstatep)

DEFPARSER(label);

/* DON'T TOUCH THIS */
#define LIST_GROW_LOW_TRESH 5
#define LIST_GROW_LOW 3
#define LIST_GROW 1.2
#define LIST_GROW_ADD 16
#define LIST_INIT_SIZE 1

#define LIST_STACK_GROW 1.2
#define LIST_STACK_INIT_SIZE 64

#include "xmalloc.h"

static inline LIST_t *LIST_init (LIST_t *list)
{
        _A(list != NULL);

        list->memb  = xmalloc (sizeof (SEXP_t) * LIST_INIT_SIZE);
        list->count = 0;
        list->size  = LIST_INIT_SIZE;
        
        return (list);
}

static inline LIST_t *LIST_new (void)
{
        LIST_t *list;

        list = xmalloc (sizeof (LIST_t));
        LIST_init (list);
        
        return (list);
}

SEXP_t *SEXP_READV_int  (SEXP_t *sexp, int   *ptr);
SEXP_t *SEXP_READP_int  (SEXP_t *sexp, int  **ptr);
SEXP_t *SEXP_READV_char (SEXP_t *sexp, char  *ptr);
SEXP_t *SEXP_READP_char (SEXP_t *sexp, char **ptr);

#if defined(SEAP_TYPES_FILE)
/* Create type definitions */
# define STRUCT(name) struct name
# define VAR(type, name) type   name
# define PTR(type, name) type * name
#  include SEAP_TYPES_FILE
# undef  STRUCT
# undef  VAR
# undef  PTR

/* Create type functions */ 
# define STRUCT(name)                                                   \
        static inline void SEXPtoTYPE_struct_##name (SEXP_t *sexp, struct name * ptr)

# define VAR(type, name) sexp = SEXP_READV_##type (sexp, &(ptr->name))
# define PTR(type, name) sexp = SEXP_READP_##type (sexp, &(ptr->name))

#  include SEAP_TYPES_FILE
# undef  STRUCT
# undef  VAR
# undef  PTR

#define struct2sexp(type, ptr, sexp) SEXPtoTYPE_struct_##type (sexp, ptr)
#define sexp2struct(type, sexp, ptr) TYPEtoSEXP_struct_##type (ptr, sexp)

#endif /* SEAP_TYPES_FILE */

typedef struct {
        char   *name;
        SEXP_t *value;
} SEAP_attr_t;

typedef struct {
        uint64_t     id;
        SEAP_attr_t *attrs;
        SEXP_t      *sexp;
} SEAP_msg_t;

int     SEAP_connect (SEAP_CTX_t *ctx, const char *uri, uint32_t flags);
int     SEAP_listen (SEAP_CTX_t *ctx, int sd, uint32_t maxcli);
//int     SEAP_accept (SEAP_t *ctx, int sd);
int     SEAP_open  (SEAP_CTX_t *ctx, const char *path, uint32_t flags);
SEXP_t *SEAP_read  (SEAP_CTX_t *ctx, int sd);
int     SEAP_write (SEAP_CTX_t *ctx, int sd, SEXP_t *sexp);
int     SEAP_close (SEAP_CTX_t *ctx, int sd);

int SEAP_openfd (SEAP_CTX_t *ctx, int fd, uint32_t flags);
int SEAP_openfd2 (SEAP_CTX_t *ctx, int ifd, int ofd, uint32_t flags);

#if 0
int SEAP_openfp (SEAP_CTX_t *ctx, FILE *fp, uint32_t flags);
#endif /* 0 */

SEAP_msg_t *SEAP_msg_new (void);
int SEAP_recvsexp (SEAP_CTX_t *ctx, int sd, SEXP_t **sexp);
int SEAP_recvmsg (SEAP_CTX_t *ctx, int sd, SEAP_msg_t **seap_msg);
int SEAP_sendsexp (SEAP_CTX_t *ctx, int sd, SEXP_t *sexp);
int SEAP_sendmsg (SEAP_CTX_t *ctx, int sd, SEAP_msg_t *seap_msg);

#endif /* SEAP_H */
