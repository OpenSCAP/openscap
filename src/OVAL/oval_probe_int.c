/*
 * Copyright 2010 Red Hat Inc., Durham, North Carolina.
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
#include <errno.h>
#include <string.h>
#include "common/assume.h"
#include "oval_sexp.h"
#include "oval_probe_impl.h"
#include "oval_definitions_impl.h"
#include "public/oval_system_characteristics.h"
#include "probes/public/probe-api.h"
#include "oval_probe_int.h"

static struct oval_value *oval_object_getentval(struct oval_object *obj, const char *name)
{
        struct oval_value   *val;
        struct oval_entity  *ent;
        struct oval_object_content *con;
        struct oval_object_content_iterator *cit;
        char *ent_name;

        cit = oval_object_get_object_contents(obj);
        val = NULL;

	while (oval_object_content_iterator_has_more(cit)) {
                con = oval_object_content_iterator_next(cit);

                if (oval_object_content_get_type(con) != OVAL_OBJECTCONTENT_ENTITY)
                        continue;

                ent = oval_object_content_get_entity(con);
                ent_name = oval_entity_get_name(ent);

                if (strcmp(ent_name, name) != 0)
                        continue;

                val = oval_entity_get_value(ent);
                break;
        }

        return(val);
}

static struct oval_syschar *oval_probe_envvar_eval(struct oval_object *obj, struct oval_syschar_model *model)
{
        struct oval_syschar *sys;
        struct oval_value   *val;
        char *var_name, *var_value;

        val = oval_object_getentval(obj, "name");

        if (val == NULL)
                return(NULL);

        if (oval_value_get_datatype(val) != OVAL_DATATYPE_STRING)
                return(NULL);

        var_name  = oval_value_get_text(val);
        var_value = getenv(var_name);

        if (var_value == NULL)
                sys = oval_syschar_new(model, obj);
        else {
                SEXP_t *items, *r0, *r1, *cobj;

                items = SEXP_list_new(r0= probe_item_creat("environmentvariable_item", NULL,
                                                           "name", NULL, r1 = SEXP_string_new(var_value, strlen(var_value))),
                                      NULL);
                cobj = _probe_cobj_new(SYSCHAR_FLAG_UNKNOWN, items);
                sys  = oval_sexp2sysch(cobj, model, obj);
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

static struct oval_syschar *oval_probe_variable_eval(struct oval_object *obj, struct oval_syschar_model *sys_model)
{
        struct oval_value    *val;
        struct oval_syschar  *sys;
        struct oval_value_iterator *vit;
        struct oval_variable *var;
        struct oval_definition_model *def_model;
        char *var_ref;

        val = oval_object_getentval(obj, "var_ref");

        if (val == NULL)
                return(NULL);

        if (oval_value_get_datatype(val) != OVAL_DATATYPE_STRING)
                return(NULL);

        var_ref = oval_value_get_text(val);
        assume_d(var_ref != NULL, -1);

        def_model = oval_syschar_model_get_definition_model(sys_model);
        var = oval_definition_model_get_variable(def_model, var_ref);
        vit = oval_syschar_model_get_variable_values(sys_model, var);

        if (vit != NULL)
                sys = oval_syschar_new(sys_model, obj);
        else {
                SEXP_t *items, *r0, *r1, *item, *cobj, *vrent, *val_sexp;

                items = SEXP_list_new(NULL);

                /* Create shared entity */
                vrent = probe_ent_creat1("var_ref", NULL,
                                         r0 = SEXP_string_new(var_ref, strlen(var_ref)));
                SEXP_free(r0);

                while (oval_value_iterator_has_more(vit)) {
                        val = oval_value_iterator_next(vit);

                        if (oval_value_get_datatype(val) == OVAL_DATATYPE_STRING) {
                                val_sexp = oval_value_to_sexp(val, OVAL_DATATYPE_STRING);
                                assume_d(val_sexp != NULL, -1);

                                item = probe_item_creat("variable_item", NULL,
                                                        "value", NULL, val_sexp,
                                                        NULL);
                                /* Add shared entity */
                                SEXP_list_add(item, vrent);

                                /* Add item to the item list */
                                SEXP_list_add(items, item);
                                SEXP_vfree(item, val_sexp, NULL);
                        }
                }

                oval_value_iterator_free(vit);
                cobj = _probe_cobj_new(SYSCHAR_FLAG_UNKNOWN, items);
                sys  = oval_sexp2sysch(cobj, sys_model, obj);
                SEXP_vfree(cobj, items, vrent, NULL);
        }

        return(sys);
}

int oval_probe_var_handler(oval_subtype_t type, void *ptr, int act, ...)
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

                *sys = oval_probe_variable_eval(obj, model);
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
