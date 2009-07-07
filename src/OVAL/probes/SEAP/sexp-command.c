#include <string.h>
#include "seap-types.h"
#include "sexp-types.h"
#include "sexp-manip.h"
#include "redblack.h"
#include "sexp-command.h"

#ifndef _A
#define _A(x) assert(x)
#endif

RBNODECMP(command)
{
        return (a->code - b->code);
}

RBNODEJOIN(command)
{
        (void)b;
        return (a);
}

RBNODECMP(waiting)
{
        return (a->id - b->id);
}

RBNODEJOIN(waiting)
{
        (void)b;
        return (a);
}

RBTREECODE(command);
RBTREECODE(waiting);

int SEAP_cmd_register (SEAP_CTX_t *ctx, uint32_t code, SEXP_t * (*cmdfn)(SEXP_t *, void *), void *arg)
{
        return (-1);
}

int SEAP_cmd_unregister (SEAP_CTX_t *ctx, uint32_t code)
{
        return (-1);
}

static SEXP_t *__cmd2sexp (SEAP_cmd_t *cmd)
{
        SEXP_t *sexp;
        
        _A(cmd != NULL);
        
        sexp = SEXP_list_new ();
        SEXP_list_add (sexp,
                       SEXP_string_new (SEAP_SYM_CMD, strlen (SEAP_SYM_CMD)));
        
        SEXP_list_add (sexp,
                       SEXP_string_new (":id", 3));
        SEXP_list_add (sexp,
                       SEXP_number_newhu (cmd->id));
        
        if (cmd->flags & SEAP_CMDFLAG_REPLY) {
                SEXP_list_add (sexp,
                               SEXP_string_new (":reply_id", 9));
                SEXP_list_add (sexp,
                               SEXP_number_newhu (cmd->rid));
        }
        
        SEXP_list_add (sexp,
                       SEXP_string_new (":class", 6));
        switch (cmd->class) {
        case SEAP_CMDCLASS_USR:
                SEXP_list_add (sexp,
                               SEXP_string_new ("usr", 4));
                break;
        case SEAP_CMDCLASS_INT:
                SEXP_list_add (sexp,
                               SEXP_string_new ("int", 8));
                break;
        default:
                abort ();
        }
        
        SEXP_list_add (sexp,
                       SEXP_string_new (":type", 5));
        
        SEXP_list_add (sexp,
                       (cmd->flags & SEAP_CMDFLAG_SYNC ?
                        SEXP_string_new ("sync", 4) : SEXP_string_new ("async", 5)));
        
        SEXP_list_add (sexp,
                       SEXP_number_newhu (cmd->code));
        
        if (cmd->args != NULL)
                SEXP_list_add (sexp, cmd->args);
        
        SEXP_VALIDATE(sexp);
        return (sexp);
}

SEXP_t *SEAP_cmd_exec (SEAP_CTX_t *ctx, int where, cmd_t cmd, SEXP_t *args,
                       cmd_type_t type, SEXP_t *(*func)(SEXP_t *, void *), void *funcarg)
{
        _A(ctx != NULL);
        
        if (where < 0) {
                /* local */
                
                /* lookup */
                /* exec */
                /* func */
                /* ret */
        } else {
                /* remote */
                
                switch (type) {
                case SEAP_CMDTYPE_SYNC:
                        /* construct message */
                        /* send */
                        /* recv */
                        /* func */
                        /* ret */
                        break;
                case SEAP_CMDTYPE_ASYNC:
                        /* construct message */
                        /* register func */
                        /* send */
                        /* ret */
                        break;
                default:
                        errno = EINVAL;
                        return (NULL);
                }
        }
}
