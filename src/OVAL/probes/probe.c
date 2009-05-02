#include <seap.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <xmalloc.h>
#include "oval_definitions_impl.h"
#include "oval_collection_impl.h"
#include "oval_agent_api_impl.h"
#include "probe.h"

#ifndef _A
#define _A(x) assert(x)
#endif

/*
 * Examples of S-exp OVAL object (XML) representation:
 *
 * 1) object without attributes
 *    (rpminfo_state (name "foo") (arch "i386") ...)
 * 
 * 2) object with attributes
 *    ((rpminfo_state :attr1 "attrval") (name "foo") (arch "i386") ...)
 */

SEXP_t *SEXP_OVALobj_create (const char *obj_name, OVALobj_attr_t obj_attrs[], ...)
{
        SEXP_t *sexp = NULL, *sexp_tmp;
        uint16_t i;
        va_list ap;
        
        char *arg_name;
        char *arg_value;
        OVALobj_attr_t *arg_attrs;
        
        _A(obj_name != NULL);
        
        sexp = SEXP_list_new ();
        
        if (obj_attrs == NULL) {
                SEXP_list_add (sexp, SEXP_string_new (obj_name, strlen (obj_name)));
        } else {
                
                sexp_tmp = SEXP_list_new ();
                SEXP_list_add (sexp_tmp, SEXP_string_new (obj_name, strlen (obj_name)));

                /* Add object attributes */
                for (i = 0; obj_attrs[i].name != NULL; ++i) {
                        SEXP_list_add (sexp_tmp,
                                       SEXP_string_new (obj_attrs[i].name, strlen (obj_attrs[i].name)));
                        
                        /*
                         * Value isn't mandatory. The convention here
                         * is that if an attribute has a value, then the
                         * attribute name must begin with colon.
                         */
                        if (obj_attrs[i].value != NULL) {
                                SEXP_list_add (sexp_tmp,
                                               SEXP_string_new(obj_attrs[i].value, strlen (obj_attrs[i].value)));
                        }
                }
                
                SEXP_list_add (sexp, sexp_tmp);
        }
        
        /* Add object elements */
        va_start (ap, obj_attrs);
        
        while ((arg_name  = va_arg (ap, char *)) != NULL) {
                arg_attrs = va_arg (ap, OVALobj_attr_t *);
                arg_value = va_arg (ap, char *);
                
                sexp_tmp = SEXP_list_new ();
                
                if (arg_attrs == NULL) {
                        SEXP_list_add (sexp_tmp, SEXP_string_new (arg_name, strlen (arg_name)));
                } else {
                        /* add attrs */
                        SEXP_t *elm_list = NULL;
                }
                
                /* add value to the element list */
                SEXP_list_add (sexp_tmp, SEXP_string_new (arg_value, strlen (arg_value)));
                
                /* add the element to the object */
                SEXP_list_add (sexp, sexp_tmp);
        }
        
        va_end (ap);
        
        return (sexp);
}

char *SEXP_OVALobj_getelm_val (SEXP_t *obj, const char *name)
{
        uint32_t i;
        SEXP_t  *elm, *val, *elm_name;

        _A(obj != NULL);
        _A(name != NULL);

        i = 2;
        while ((elm = SEXP_list_nth (obj, i)) != NULL) {
                elm_name = SEXP_list_first (elm);
                
                if (!SEXP_stringp (elm_name)) {
                        if (SEXP_listp (elm_name)) {
                                elm_name = SEXP_list_first (elm_name);
                        } else {
                                continue;
                        }
                }
                
                if (SEXP_strcmp (elm_name, name) == 0) {
                        val = SEXP_list_nth (elm, 2);
                        
                        if (SEXP_stringp (val))
                                return SEXP_string_cstr (val);
                        else 
                                return (NULL);
                }
        }
        
        return (NULL);
}

SEXP_t *oval_object_to_sexp (const char *typestr, struct oval_object *object)
{
        SEXP_t *sexp, *elm, *elm_value;
        struct oval_iterator_object_content *cit;
        struct oval_object_content *content;
        struct oval_entity *entity;
        char *obj_name;
        uint16_t obj_namelen;

        /* (typestr_object (ent1 val1) (ent2 val2) ...) */
        sexp = SEXP_list_new ();
        cit  = oval_object_object_content (object);
        
        /* give the object a name */
        obj_namelen = strlen (typestr) + strlen ("_object");
        obj_name    = xmalloc (sizeof (char) * (obj_namelen + 1));
        
        snprintf (obj_name, obj_namelen + 1, "%s_object", typestr);
        SEXP_list_add (sexp, SEXP_string_new (obj_name, obj_namelen));
        
        while (oval_iterator_object_content_has_more (cit)) {
                content   = oval_iterator_object_content_next (cit);
                elm_value = NULL;
                
                switch (oval_object_content_type (content)) {
                case OVAL_OBJECTCONTENT_ENTITY:
                {
                        char *elm_name;
                        
                        /* ENTITY BEG */
                        entity = oval_object_content_entity (content);
                        
                        switch (oval_entity_datatype (entity)) {
                        case OVAL_DATATYPE_VERSION:
                        case OVAL_DATATYPE_STRING:
                        case OVAL_DATATYPE_EVR_STRING:
                        {
                                char *strval = oval_value_text (oval_entity_value (entity));
                                elm_value    = SEXP_string_new (strval, strlen (strval));
                        } break;
                        case OVAL_DATATYPE_FLOAT:
                                /* TODO */
                                continue;
                        case OVAL_DATATYPE_INTEGER:
                                /* TODO */
                                continue;
                        case OVAL_DATATYPE_BOOLEAN:
                                /* TODO */
                                continue;
                        default:
                                continue;
                        }
                        
                        elm = SEXP_list_new ();
                        elm_name = oval_entity_name (entity);
                        
                        /* TODO: handle element attributes */
                        SEXP_list_add (elm, SEXP_string_new (elm_name, strlen (elm_name)));
                        if (elm_value != NULL)
                                SEXP_list_add (elm, elm_value);
                        
                        /* ENTITY END */
                } break;
                default:
                        continue;
                }
                
                /* add the prepared element */
                SEXP_list_add (sexp, elm);
        }
        
        return (sexp);
}
