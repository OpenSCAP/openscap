#include <seap.h>
#include <assert.h>
#include <string.h>
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
        

        return (sexp);
}

const char *SEXP_OVALobj_getelm_val (SEXP_t *obj, const char *name)
{
        _A(obj != NULL);
        _A(name != NULL);

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

struct oval *sexp_to_oval_state (SEXP_t *sexp)
{

}
