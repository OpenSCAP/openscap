#pragma once
#ifndef SEAP_MESSAGE_H
#define SEAP_MESSAGE_H

#include <stdint.h>
#include <stdbool.h>
#include <sexp-types.h>

#if SEAP_MSGID_BITS == 64
typedef uint64_t SEAP_msgid_t;
#else
typedef uint32_t SEAP_msgid_t;
#endif

typedef struct SEAP_msg  SEAP_msg_t;
typedef struct SEAP_attr SEAP_attr_t;

SEAP_msg_t *SEAP_msg_new (void);
SEAP_msg_t *SEAP_msg_clone (SEAP_msg_t *msg);
void        SEAP_msg_free (SEAP_msg_t *msg);

SEAP_msgid_t SEAP_msg_id (SEAP_msg_t *msg);

int     SEAP_msg_set (SEAP_msg_t *msg, SEXP_t *sexp);
void    SEAP_msg_unset (SEAP_msg_t *msg);
SEXP_t *SEAP_msg_get (SEAP_msg_t *msg);

SEXP_t *SEAP_msgattr_get (SEAP_msg_t *msg, const char *name);
int     SEAP_msgattr_set (SEAP_msg_t *msg, const char *name, SEXP_t *value);
int     SEAP_msgattr_del (SEAP_msg_t *msg, const char *name);
bool    SEAP_msgattr_exists (SEAP_msg_t *msg, const char *name);

#endif /* SEAP_MESSAGE_H */
