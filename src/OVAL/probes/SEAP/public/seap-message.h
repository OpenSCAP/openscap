#pragma once
#ifndef SEAP_MESSAGE_H
#define SEAP_MESSAGE_H

#include <stdint.h>
#include <sexp-types.h>

typedef uint64_t SEAP_msgid_t;
typedef struct SEAP_msg  SEAP_msg_t;
typedef struct SEAP_attr SEAP_attr_t;

SEAP_msg_t *SEAP_msg_new (void);
void        SEAP_msg_free (SEAP_msg_t *msg);

SEAP_msgid_t SEAP_msg_id (SEAP_msg_t *msg);

int     SEAP_msg_set (SEAP_msg_t *msg, SEXP_t *sexp);
SEXP_t *SEAP_msg_get (SEAP_msg_t *msg);

SEXP_t *SEAP_msgattr_get (SEAP_msg_t *msg, const char *name);
int     SEAP_msgattr_set (SEAP_msg_t *msg, const char *name, SEXP_t *value);
int     SEAP_msgattr_del (SEAP_msg_t *msg, const char *name);
int     SEAP_msgattr_exists (SEAP_msg_t *msg, const char *name);

#endif /* SEAP_MESSAGE_H */
