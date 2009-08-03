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

void SEAP_msg_free (SEAP_msg_t *msg)
{
        _A(msg != NULL);

        if (msg->attrs != NULL)
                sm_free (msg->attrs);
        
        sm_free (msg);
        return;
}

int SEAP_msg_set (SEAP_msg_t *msg, SEXP_t *sexp)
{
        msg->sexp = sexp;
        return (0);
}

SEXP_t *SEAP_msg_get (SEAP_msg_t *msg)
{
        return (msg->sexp);
}

int SEAP_msgattr_set (SEAP_msg_t *msg, const char *attr, SEXP_t *value)
{
        _A(msg != NULL);
        _A(attr != NULL);

        SEXP_VALIDATE(value);

        msg->attrs = sm_realloc (msg->attrs, sizeof (SEAP_attr_t) * (++msg->attrs_cnt));
        msg->attrs[msg->attrs_cnt - 1].name  = strdup (attr);
        msg->attrs[msg->attrs_cnt - 1].value = value;
        
        return (0);
}

SEXP_t *SEAP_msgattr_get (SEAP_msg_t *msg, const char *name)
{
        SEXP_t *value = NULL;
        uint16_t i;

        _A(msg  != NULL);
        _A(name != NULL);

        /* FIXME: this is stupid */
        for (i = 0; i < msg->attrs_cnt; ++i) {
                if (strcmp (name, msg->attrs[i].name) == 0)
                        return (msg->attrs[i].value);
        }
        
        return (NULL);
}
