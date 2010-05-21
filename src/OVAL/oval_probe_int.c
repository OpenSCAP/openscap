#include <errno.h>
#include <string.h>
#include "oval_sexp.h"
#include "oval_probe_impl.h"
#include "oval_definitions_impl.h"
#include "public/oval_system_characteristics.h"
#include "probes/public/probe-api.h"
#include "oval_probe_int.h"

static struct oval_syschar *oval_probe_envvar_eval(struct oval_object *obj, struct oval_syschar_model *model)
{
        struct oval_syschar *sys;
        struct oval_entity  *ent;
        struct oval_object_content_iterator *cit;
        struct oval_object_content *con;
        struct oval_value *val;

        char *var_name, *var_value, *ent_name;

        cit = oval_object_get_object_contents(obj);

	while (oval_object_content_iterator_has_more(cit)) {
                con = oval_object_content_iterator_next(cit);

                if (oval_object_content_get_type(con) != OVAL_OBJECTCONTENT_ENTITY)
                        continue;

                ent = oval_object_content_get_entity(con);
                ent_name = oval_entity_get_name(ent);

                if (strcmp(ent_name, "name") != 0)
                        continue;

                if (oval_entity_get_datatype(ent) != OVAL_DATATYPE_STRING)
                        continue;

                val = oval_entity_get_value(ent);
                var_name = oval_value_get_text(val);

                goto found;
        }

        return(NULL);
found:
        var_value = getenv(var_name);

        if (var_value == NULL)
                sys = oval_syschar_new(model, obj);
        else {
                SEXP_t *items, *r0, *r1, *cobj;

                items = SEXP_list_new(r0= probe_item_creat("environmentvariable_item", NULL,
                                                           "name", NULL, r1 = SEXP_string_new(var_value, strlen(var_value))),
                                      NULL);
                cobj = _probe_cobj_new(SYSCHAR_FLAG_UNKNOWN, items);
                sys = oval_sexp2sysch(cobj, model, obj);
                SEXP_vfree(r0, r1, items, cobj, NULL);
        }

        return(sys);
}

int oval_probe_envvar_handler(oval_subtype_t type, void *ptr, int act, ...)
{
        int ret = 0;
        va_list ap;
        struct oval_syschar_model *model = (struct oval_syschar_model *)ptr;

        va_start(ap, act);

        switch(act) {
        case PROBE_HANDLER_ACT_EVAL:
        {
                struct oval_object   *obj = va_arg(ap, struct oval_object *);
                struct oval_syschar **sys = va_arg(ap, struct oval_syschar **);
                int flags = va_arg(ap, int);

                *sys = oval_probe_envvar_eval(obj, model);
                ret  = (*sys == NULL ? -1 : 0);
                break;
        }
        case PROBE_HANDLER_ACT_INIT:
        case PROBE_HANDLER_ACT_OPEN:
        case PROBE_HANDLER_ACT_CLOSE:
        case PROBE_HANDLER_ACT_FREE:
        case PROBE_HANDLER_ACT_RESET:
                break;
        default:
                errno = EINVAL;
                ret = -1;
        }

        va_end(ap);
        return(ret);
}

int oval_probe_var_handler(oval_subtype_t type, void *ptr, int act, ...)
{
        int ret = 0;
        va_list ap;

        va_start(ap, act);

        switch(act) {
        case PROBE_HANDLER_ACT_EVAL:
        {
                struct oval_object   *obj = va_arg(ap, struct oval_object *);
                struct oval_syschar **sys = va_arg(ap, struct oval_syschar **);
                int flags = va_arg(ap, int);

                break;
        }
        case PROBE_HANDLER_ACT_INIT:
        case PROBE_HANDLER_ACT_OPEN:
        case PROBE_HANDLER_ACT_CLOSE:
        case PROBE_HANDLER_ACT_FREE:
        case PROBE_HANDLER_ACT_RESET:
                break;
        default:
                errno = EINVAL;
                ret = -1;
        }

        va_end(ap);
        return(ret);
}
