/**
 * @addtogroup PROBEAPI
 * @{
 * @file   probe-api.c
 * @brief  Probe API implmentation
 * @author "Daniel Kopecek" <dkopecek@redhat.com>
 * @author "Tomas Heinrich" <theinric@redhat.com>
 */
/*
 * Copyright 2009-2011 Red Hat Inc., Durham, North Carolina.
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
 *      Tomas Heinrich <theinric@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdarg.h>
#include <string.h>
#include <errno.h>

#include "debug_priv.h"
#include "_probe-api.h"
#include "assume.h"
#include "memusage.h"
#include "sysinfo.h"
#include "oval_probe_impl.h"

extern probe_rcache_t  *OSCAP_GSYM(pcache);
extern probe_ncache_t  *OSCAP_GSYM(encache);
extern struct id_desc_t OSCAP_GSYM(id_desc);

/*
 * items
 */
SEXP_t *probe_item_creat(const char *name, SEXP_t * attrs, ...)
{
	va_list ap;
	SEXP_t *itm, *ns, *val, *ent;

	_LOGCALL_;

	va_start(ap, attrs);

	itm = probe_item_new(name, attrs);
	name = va_arg(ap, const char *);

	while (name != NULL) {
		attrs = va_arg(ap, SEXP_t *);
		val = va_arg(ap, SEXP_t *);

                ns  = probe_ncache_ref (OSCAP_GSYM(encache), name);
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
		SEXP_list_add(itm, ent);

		SEXP_free(ent);

		name = va_arg(ap, const char *);
	}

	return (itm);
}

SEXP_t *probe_item_new(const char *name, SEXP_t * attrs)
{
	SEXP_t *itm, *sid, *attr;

	_LOGCALL_;

	sid = probe_item_newid(&(OSCAP_GSYM(id_desc)));
	attr = probe_attr_creat("id", sid, NULL);
	if (attrs != NULL) {
		attrs = SEXP_list_join(attr, attrs);
		SEXP_free(attr);
	} else
		attrs = attr;
	/*
	 * Objects have the same structure as items.
	 */
	itm = probe_obj_new(name, attrs);
	SEXP_vfree(sid, attrs, NULL);

	return itm;
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
        SEXP_t *r0;

	_A(obj != NULL);
	_LOGCALL_;

	probe_item_attr_add(obj, "status", r0 = SEXP_number_newi_32((int) status));
        SEXP_free(r0);

	return (0);
}

int probe_itement_setstatus(SEXP_t *obj, const char *name, uint32_t n, oval_syschar_status_t status)
{
        SEXP_t *ent_h, *ent_s, *r0;
	_LOGCALL_;

        ent_h = probe_item_getent (obj, name, n);
        ent_s = SEXP_unref (ent_h);

        _A(ent_s != NULL);

        probe_ent_attr_add (ent_s, "status", r0 = SEXP_number_newi_32 ((int) status));
        SEXP_free (ent_s);
        SEXP_free (r0);

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

void probe_item_resetidctr(struct id_desc_t *id_desc)
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

                ns  = probe_ncache_ref (OSCAP_GSYM(encache), name);
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
	ns  = probe_ncache_ref (OSCAP_GSYM(encache), name);

	if (attrs != NULL) {
		SEXP_t *nl, *nj;

		nl = SEXP_list_new(ns, NULL);
		nj = SEXP_list_join(nl, attrs);

		SEXP_list_add(obj, nj);
		SEXP_free(nl);
                SEXP_free(nj);
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
        SEXP_t *r0;
	_LOGCALL_;

	probe_item_attr_add(obj, "status", r0 = SEXP_number_newi_32(status));
        SEXP_free(r0);

	return (-1);
}

#if 0
static int probe_obj_setentstatus(SEXP_t * obj, const char *name, uint32_t n, oval_syschar_status_t status)
{
	_LOGCALL_;
	/* TBI */
	return (-1);
}
#endif

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
SEXP_t *probe_cobj_new(oval_syschar_collection_flag_t flag, SEXP_t *msg_list, SEXP_t *item_list)
{
	SEXP_t *cobj, *r0;

	msg_list = (msg_list == NULL) ? SEXP_list_new(NULL) : SEXP_ref(msg_list);
	item_list = (item_list == NULL) ? SEXP_list_new(NULL) : SEXP_ref(item_list);
	cobj = SEXP_list_new(r0 = SEXP_number_newu(flag),
			     msg_list,
			     item_list,
			     NULL);
	SEXP_vfree(msg_list, item_list, r0, NULL);

	return cobj;
}

int probe_cobj_add_msg(SEXP_t *cobj, const SEXP_t *msg)
{
	SEXP_t *lst;

	lst = SEXP_listref_nth(cobj, 2);
	SEXP_list_add(lst, msg);
	SEXP_free(lst);

	return 0;
}

SEXP_t *probe_cobj_get_msgs(const SEXP_t *cobj)
{
	return SEXP_list_nth(cobj, 2);
}

static SEXP_t *probe_item_optimize(const SEXP_t *item);
static int probe_cobj_memcheck(size_t item_cnt);

int probe_cobj_add_item(SEXP_t *cobj, const SEXP_t *item)
{
	SEXP_t *lst, *oitem;
	size_t item_cnt;

	lst = SEXP_listref_nth(cobj, 3);
	item_cnt = SEXP_list_length(lst);
	if (probe_cobj_memcheck(item_cnt) != 0) {
		SEXP_t *msg;

		msg = probe_msg_creat(OVAL_MESSAGE_LEVEL_WARNING, "Object is incomplete due to memory constraints.");
		probe_cobj_add_msg(cobj, msg);
		probe_cobj_set_flag(cobj, SYSCHAR_FLAG_INCOMPLETE);
		SEXP_vfree(lst, msg, NULL);

		return -1;
	}

	oitem = probe_item_optimize(item);
	SEXP_list_add(lst, oitem);
	SEXP_vfree(lst, oitem, NULL);

	return 0;
}

SEXP_t *probe_cobj_get_items(const SEXP_t *cobj)
{
	return SEXP_list_nth(cobj, 3);
}

void probe_cobj_set_flag(SEXP_t *cobj, oval_syschar_collection_flag_t flag)
{
	SEXP_t *sflag, *old_sflag;
	oval_syschar_collection_flag_t of;

	sflag = SEXP_number_newu(flag);
	old_sflag = SEXP_list_replace(cobj, 1, sflag);
	of = SEXP_number_getu(old_sflag);
	SEXP_free(old_sflag);
	SEXP_free(sflag);
	dI("old flag: %d, new flag: %d.\n", of, flag);
}

oval_syschar_collection_flag_t probe_cobj_get_flag(const SEXP_t *cobj)
{
	SEXP_t *sflag;
	oval_syschar_collection_flag_t flag;

	sflag = SEXP_list_first(cobj);
	if (sflag == NULL) {
		dE("sflag == NULL.\n");
		return SYSCHAR_FLAG_UNKNOWN;
	}

	flag = SEXP_number_getu(sflag);
	SEXP_free(sflag);

	return flag;
}

oval_syschar_collection_flag_t probe_cobj_combine_flags(oval_syschar_collection_flag_t f1,
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

oval_syschar_collection_flag_t probe_cobj_compute_flag(SEXP_t *cobj)
{
	oval_syschar_collection_flag_t flag;
	SEXP_t *items, *item;
	int error_cnt = 0;
	int exists_cnt = 0;
	int does_not_exist_cnt = 0;
	int not_collected_cnt = 0;

	items = probe_cobj_get_items(cobj);
	SEXP_list_foreach(item, items) {
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
			flag = SYSCHAR_STATUS_ERROR;
			goto cleanup;
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

	if (probe_cobj_get_flag(cobj) == SYSCHAR_FLAG_UNKNOWN)
		probe_cobj_set_flag(cobj, flag);

 cleanup:
	SEXP_free(items);
	return flag;
}

/*
 * messages
 */
SEXP_t *probe_msg_creat(oval_message_level_t level, char *message)
{
	SEXP_t *lvl, *str, *msg;

	_D(message);
	lvl = SEXP_number_newu(level);
	str = SEXP_string_newf("%s", message);
	msg = SEXP_list_new(lvl, str, NULL);
	SEXP_vfree(lvl, str, NULL);

	return msg;
}

SEXP_t *probe_msg_creatf(oval_message_level_t level, const char *fmt, ...)
{
	va_list alist;
	int len;
	char *cstr;
	SEXP_t *lvl, *str, *msg;

	_LOGCALL_;

	va_start(alist, fmt);
	len = vasprintf(&cstr, fmt, alist);
	va_end(alist);
	if (len < 0)
		return NULL;

	_D(cstr);
	str = SEXP_string_new(cstr, len);
	oscap_free(cstr);
	lvl = SEXP_number_newu(level);
	msg = SEXP_list_new(lvl, str, NULL);
	SEXP_vfree(lvl, str, NULL);

	return msg;
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
	ns  = probe_ncache_ref (OSCAP_GSYM(encache), name);

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
	SEXP_t *val_lst;
	int len;

	_LOGCALL_;
	if (probe_ent_attrexists(ent, "var_ref"))
		val_lst = SEXP_list_nth(ent, 2);
	else
		val_lst = SEXP_list_rest(ent);
	len = SEXP_list_length(val_lst);

	if (res != NULL)
		(*res) = val_lst;
	else
		SEXP_free(val_lst);

	return (len);
}

SEXP_t *probe_ent_getval(const SEXP_t * ent)
{
	_LOGCALL_;
	if (probe_ent_attrexists(ent, "var_ref")) {
		SEXP_t *r0, *r1;
		unsigned int val_idx;

		r0 = probe_ent_getattrval(ent, "val_idx");
		if (r0 == NULL) {
			r0 = SEXP_list_nth(ent, 2);
			r1 = SEXP_list_first(r0);
		} else {
			val_idx = SEXP_number_getu(r0);
			SEXP_free(r0);

			r0 = SEXP_list_nth(ent, 2);
			r1 = SEXP_list_nth(r0, val_idx + 1);
		}
		SEXP_free(r0);

		return r1;
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
			}

                        SEXP_free(attr);
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

int probe_ent_setdatatype(SEXP_t *ent, oval_datatype_t type)
{
	const char *strtype;

	_A(ent != NULL);
	_LOGCALL_;

	switch (type) {
	case OVAL_DATATYPE_BOOLEAN:
	case OVAL_DATATYPE_FLOAT:
	case OVAL_DATATYPE_INTEGER:
	case OVAL_DATATYPE_STRING:
		/* these are stored directly in each SEXP */
		return 0;
	default:
		/* set the SEXP to a user datatype */
		strtype = oval_datatype_get_text(type);
		return SEXP_datatype_set(ent, strtype);
	}
}

static oval_datatype_t _sexp_val_getdatatype(const SEXP_t *val)
{
	SEXP_type_t sdt;
	SEXP_numtype_t sndt;

	sdt = SEXP_typeof(val);

	switch (sdt) {
	case SEXP_TYPE_STRING:
		return OVAL_DATATYPE_STRING;
	case SEXP_TYPE_NUMBER:
		sndt = SEXP_number_type(val);
		switch (sndt) {
		case SEXP_NUM_BOOL:
			return OVAL_DATATYPE_BOOLEAN;
		case SEXP_NUM_FLOAT:
			return OVAL_DATATYPE_FLOAT;
		default: /* everything else is considered an integer */
			return OVAL_DATATYPE_INTEGER;
		}
	default:
		dE("Unexpected SEXP datatype: %d, '%s'.\n", sdt, SEXP_strtype(ent));
		return OVAL_DATATYPE_UNKNOWN;
	}
}

oval_datatype_t probe_ent_getdatatype(const SEXP_t * ent)
{
	const char *str;

	_A(ent != NULL);
	_LOGCALL_;

	str = SEXP_datatype(ent);
	if (str != NULL) {
		/* user datatype */
		return oval_datatype_from_text(str);
	} else {
		/* SEXP datatype */
		SEXP_t *val;
		oval_datatype_t dt;

		val = probe_ent_getval(ent);
		if (val == NULL)
			return OVAL_DATATYPE_UNKNOWN;

		dt = _sexp_val_getdatatype(val);
		SEXP_free(val);

		return dt;
	}
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
        SEXP_t *r0;
	_LOGCALL_;

        probe_item_attr_add (ent, "status", r0 = SEXP_number_newi_32(status));
        SEXP_free(r0);

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

void probe_filebehaviors_canonicalize(SEXP_t **behaviors)
{
	SEXP_t *bhs, *r0;

	_A(behaviors != NULL);

	bhs = *behaviors;

	if (!bhs) {
		r0 = SEXP_list_new(NULL);
		bhs = *behaviors = probe_ent_creat1("behaviors", r0, NULL);
		SEXP_free(r0);
	}

	if (!probe_ent_attrexists(bhs, "max_depth")) {
		r0 = SEXP_string_newf("-1");
		probe_ent_attr_add(bhs, "max_depth", r0);
		SEXP_free(r0);
	}
	if (!probe_ent_attrexists(bhs, "recurse")) {
		r0 = SEXP_string_newf("symlinks and directories");
		probe_ent_attr_add(bhs, "recurse", r0);
		SEXP_free(r0);
	}
	if (!probe_ent_attrexists(bhs, "recurse_direction")) {
		r0 = SEXP_string_newf("none");
		probe_ent_attr_add(bhs, "recurse_direction", r0);
		SEXP_free(r0);
	}
	if (!probe_ent_attrexists(bhs, "recurse_file_system")) {
		r0 = SEXP_string_newf("all");
		probe_ent_attr_add(bhs, "recurse_file_system", r0);
		SEXP_free(r0);
	}
}

void probe_tfc54behaviors_canonicalize(SEXP_t **behaviors)
{
	SEXP_t *bhs, *r0;

	_A(behaviors != NULL);

	bhs = *behaviors;

	if (!bhs) {
		r0 = SEXP_list_new(NULL);
		bhs = *behaviors = probe_ent_creat1("behaviors", r0, NULL);
		SEXP_free(r0);
	}

	probe_filebehaviors_canonicalize(&bhs);

	if (!probe_ent_attrexists(bhs, "ignore_case")) {
		r0 = SEXP_string_newf("0");
		probe_ent_attr_add(bhs, "ignore_case", r0);
		SEXP_free(r0);
	}
	if (!probe_ent_attrexists(bhs, "multiline")) {
		r0 = SEXP_string_newf("1");
		probe_ent_attr_add(bhs, "multiline", r0);
		SEXP_free(r0);
	}
	if (!probe_ent_attrexists(bhs, "singleline")) {
		r0 = SEXP_string_newf("0");
		probe_ent_attr_add(bhs, "singleline", r0);
		SEXP_free(r0);
	}
}

/**
 * Return a copy of the supplied item with optimized memory representation
 */
static SEXP_t *probe_item_optimize(const SEXP_t *item)
{
	// todo
	return SEXP_ref(item);
}

#define PROBE_RESULT_MEMCHECK_CTRESHOLD  32768  /* item count */
#define PROBE_RESULT_MEMCHECK_MINFREEMEM 128    /* MiB */
#define PROBE_RESULT_MEMCHECK_MAXRATIO   0.80   /* max. memory usage ratio - used/total */

/**
 * Return true if the memory constraints are not reached.
 */
static int probe_cobj_memcheck(size_t item_cnt)
{
	if (item_cnt > PROBE_RESULT_MEMCHECK_CTRESHOLD) {
		struct memusage mu;
		struct sysinfo  si;
		double c_ratio;

		// todo: add an error message to the collected object?

		if (memusage (&mu) != 0)
			return (-1);

		if (sysinfo (&si) != 0)
			return (-1);

		c_ratio = (double)mu.mu_data/(double)((si.totalram * si.mem_unit) / 1024);

		if (c_ratio > PROBE_RESULT_MEMCHECK_MAXRATIO) {
			dW("Memory usage ratio limit reached! limit=%f, current=%f\n",
			   PROBE_RESULT_MEMCHECK_MAXRATIO, c_ratio);
			errno = ENOMEM;
			return (-1);
		}

		if (((si.freeram * si.mem_unit) / 1048576) < PROBE_RESULT_MEMCHECK_MINFREEMEM) {
			dW("Minimum free memory limit reached! limit=%zu, current=%zu\n",
			   PROBE_RESULT_MEMCHECK_MINFREEMEM, (si.freeram * si.mem_unit) / 1048576);
			errno = ENOMEM;
			return (-1);
		}
	}

	return (0);
}

SEXP_t *probe_item_create(oval_subtype_t item_subtype, probe_elmatr_t *item_attributes[],
                          /* const char *value_name, oval_datatype_t value_type, void *value, */ ...)
{
        va_list ap;
	SEXP_t *item, *name_sexp, *value_sexp = NULL, *entity;
        SEXP_t value_sexp_mem, entity_mem;
	const char *value_name, *subtype_name;
        char item_name[64];
        oval_datatype_t value_type;

        char  *value_str;
        int    value_int;
        double value_flt;
        bool   value_bool;
        bool   free_value = true;

        subtype_name = oval_subtype2str(item_subtype);

        if (subtype_name == NULL) {
                dE("Invalid/Unknown subtype: %d\n", (int)item_subtype);
                return (NULL);
        }

        if (strlen(subtype_name) + strlen("_item") < sizeof item_name) {
                strcpy(item_name, subtype_name);
                strcat(item_name, "_item");
                item_name[sizeof item_name - 1] = '\0';
        } else {
                dE("item name too long: no buffer space available\n");
                return (NULL);
        }

	va_start(ap, item_attributes);

	item       = probe_item_new(item_name, NULL);
	value_name = va_arg(ap, const char *);

        while (value_name != NULL) {
		value_type = va_arg(ap, oval_datatype_t);

                switch (value_type) {
                case OVAL_DATATYPE_STRING:
                        value_str  = va_arg(ap, char *);

                        if (value_str == NULL)
                                goto skip;

                        value_sexp = SEXP_string_new_r(&value_sexp_mem, value_str, strlen(value_str));
                        break;
                case OVAL_DATATYPE_BOOLEAN:
                        value_bool = (bool)va_arg(ap, int);
                        value_sexp = SEXP_number_newb_r(&value_sexp_mem, value_bool);
                        break;
                case OVAL_DATATYPE_INTEGER:
                        value_int  = va_arg(ap, int);
                        value_sexp = SEXP_number_newi_r(&value_sexp_mem, value_int);
                        break;
                case OVAL_DATATYPE_FLOAT:
                        value_flt  = va_arg(ap, double);
                        value_sexp = SEXP_number_newf_r(&value_sexp_mem, value_flt);
                        break;
                case OVAL_DATATYPE_SEXP:
                        value_sexp = va_arg(ap, SEXP_t *);

                        if (value_sexp == NULL)
                                goto skip;
                        else
                                free_value = false;

			value_type = _sexp_val_getdatatype(value_sexp);
                        break;
                case OVAL_DATATYPE_EVR_STRING:
                case OVAL_DATATYPE_FILESET_REVISION:
                case OVAL_DATATYPE_IOS_VERSION:
                case OVAL_DATATYPE_VERSION:
                        value_str  = va_arg(ap, char *);
                        value_sexp = SEXP_string_new_r(&value_sexp_mem, value_str, strlen(value_str));

                        break;
                        /* TODO */
                case OVAL_DATATYPE_BINARY:
                case OVAL_DATATYPE_RECORD:
                case OVAL_DATATYPE_UNKNOWN:
			dE("Unknown or unsupported OVAL datatype: %d, '%s', name: '%s'.\n",
			   value_type, oval_datatype_get_text(value_type), value_name);
                        SEXP_free(item);

                        return (NULL);
                }

                name_sexp = probe_ncache_ref(OSCAP_GSYM(encache), value_name);
                entity    = SEXP_list_new_r(&entity_mem, name_sexp, value_sexp, NULL);

		if (probe_ent_setdatatype(entity, value_type) != 0) {
			SEXP_free_r(&entity_mem);
			SEXP_free(name_sexp);
			SEXP_free(item);
			if (free_value)
				SEXP_free_r(value_sexp);

			return (NULL);
		}

                assume_d(item       != NULL, NULL);
                assume_d(entity     != NULL, NULL);
                assume_d(name_sexp  != NULL, NULL);
                assume_d(value_sexp != NULL, NULL);

                SEXP_list_add(item, entity);

                SEXP_free_r(&entity_mem);
                SEXP_free(name_sexp);

                if (free_value)
                        SEXP_free_r(&value_sexp_mem);
        skip:
                value_name = va_arg(ap, const char *);
        }

        return (item);
}
