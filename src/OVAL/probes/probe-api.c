/**
 * @file
 * @author "Daniel Kopecek" <dkopecek@redhat.com>
 */

/*
 * Copyright 2009 Red Hat Inc., Durham, North Carolina.
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
 *      Daniel Kopecek <dkopecek@redhat.com>
 */

#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include "_probe-api.h"

/*
This file contains functions for manipulating with the S-exp representation of OVAL objects and items.
Currently object and items have the same structure and the API distinction is just formal. However, the
structure can diverge in the future and the API functions for manipulating with items should be used
only with items and vice versa. The most recent description of the object and item structure can be found
in this file and should be used as the main source of information for implementing new API functions.

Object structure
----------------

The basic structure of an object is

   (foo_object bar)

where foo_object is the object's name and bar is the value of this object. The value is optional.
In the case the object has some attributes the structure is

   ((foo_object :attr1 val1) bar)

where attr1 is the name of the attribute and val1 is the attribute's value. The colon in the
attribute name signals that this attribute has a value. 

*/

#if 0				/* experimental */
/*
 * Initialization
 */

int probe_api_init(void)
{
	/* initialize name cache */
	return (0);
}

int probe_api_deinit(void)
{
	/* destroy name cache */
	return (0);
}
#endif

/*
 * items
 */

SEXP_t *probe_item_build(const char *fmt, ...)
{
	_LOGCALL_;
	/* TBI */
	return (NULL);
}

/*
  SEXP_t *probe_item_creat (const char *name, SEXP_t *attrs, ...)
  {
  return probe_obj_creat (name, attrs, ...);
  }
*/

SEXP_t *probe_item_new(const char *name, SEXP_t * attrs)
{
	_LOGCALL_;
	/*
	 * Objects have the same structure as items.
	 */
	return probe_obj_new(name, attrs);
}

SEXP_t *probe_item_attr_add(SEXP_t * item, const char *name, SEXP_t * val)
{
	SEXP_t *n_ref, *ns;

	_LOGCALL_;

	n_ref = SEXP_listref_first(item);

	if (SEXP_listp(n_ref)) {
		/*
		 * There are already some attributes.
		 * Just add the new to the list.
		 */
		if (val == NULL)
			ns = SEXP_string_new(name, strlen(name));
		else
			ns = SEXP_string_newf(":%s", name);

		SEXP_list_add(n_ref, ns);
		SEXP_free(ns);

		if (val != NULL)
			SEXP_list_add(n_ref, val);
	} else {
		/*
		 * There aren't attributes in this item.
		 * We need to replace the item name S-exp
		 * with a list containing the item name
		 * S-exp and the attribute.
		 */
		SEXP_t *nl;

		if (val == NULL)
			ns = SEXP_string_new(name, strlen(name));
		else
			ns = SEXP_string_newf(":%s", name);

		nl = SEXP_list_new(n_ref, ns, val, NULL);

		SEXP_free(n_ref);
		SEXP_free(ns);

		n_ref = SEXP_list_replace(item, 1, nl);
		SEXP_free(nl);
	}

	SEXP_free(n_ref);

	return (val);
}

SEXP_t *probe_item_ent_add(SEXP_t *item, const char *name, SEXP_t *attrs, SEXP_t *val)
{
	SEXP_t *ent;

	_LOGCALL_;

	ent = probe_ent_creat1(name, attrs, val);
	SEXP_list_add(item, ent);
	SEXP_free(ent);

	return (item);
}

int probe_item_setstatus(SEXP_t *obj, oval_syschar_status_t status)
{
	_A(obj != NULL);
	_LOGCALL_;

	probe_item_attr_add(obj, "status", SEXP_number_newi_32((int) status));
	return (0);
}

int probe_itement_setstatus(SEXP_t *obj, const char *name, uint32_t n, oval_syschar_status_t status)
{
        SEXP_t *ent_h, *ent_s;
	_LOGCALL_;
	
        ent_h = probe_item_getent (obj, name, n);
        ent_s = SEXP_unref (ent_h);
        
        _A(ent_s != NULL);
        
        probe_ent_attr_add (ent_s, "status", SEXP_number_newi_32 ((int) status));
        SEXP_free (ent_s);
        
        return (0);
}

SEXP_t *probe_item_newid(struct id_desc_t * id_desc)
{
	int id_ctr;
	SEXP_t *sid;

#if defined(HAVE_ATOMIC_FUNCTIONS)
	id_ctr = __sync_fetch_and_add(&(id_desc->item_id_ctr), 1);
#else
	pthread_mutex_lock(&(id_desc->item_id_ctr_lock));
	id_ctr = id_desc->item_id_ctr++;
	pthread_mutex_unlock(&(id_desc->item_id_ctr_lock));
#endif
	sid = SEXP_string_newf("%d", id_ctr);

	return sid;
}

static void probe_item_resetidctr(struct id_desc_t *id_desc)
{
	id_desc->item_id_ctr = 1;
}

/*
 * attributes
 */

SEXP_t *probe_attr_creat(const char *name, const SEXP_t * val, ...)
{
	va_list ap;
	SEXP_t *list, *ns;

	_LOGCALL_;

	va_start(ap, val);
	list = SEXP_list_new(NULL);

	while (name != NULL) {
		if (val == NULL) {
			ns = SEXP_string_new(name, strlen(name));
			SEXP_list_add(list, ns);
			SEXP_free(ns);
		} else {
			ns = SEXP_string_newf(":%s", name);
			SEXP_list_add(list, ns);
			SEXP_list_add(list, val);
			SEXP_free(ns);
		}

		name = va_arg(ap, const char *);
		val = va_arg(ap, SEXP_t *);
	}

	va_end(ap);

	return (list);
}

/*
 * objects
 */

SEXP_t *probe_obj_build(const char *fmt, ...)
{
	_LOGCALL_;
	/* TBI */
	return (NULL);
}

SEXP_t *probe_obj_creat(const char *name, SEXP_t * attrs, ...)
{
	va_list ap;
	SEXP_t *obj, *ns, *val, *ent;

	_LOGCALL_;

	va_start(ap, attrs);

	obj = probe_obj_new(name, attrs);
	name = va_arg(ap, const char *);

	while (name != NULL) {
		attrs = va_arg(ap, SEXP_t *);
		val = va_arg(ap, SEXP_t *);

                ns  = encache_ref (OSCAP_GSYM(encache), name);
		ent = SEXP_list_new(NULL);
                
		if (attrs != NULL) {
			SEXP_t *nl, *nj;

			nl = SEXP_list_new(ns, NULL);
			nj = SEXP_list_join(nl, attrs);

			SEXP_list_add(ent, nj);
			SEXP_free(nl);
			SEXP_free(nj);
		} else
			SEXP_list_add(ent, ns);

		SEXP_free(ns);

		SEXP_list_add(ent, val);
		SEXP_list_add(obj, ent);

		SEXP_free(ent);

		name = va_arg(ap, const char *);
	}

	return (obj);
}

SEXP_t *probe_obj_new(const char *name, SEXP_t * attrs)
{
	SEXP_t *obj, *ns;

	_LOGCALL_;

	obj = SEXP_list_new(NULL);
	ns  = encache_ref (OSCAP_GSYM(encache), name);

	if (attrs != NULL) {
		SEXP_t *nl;

		nl = SEXP_list_new(ns, NULL);
		nl = SEXP_list_join(nl, attrs);

		SEXP_list_add(obj, nl);
		SEXP_free(nl);
	} else
		SEXP_list_add(obj, ns);

	SEXP_free(ns);

	return (obj);
}

SEXP_t *probe_obj_getent(const SEXP_t * obj, const char *name, uint32_t n)
{
	SEXP_t *objents, *ent, *ent_name;

	_A(obj != NULL);
	_A(name != NULL);
	_A(n > 0);
	_LOGCALL_;

	ent = NULL;
	objents = SEXP_list_rest(obj);

	SEXP_list_foreach(ent, objents) {
		ent_name = SEXP_list_first(ent);

		if (SEXP_listp(ent_name)) {
			SEXP_t *nr;

			nr = SEXP_list_first(ent_name);
			SEXP_free(ent_name);
			ent_name = nr;
		}

		if (SEXP_stringp(ent_name)) {
#if !defined(NDEBUG)
			char buf[128];
			SEXP_string_cstr_r(ent_name, buf, sizeof buf);
			_D("1=\"%s\", 2=\"%s\", n=%u\n", buf, name, n);
#endif

			if (SEXP_strcmp(ent_name, name) == 0 && (--n == 0)) {
				SEXP_free(ent_name);
				break;
			}
		}

		SEXP_free(ent_name);
	}

	SEXP_free(objents);

	return (ent);
}

SEXP_t *probe_obj_getentval(const SEXP_t * obj, const char *name, uint32_t n)
{
	SEXP_t *ent, *val;

	_LOGCALL_;

	ent = probe_obj_getent(obj, name, n);
	val = probe_ent_getval(ent);

	SEXP_free(ent);

	return (val);
}

int probe_obj_getentvals(const SEXP_t * obj, const char *name, uint32_t n, SEXP_t ** res)
{
	SEXP_t *ent;
	int ret;

	_LOGCALL_;

	ent = probe_obj_getent(obj, name, n);
	ret = probe_ent_getvals(ent, res);

	SEXP_free(ent);

	return (ret);
}

SEXP_t *probe_obj_getattrval(const SEXP_t * obj, const char *name)
{
	SEXP_t *obj_name;
	char name_buf[64 + 1];
	size_t name_len;

	_LOGCALL_;

	obj_name = SEXP_list_first(obj);
	name_len = snprintf(name_buf, sizeof name_buf, ":%s", name);

	_D("an=%s\n", name_buf);

	_A(name_len < sizeof name_buf);

	if (SEXP_listp(obj_name)) {
		uint32_t i;
		SEXP_t *attr;

		i = 2;

		while ((attr = SEXP_list_nth(obj_name, i)) != NULL) {
			if (SEXP_stringp(attr)) {
				if (SEXP_string_nth(attr, 1) == ':') {
					if (SEXP_strcmp(attr, name_buf) == 0) {
						SEXP_t *val;

						val = SEXP_list_nth(obj_name, i + 1);
						SEXP_free(attr);
						SEXP_free(obj_name);

						return (val);
					}

					++i;
				}

				++i;
			}

			SEXP_free(attr);
		}
	}

	SEXP_free(obj_name);

	return (NULL);
}

bool probe_obj_attrexists(const SEXP_t * obj, const char *name)
{
	SEXP_t *obj_name;
	char name_buf[64 + 1];
	size_t name_len;

	obj_name = SEXP_list_first(obj);
	name_len = snprintf(name_buf, sizeof name_buf, ":%s", name);

	_A(name_len < sizeof name_buf);

	if (SEXP_listp(obj_name)) {
		uint32_t i;
		SEXP_t *attr;

		i = 2;

		while ((attr = SEXP_list_nth(obj_name, i)) != NULL) {
			if (SEXP_stringp(attr)) {
				if (SEXP_string_nth(attr, 1) == ':') {
					if (SEXP_strcmp(attr, name_buf) == 0) {
						SEXP_free(attr);
						SEXP_free(obj_name);

						return (true);
					}
					++i;
				}
				++i;
			}

			SEXP_free(attr);
		}
	}

	SEXP_free(obj_name);

	return (false);
}

int probe_obj_setstatus(SEXP_t * obj, oval_syschar_status_t status)
{
	_LOGCALL_;

	probe_item_attr_add(obj, "status", SEXP_number_newi_32(status));
	return (-1);
}

static int probe_obj_setentstatus(SEXP_t * obj, const char *name, uint32_t n, oval_syschar_status_t status)
{
	_LOGCALL_;
	/* TBI */
	return (-1);
}

char *probe_obj_getname(const SEXP_t * obj)
{
	return probe_ent_getname(obj);
}

size_t probe_obj_getname_r(const SEXP_t * obj, char *buffer, size_t buflen)
{
	return probe_ent_getname_r(obj, buffer, buflen);
}

/*
 * collected objects
 */

SEXP_t *_probe_cobj_new(oval_syschar_collection_flag_t flag, const SEXP_t *item_list)
{
	SEXP_t *cobj, *item, *sflag;
	int error_cnt = 0;
	int exists_cnt = 0;
	int does_not_exist_cnt = 0;
	int not_collected_cnt = 0;

	if (flag == SYSCHAR_FLAG_UNKNOWN) {
		SEXP_list_foreach(item, item_list) {
			switch (probe_ent_getstatus(item)) {
			case SYSCHAR_STATUS_ERROR:
				++error_cnt;
				break;
			case SYSCHAR_STATUS_EXISTS:
				++exists_cnt;
				break;
			case SYSCHAR_STATUS_DOES_NOT_EXIST:
				++does_not_exist_cnt;
				break;
			case SYSCHAR_STATUS_NOT_COLLECTED:
				++not_collected_cnt;
				break;
			default:
				SEXP_free(item);
				return NULL;
			}
		}

		if (error_cnt > 0) {
			flag = SYSCHAR_FLAG_ERROR;
		} else if (not_collected_cnt > 0) {
			flag = SYSCHAR_FLAG_INCOMPLETE;
		} else if (exists_cnt > 0) {
			flag = SYSCHAR_FLAG_COMPLETE;
		} else {
			flag = SYSCHAR_FLAG_DOES_NOT_EXIST;
		}
	}

	sflag = SEXP_number_newi(flag);
	cobj = SEXP_list_new(sflag, item_list, NULL);
	SEXP_free(sflag);

	return cobj;
}

SEXP_t *_probe_cobj_get_items(const SEXP_t *cobj)
{
	return SEXP_list_nth(cobj, 2);
}

oval_syschar_collection_flag_t _probe_cobj_get_flag(const SEXP_t *cobj)
{
	SEXP_t *sflag;
	oval_syschar_collection_flag_t flag;

	sflag = SEXP_list_first(cobj);
	flag = SEXP_number_geti(sflag);
	SEXP_free(sflag);

	return flag;
}

oval_syschar_collection_flag_t _probe_cobj_combine_flags(oval_syschar_collection_flag_t f1,
							 oval_syschar_collection_flag_t f2,
							 oval_setobject_operation_t op)
{
	oval_syschar_collection_flag_t result = SYSCHAR_FLAG_ERROR;

	switch (op) {
	case OVAL_SET_OPERATION_COMPLEMENT:
		if (f1 == SYSCHAR_FLAG_ERROR) {
			result = SYSCHAR_FLAG_ERROR;
		} else if (f1 == SYSCHAR_FLAG_COMPLETE) {
			if (f2 == SYSCHAR_FLAG_NOT_APPLICABLE || f2 == SYSCHAR_FLAG_ERROR) {
				result = SYSCHAR_FLAG_ERROR;
			} else if (f2 == SYSCHAR_FLAG_COMPLETE || f2 == SYSCHAR_FLAG_DOES_NOT_EXIST) {
				result = SYSCHAR_FLAG_COMPLETE;
			} else if (f2 == SYSCHAR_FLAG_INCOMPLETE) {
				result = SYSCHAR_FLAG_ERROR;
			} else if (f2 == SYSCHAR_FLAG_NOT_COLLECTED) {
				result = SYSCHAR_FLAG_ERROR;
			}
		} else if (f1 == SYSCHAR_FLAG_INCOMPLETE) {
			if (f2 == SYSCHAR_FLAG_NOT_APPLICABLE || f2 == SYSCHAR_FLAG_ERROR) {
				result = SYSCHAR_FLAG_ERROR;
			} else if (f2 == SYSCHAR_FLAG_DOES_NOT_EXIST) {
				result = SYSCHAR_FLAG_INCOMPLETE;
			} else if (f2 == SYSCHAR_FLAG_NOT_COLLECTED) {
				result = SYSCHAR_FLAG_ERROR;
			} else if (f2 == SYSCHAR_FLAG_INCOMPLETE) {
				result = SYSCHAR_FLAG_ERROR;
			} else if (f2 == SYSCHAR_FLAG_COMPLETE) {
				result = SYSCHAR_FLAG_INCOMPLETE;
			}
		} else if (f1 == SYSCHAR_FLAG_DOES_NOT_EXIST) {
			if (f2 != SYSCHAR_FLAG_NOT_APPLICABLE) {
				result = SYSCHAR_FLAG_DOES_NOT_EXIST;
			} else {
				result = SYSCHAR_FLAG_ERROR;
			}
		} else if (f1 == SYSCHAR_FLAG_NOT_COLLECTED) {
			if (f2 != SYSCHAR_FLAG_NOT_APPLICABLE && f2 != SYSCHAR_FLAG_ERROR) {
				result = SYSCHAR_FLAG_NOT_COLLECTED;
			} else {
				result = SYSCHAR_FLAG_ERROR;
			}
		} else if (f1 == SYSCHAR_FLAG_NOT_APPLICABLE) {
			result = SYSCHAR_FLAG_ERROR;
		}
		break;

	case OVAL_SET_OPERATION_INTERSECTION:
		if (f1 == SYSCHAR_FLAG_ERROR) {
			result = SYSCHAR_FLAG_ERROR;
		} else if (f1 == SYSCHAR_FLAG_COMPLETE) {
			if (f2 == SYSCHAR_FLAG_ERROR) {
				result = SYSCHAR_FLAG_ERROR;
			} else if (f2 == SYSCHAR_FLAG_DOES_NOT_EXIST) {
				result = SYSCHAR_FLAG_DOES_NOT_EXIST;
			} else if (f2 == SYSCHAR_FLAG_COMPLETE || f2 == SYSCHAR_FLAG_NOT_APPLICABLE) {
				result = SYSCHAR_FLAG_COMPLETE;
			} else if (f2 == SYSCHAR_FLAG_INCOMPLETE) {
				result = SYSCHAR_FLAG_INCOMPLETE;
			} else if (f2 == SYSCHAR_FLAG_NOT_COLLECTED) {
				result = SYSCHAR_FLAG_NOT_COLLECTED;
			}
		} else if (f1 == SYSCHAR_FLAG_INCOMPLETE) {
			if (f2 == SYSCHAR_FLAG_ERROR) {
				result = SYSCHAR_FLAG_ERROR;
			} else if (f2 == SYSCHAR_FLAG_DOES_NOT_EXIST) {
				result = SYSCHAR_FLAG_DOES_NOT_EXIST;
			} else if (f2 == SYSCHAR_FLAG_NOT_APPLICABLE) {
				result = SYSCHAR_FLAG_NOT_APPLICABLE;
			} else if (f2 == SYSCHAR_FLAG_NOT_COLLECTED || f2 == SYSCHAR_FLAG_COMPLETE || f2 == SYSCHAR_FLAG_INCOMPLETE) {
				result = SYSCHAR_FLAG_INCOMPLETE;
			}
		} else if (f1 == SYSCHAR_FLAG_DOES_NOT_EXIST) {
			result = SYSCHAR_FLAG_DOES_NOT_EXIST;
		} else if (f1 == SYSCHAR_FLAG_NOT_COLLECTED) {
			if (f2 == SYSCHAR_FLAG_ERROR) {
				result = SYSCHAR_FLAG_ERROR;
			} else if (f2 == SYSCHAR_FLAG_DOES_NOT_EXIST) {
				result = SYSCHAR_FLAG_DOES_NOT_EXIST;
			} else if (f2 == SYSCHAR_FLAG_COMPLETE || f2 == SYSCHAR_FLAG_INCOMPLETE || f2 == SYSCHAR_FLAG_NOT_APPLICABLE || f2 == SYSCHAR_FLAG_NOT_COLLECTED) {
				result = SYSCHAR_FLAG_NOT_COLLECTED;
			}
		} else if (f1 == SYSCHAR_FLAG_NOT_APPLICABLE) {
			if (f2 == SYSCHAR_FLAG_ERROR) {
				result = SYSCHAR_FLAG_ERROR;
			} else if (f2 == SYSCHAR_FLAG_DOES_NOT_EXIST) {
				result = SYSCHAR_FLAG_DOES_NOT_EXIST;
			} else if (f2 == SYSCHAR_FLAG_COMPLETE) {
				result = SYSCHAR_FLAG_COMPLETE;
			} else if (f2 == SYSCHAR_FLAG_INCOMPLETE) {
				result = SYSCHAR_FLAG_INCOMPLETE;
			} else if (f2 == SYSCHAR_FLAG_NOT_APPLICABLE) {
				result = SYSCHAR_FLAG_NOT_APPLICABLE;
			} else if (f2 == SYSCHAR_FLAG_NOT_COLLECTED) {
				result = SYSCHAR_FLAG_NOT_COLLECTED;
			}
		}
		break;

	case OVAL_SET_OPERATION_UNION:
		if (f1 == SYSCHAR_FLAG_ERROR) {
			result = SYSCHAR_FLAG_ERROR;
		} else if (f1 == SYSCHAR_FLAG_COMPLETE) {
			if (f2 == SYSCHAR_FLAG_ERROR) {
				result = SYSCHAR_FLAG_ERROR;
			} else if (f2 == SYSCHAR_FLAG_COMPLETE || f2 == SYSCHAR_FLAG_DOES_NOT_EXIST || f2 == SYSCHAR_FLAG_NOT_APPLICABLE) {
				result = SYSCHAR_FLAG_COMPLETE;
			} else if (f2 == SYSCHAR_FLAG_INCOMPLETE || f2 == SYSCHAR_FLAG_NOT_COLLECTED) {
				result = SYSCHAR_FLAG_INCOMPLETE;
			}
		} else if (f1 == SYSCHAR_FLAG_INCOMPLETE) {
			if (f2 != SYSCHAR_FLAG_ERROR) {
				result = SYSCHAR_FLAG_INCOMPLETE;
			} else {
				result = SYSCHAR_FLAG_ERROR;
			}
		} else if (f1 == SYSCHAR_FLAG_DOES_NOT_EXIST) {
			if (f2 == SYSCHAR_FLAG_ERROR) {
				result = SYSCHAR_FLAG_ERROR;
			} else if (f2 == SYSCHAR_FLAG_COMPLETE) {
				result = SYSCHAR_FLAG_COMPLETE;
			} else if (f2 == SYSCHAR_FLAG_INCOMPLETE || f2 == SYSCHAR_FLAG_NOT_COLLECTED) {
				result = SYSCHAR_FLAG_INCOMPLETE;
			} else if (f2 == SYSCHAR_FLAG_NOT_APPLICABLE || f2 == SYSCHAR_FLAG_DOES_NOT_EXIST) {
				result = SYSCHAR_FLAG_DOES_NOT_EXIST;
			}
		} else if (f1 == SYSCHAR_FLAG_NOT_COLLECTED) {
			if (f2 == SYSCHAR_FLAG_ERROR) {
				result = SYSCHAR_FLAG_ERROR;
			} else if (f2 == SYSCHAR_FLAG_COMPLETE || f2 == SYSCHAR_FLAG_INCOMPLETE || f2 == SYSCHAR_FLAG_DOES_NOT_EXIST) {
				result = SYSCHAR_FLAG_INCOMPLETE;
			} else if (f2 == SYSCHAR_FLAG_NOT_APPLICABLE || f2 == SYSCHAR_FLAG_NOT_COLLECTED) {
				result = SYSCHAR_FLAG_NOT_COLLECTED;
			}
		} else if (f1 == SYSCHAR_FLAG_NOT_APPLICABLE) {
			if (f2 == SYSCHAR_FLAG_ERROR) {
				result = SYSCHAR_FLAG_ERROR;
			} else if (f2 == SYSCHAR_FLAG_COMPLETE) {
				result = SYSCHAR_FLAG_COMPLETE;
			} else if (f2 == SYSCHAR_FLAG_INCOMPLETE) {
				result = SYSCHAR_FLAG_INCOMPLETE;
			} else if (f2 == SYSCHAR_FLAG_NOT_APPLICABLE) {
				result = SYSCHAR_FLAG_NOT_APPLICABLE;
			} else if (f2 == SYSCHAR_FLAG_DOES_NOT_EXIST) {
				result = SYSCHAR_FLAG_DOES_NOT_EXIST;
			} else if (f2 == SYSCHAR_FLAG_NOT_COLLECTED) {
				result = SYSCHAR_FLAG_NOT_COLLECTED;
			}
		}
		break;

	default:
		result = SYSCHAR_FLAG_ERROR;
	}

	return result;
}

/*
 * entities
 */

SEXP_t *probe_ent_creat(const char *name, SEXP_t * attrs, SEXP_t * val, ...)
{
	va_list ap;
	SEXP_t *ent_list, *ent;

	_LOGCALL_;

	va_start(ap, val);
	ent_list = SEXP_list_new(NULL);

	while (name != NULL) {
		ent = probe_ent_creat1(name, attrs, val);

		SEXP_list_add(ent_list, ent);
		SEXP_free(ent);

		name = va_arg(ap, const char *);
		attrs = va_arg(ap, SEXP_t *);
		val = va_arg(ap, SEXP_t *);
	}

	return (ent_list);
}

SEXP_t *probe_ent_creat1(const char *name, SEXP_t * attrs, SEXP_t * val)
{
	SEXP_t *ent, *ns;

	_LOGCALL_;

	ent = SEXP_list_new(NULL);
	ns  = encache_ref (OSCAP_GSYM(encache), name);

	if (attrs != NULL) {
		SEXP_t *nl, *nj;

		nl = SEXP_list_new(ns, NULL);
		nj = SEXP_list_join(nl, attrs);

		SEXP_list_add(ent, nj);

		SEXP_free(nl);
		SEXP_free(nj);
	} else
		SEXP_list_add(ent, ns);

	SEXP_free(ns);

	if (val != NULL)
		SEXP_list_add(ent, val);

	return (ent);
}

SEXP_t *probe_ent_attr_add(SEXP_t * ent, const char *name, SEXP_t * val)
{
	_LOGCALL_;
	return (probe_item_attr_add(ent, name, val));
}

int probe_ent_getvals(const SEXP_t * ent, SEXP_t ** res)
{
	_LOGCALL_;
	(*res) = SEXP_list_rest(ent);
	return (SEXP_list_length(*res));
}

SEXP_t *probe_ent_getval(const SEXP_t * ent)
{
	_LOGCALL_;
	if (probe_ent_attrexists(ent, "var_ref")) {
		SEXP_t *r0, *r1;
		unsigned int val_idx;

		r0 = probe_ent_getattrval(ent, "val_idx");
		val_idx = SEXP_number_getu(r0);
		SEXP_free(r0);

		r0 = SEXP_list_nth(ent, 2);
		r1 = SEXP_list_nth(r0, val_idx + 1);
		SEXP_free(r0);

		return (r1);
	} else {
		return (SEXP_list_nth(ent, 2));
	}
}

SEXP_t *probe_ent_getattrval(const SEXP_t * ent, const char *name)
{
	SEXP_t *attrs;

	_LOGCALL_;

	if (ent == NULL) {
		errno = EFAULT;
		return (NULL);
	}

	attrs = SEXP_list_first(ent);

	if (SEXP_listp(attrs)) {
		SEXP_t *attr;
		uint32_t i;

		i = 2;

		while ((attr = SEXP_list_nth(attrs, i)) != NULL) {
			if (SEXP_stringp(attr)) {
				char attr_name[32 + 1];
				size_t attr_nlen;

				attr_nlen = SEXP_string_cstr_r(attr, attr_name, sizeof attr_name);

				if (attr_nlen > 2) {
					if (attr_name[0] == ':') {
						if (strcmp(attr_name + 1, name) == 0) {
							SEXP_free(attr);
							attr = SEXP_list_nth(attrs, i + 1);
							SEXP_free(attrs);
							return attr;
						}
					}
				}

				SEXP_free(attr);
			}

			++i;
		}
	}

	SEXP_free(attrs);
	return (NULL);
}

bool probe_ent_attrexists(const SEXP_t * ent, const char *name)
{
	_LOGCALL_;
	return probe_obj_attrexists(ent, name);
}

int probe_ent_setdatatype(SEXP_t * ent, oval_datatype_t type)
{
	SEXP_t *val;

	_A(ent != NULL);
	_LOGCALL_;

	val = probe_ent_getval(ent);

	if (val == NULL)
		return (-1);

	switch (type) {
	case OVAL_DATATYPE_BINARY:
		return SEXP_datatype_set(val, "binary");
	case OVAL_DATATYPE_BOOLEAN:
		return SEXP_datatype_set(val, "bool");
	case OVAL_DATATYPE_EVR_STRING:
		return SEXP_datatype_set(val, "evr_str");
	case OVAL_DATATYPE_FILESET_REVISION:
		return SEXP_datatype_set(val, "fset_rev");
	case OVAL_DATATYPE_FLOAT:
		/* TODO */
		return (-1);
	case OVAL_DATATYPE_IOS_VERSION:
		return SEXP_datatype_set(val, "ios_ver");
	case OVAL_DATATYPE_VERSION:
		return SEXP_datatype_set(val, "version");
	case OVAL_DATATYPE_INTEGER:
		/* TODO */
		return (-1);
	case OVAL_DATATYPE_STRING:
		return (SEXP_stringp(val) ? 0 : -1);
	default:
		return (-1);
	}

	return (0);
}

oval_datatype_t probe_ent_getdatatype(const SEXP_t * ent)
{
	SEXP_t *val;
	const char *str;
	oval_datatype_t ret;

	_A(ent != NULL);
	_LOGCALL_;

	val = probe_ent_getval(ent);
	ret = OVAL_DATATYPE_UNKNOWN;

	if (val == NULL)
		return (OVAL_DATATYPE_UNKNOWN);

	str = SEXP_datatype(val);

	if (str != NULL) {
		switch (str[0]) {
		case 'b':
			if (strcmp(str + 1, "ool") == 0)
				ret = OVAL_DATATYPE_BOOLEAN;
			else if (strcmp(str + 1, "inary") == 0)
				ret = OVAL_DATATYPE_BINARY;
			break;
		case 'e':
			if (strcmp(str + 1, "vr_str") == 0)
				ret = OVAL_DATATYPE_EVR_STRING;
			break;
		case 'f':
			if (strcmp(str + 1, "set_rev") == 0)
				ret = OVAL_DATATYPE_FILESET_REVISION;
			break;
		case 'i':
			if (strcmp(str + 1, "os_ver") == 0)
				ret = OVAL_DATATYPE_IOS_VERSION;
			break;
		case 'v':
			if (strcmp(str + 1, "ersion") == 0)
				ret = OVAL_DATATYPE_VERSION;
			break;
		}
	} else {
		switch (SEXP_typeof(val)) {
		case SEXP_TYPE_NUMBER:

			switch (SEXP_number_type(val)) {
			case SEXP_NUM_FLOAT:
				ret = OVAL_DATATYPE_FLOAT;
				break;
			case SEXP_NUM_BOOLEAN:
				ret = OVAL_DATATYPE_BOOLEAN;
				break;
			case SEXP_NUM_NONE:
				abort();
			default:
				ret = OVAL_DATATYPE_INTEGER;
			}

			break;
		case SEXP_TYPE_STRING:
			ret = OVAL_DATATYPE_STRING;
			break;
		}
	}

	SEXP_free(val);

	return (ret);
}

int probe_ent_setmask(SEXP_t * ent, bool mask)
{
	_LOGCALL_;
	/* TBI */
	return (-1);
}

bool probe_ent_getmask(const SEXP_t * ent)
{
	_LOGCALL_;
	/* TBI */
	return (false);
}

int probe_ent_setstatus(SEXP_t * ent, oval_syschar_status_t status)
{
	_LOGCALL_;
        
        probe_item_attr_add (ent, "status", SEXP_number_newi_32(status));
	return (0);
}

oval_syschar_status_t probe_ent_getstatus(const SEXP_t * ent)
{
        SEXP_t *val;
        oval_syschar_status_t sta;
        
	_LOGCALL_;
	
        val = probe_ent_getattrval (ent, "status");
        
        if (val != NULL) {
                sta = (oval_syschar_status_t) SEXP_number_geti_32 (val);
                SEXP_free (val);
        } else {
                sta = OVAL_STATUS_EXISTS;
        }
        
	return (sta);
}

char *probe_ent_getname(const SEXP_t * ent)
{
	SEXP_t *ent_name;
	char *name_str;

	_LOGCALL_;

	if (ent == NULL) {
		errno = EFAULT;
		return (NULL);
	}

	name_str = NULL;
	ent_name = SEXP_list_first(ent);

	if (ent_name == NULL) {
		errno = EINVAL;
		return (NULL);
	}

	switch (SEXP_typeof(ent_name)) {
	case SEXP_TYPE_LIST:
		{
			SEXP_t *tmp;

			tmp = SEXP_list_first(ent_name);
			SEXP_free(ent_name);
			ent_name = tmp;

			if (!SEXP_stringp(ent_name)) {
				errno = EINVAL;
				break;
			}
		}
	case SEXP_TYPE_STRING:
		if (SEXP_string_length(ent_name) > 0)
			name_str = SEXP_string_cstr(ent_name);
		else
			errno = EINVAL;
	}

	SEXP_free(ent_name);

	return (name_str);
}

size_t probe_ent_getname_r(const SEXP_t * ent, char *buffer, size_t buflen)
{
	SEXP_t *ent_name;
	size_t name_len;

	_LOGCALL_;

	if (ent == NULL) {
		errno = EFAULT;
		return (0);
	}

	name_len = 0;
	ent_name = SEXP_list_first(ent);

	if (ent_name == NULL) {
		errno = EINVAL;
		return (0);
	}

	switch (SEXP_typeof(ent_name)) {
	case SEXP_TYPE_LIST:
		{
			SEXP_t *tmp;

			tmp = SEXP_list_first(ent_name);
			SEXP_free(ent_name);
			ent_name = tmp;

			if (!SEXP_stringp(ent_name)) {
				errno = EINVAL;
				break;
			}
		}
	case SEXP_TYPE_STRING:
		if (SEXP_string_length(ent_name) > 0)
			name_len = SEXP_string_cstr_r(ent_name, buffer, buflen);
		else
			errno = EINVAL;
	}

	SEXP_free(ent_name);

	return (name_len);
}

void probe_free(SEXP_t * obj)
{
	SEXP_free(obj);
}

