#ifndef __STUB_PROBE
#include <string.h>
#include "_sexp-types.h"
#include "_seap-types.h"
#include "_seap-message.h"
#include "public/sm_alloc.h"

SEAP_msg_t *SEAP_msg_new (void)
{
        SEAP_msg_t *new;

        new = sm_talloc (SEAP_msg_t);
        new->id = 0;
        new->attrs = NULL;
        new->attrs_cnt = 0;
        new->sexp = NULL;
        
        return (new);
}

SEAP_msg_t *SEAP_msg_clone (SEAP_msg_t *msg)
{
        uint16_t i;
        SEAP_msg_t *new;

        new = sm_talloc (SEAP_msg_t);
        memcpy (new, msg, sizeof (SEAP_msg_t));
        
        new->attrs = sm_alloc (sizeof (SEAP_attr_t) * new->attrs_cnt);
        
        for (i = 0; i < new->attrs_cnt; ++i) {
                new->attrs[i].name  = strdup (msg->attrs[i].name);
                new->attrs[i].value = SEXP_ref (msg->attrs[i].value);
        }
        
        new->sexp  = SEXP_ref (msg->sexp);
        
        return (new);
}

void SEAP_msg_free (SEAP_msg_t *msg)
{
        if (msg == NULL)
                return;
        
        if (msg->attrs != NULL) {
                for (; msg->attrs_cnt > 0; --msg->attrs_cnt) {
                        _D("name=%s, value=%p\n",
                           msg->attrs[msg->attrs_cnt - 1].name,
                           msg->attrs[msg->attrs_cnt - 1].value);

                        sm_free (msg->attrs[msg->attrs_cnt - 1].name);
                        SEXP_free (msg->attrs[msg->attrs_cnt - 1].value);
                }
                
                sm_free (msg->attrs);
        }
        
        if (msg->sexp != NULL)
                SEXP_free (msg->sexp);
        
        sm_free (msg);
        return;
}

int SEAP_msg_set (SEAP_msg_t *msg, SEXP_t *sexp)
{
        msg->sexp = SEXP_ref (sexp);
        return (0);
}

void SEAP_msg_unset (SEAP_msg_t *msg)
{
        SEXP_free (msg->sexp);
        msg->sexp = NULL;
        return;
}

SEXP_t *SEAP_msg_get (SEAP_msg_t *msg)
{
        return SEXP_ref (msg->sexp);
}

SEAP_msgid_t SEAP_msg_id (SEAP_msg_t *msg)
{
        return (msg->id);
}

int SEAP_msgattr_set (SEAP_msg_t *msg, const char *attr, SEXP_t *value)
{
        _A(msg != NULL);
        _A(attr != NULL);

        if (value != NULL)
                SEXP_VALIDATE(value);
        
        msg->attrs = sm_realloc (msg->attrs, sizeof (SEAP_attr_t) * (++msg->attrs_cnt));
        msg->attrs[msg->attrs_cnt - 1].name  = strdup (attr);
        msg->attrs[msg->attrs_cnt - 1].value = (value != NULL ? SEXP_ref (value) : NULL);
        
        return (0);
}

bool SEAP_msgattr_exists (SEAP_msg_t *msg, const char *name)
{
        uint16_t i;

        _A(msg  != NULL);
        _A(name != NULL);
        _LOGCALL_;

        _D("cnt = %u\n", msg->attrs_cnt);

        /* FIXME: this is stupid */
        for (i = 0; i < msg->attrs_cnt; ++i) {
                _D("%s ?= %s\n", name, msg->attrs[i].name);
                if (strcmp (name, msg->attrs[i].name) == 0)
                        return (true);
        }
        
        return (false);
}

SEXP_t *SEAP_msgattr_get (SEAP_msg_t *msg, const char *name)
{
        uint16_t i;

        _A(msg  != NULL);
        _A(name != NULL);

        /* FIXME: this is stupid */
        for (i = 0; i < msg->attrs_cnt; ++i) {
                if (strcmp (name, msg->attrs[i].name) == 0)
                        return SEXP_ref (msg->attrs[i].value);
        }
        
        return (NULL);
}
#endif
