#pragma once
#ifndef SEAP_TYPES_H
#define SEAP_TYPES_H

#include <stdint.h>
#include <seap-debug.h>
#include <seap-message.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SEAP_CTX SEAP_CTX_t;

#define SEAP_CTX_INITIALIZER { NULL, 0, 0, 0, SEAP_DESCTBL_INITIALIZER, SEAP_CMDTABLE_INITIALIZER }

typedef struct SEAP_cmd SEAP_cmd_t;

/* SEAP errors */
#define SEAP_ETYPE_INT  0 /* Internal error */
#define SEAP_ETYPE_USER 1 /* User-defined error */

#define SEAP_EUNFIN 1  /* Can't finish parsing */
#define SEAP_EPARSE 2  /* Parsing error */
#define SEAP_ECLOSE 3  /* Connection close */
#define SEAP_EINVAL 4  /* Invalid argument */
#define SEAP_ENOMEM 5  /* Cannot allocate memory */
#define SEAP_EMSEXP 6  /* Missing required S-exp/value */
#define SEAP_EMATTR 7  /* Missing required attribute */
#define SEAP_EUNEXP 8  /* Unexpected error */
#define SEAP_EUSER  9  /* User-defined error */
#define SEAP_ENOCMD 10 /* Unknown cmd */
#define SEAP_EQFULL 11 /* Queue full */
#define SEAP_EUNKNOWN 255 /* Unknown/Unexpected error */

/* SEAP I/O flags */
#define SEAP_IOFL_RECONN   0x00000001 /* Try to reconnect */
#define SEAP_IOFL_NONBLOCK 0x00000002 /* Non-blocking mode */

#ifdef __cplusplus
}
#endif

#endif /* SEAP_TYPES_H */
