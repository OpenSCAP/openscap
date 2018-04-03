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
#include <stdbool.h>
#include <errno.h>
#ifdef _WIN32
#include <winsock2.h>
#include <in6addr.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h> /* inet_pton() in probe_ent_from_cstr() */
#include <netinet/in.h>
#include <sys/socket.h>
#endif

#include "debug_priv.h"
#include "_probe-api.h"
#include "oval_probe_impl.h"
#include "probe/entcmp.h"
#include "probe/probe.h"
#include "SEAP/generic/strto.h"

extern probe_rcache_t  *OSCAP_GSYM(pcache);
extern probe_ncache_t  *OSCAP_GSYM(ncache);
extern struct id_desc_t OSCAP_GSYM(id_desc);
extern probe_option_t *OSCAP_GSYM(probe_optdef);
extern size_t OSCAP_GSYM(probe_optdef_count);

/*
 * items
 */
SEXP_t *probe_item_creat(const char *name, SEXP_t * attrs, ...)
{
	va_list ap;
	SEXP_t *itm, *ns, *val, *ent;

	va_start(ap, attrs);

	itm = probe_item_new(name, attrs);
	name = va_arg(ap, const char *);

	while (name != NULL) {
		attrs = va_arg(ap, SEXP_t *);
		val = va_arg(ap, SEXP_t *);

                ns  = probe_ncache_ref (OSCAP_GSYM(ncache), name);
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

	va_end(ap);

	return (itm);
}

SEXP_t *probe_item_new(const char *name, SEXP_t * attrs)
{
	SEXP_t *itm, *sid, *attr;

        /*
         * Allocate space for the ID which will be generated
         * in the icache worker
         */
	sid  = SEXP_string_new("", 0);
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
	SEXP_free(sid);
	SEXP_free(attrs);

	return itm;
}

SEXP_t *probe_item_attr_add(SEXP_t * item, const char *name, SEXP_t * val)
{
	SEXP_t *n_ref, *ns;

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

	ent = probe_ent_creat1(name, attrs, val);
	SEXP_list_add(item, ent);
	SEXP_free(ent);

	return (item);
}

int probe_item_setstatus(SEXP_t *obj, oval_syschar_status_t status)
{
        SEXP_t *r0;

	_A(obj != NULL);

	probe_item_attr_add(obj, "status", r0 = SEXP_number_newi_32((int) status));
        SEXP_free(r0);

	return (0);
}

int probe_itement_setstatus(SEXP_t *obj, const char *name, uint32_t n, oval_syschar_status_t status)
{
        SEXP_t *ent_h, *ent_s, *r0;

        ent_h = probe_item_getent (obj, name, n);
        ent_s = SEXP_unref (ent_h);

        _A(ent_s != NULL);

        probe_ent_attr_add (ent_s, "status", r0 = SEXP_number_newi_32 ((int) status));
        SEXP_free (ent_s);
        SEXP_free (r0);

        return (0);
}

void probe_item_resetidctr(struct id_desc_t *id_desc)
{
	id_desc->item_id_ctr = 1;
}

bool probe_item_filtered(const SEXP_t *item, const SEXP_t *filters)
{
	bool filtered = false;
	SEXP_t *filter, *ste;

	SEXP_list_foreach(filter, filters) {
		SEXP_t *felm, *ste_res, *r0;
		oval_result_t ores;
		oval_operator_t oopr;
		oval_filter_action_t ofact;

		r0 = SEXP_list_first(filter);
		ofact = SEXP_number_getu(r0);
		SEXP_free(r0);
		ste = SEXP_list_nth(filter, 2);
		ste_res = SEXP_list_new(NULL);

		SEXP_sublist_foreach(felm, ste, 2, SEXP_LIST_END) {
			SEXP_t *ielm, *elm_res;
			char *elm_name;
			oval_check_t ochk;
			int i;

			elm_res = SEXP_list_new(NULL);
			elm_name = probe_ent_getname(felm);

			for (i = 1;; ++i) {
				ielm = probe_obj_getent(item, elm_name, i);

				if (ielm == NULL)
					break;

				ores = probe_entste_cmp(felm, ielm);
				SEXP_list_add(elm_res, r0 = SEXP_number_newi_32(ores));

				SEXP_free(ielm);
				SEXP_free(r0);
			}

			if (SEXP_list_length(elm_res) > 0) {
				free(elm_name);
				r0 = probe_ent_getattrval(felm, "entity_check");

				if (r0 == NULL)
					ochk = OVAL_CHECK_ALL;
				else
					ochk = SEXP_number_geti_32(r0);

				SEXP_free(r0);

				ores = probe_ent_result_bychk(elm_res, ochk);
				SEXP_free(elm_res);
			} else {
				SEXP_free(elm_res);
				ores = OVAL_RESULT_FALSE;
			}
			SEXP_list_add(ste_res, r0 = SEXP_number_newi_32(ores));
			SEXP_free(r0);
		}

		r0 = probe_ent_getattrval(ste, "operator");
		if (r0 == NULL)
			oopr = OVAL_OPERATOR_AND;
		else
			oopr = SEXP_number_geti_32(r0);
		ores = probe_ent_result_byopr(ste_res, oopr);
		SEXP_free(ste);
		SEXP_free(ste_res);
		SEXP_free(r0);

		if ((ores == OVAL_RESULT_TRUE && ofact == OVAL_FILTER_ACTION_EXCLUDE)
		    || (ores == OVAL_RESULT_FALSE && ofact == OVAL_FILTER_ACTION_INCLUDE)) {
			filtered = true;
			SEXP_free(filter);
			break;
		}
	}

	return filtered;
}

/*
 * attributes
 */

SEXP_t *probe_attr_creat(const char *name, const SEXP_t * val, ...)
{
	va_list ap;
	SEXP_t *list, *ns;

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

	va_start(ap, attrs);

	obj = probe_obj_new(name, attrs);
	name = va_arg(ap, const char *);

	while (name != NULL) {
		attrs = va_arg(ap, SEXP_t *);
		val = va_arg(ap, SEXP_t *);

                ns  = probe_ncache_ref (OSCAP_GSYM(ncache), name);
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

	va_end(ap);
	return (obj);
}

SEXP_t *probe_obj_new(const char *name, SEXP_t * attrs)
{
	SEXP_t *obj, *ns;

	obj = SEXP_list_new(NULL);
	ns  = probe_ncache_ref (OSCAP_GSYM(ncache), name);

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
#if !defined(NDEBUG) && defined(SEAP_VERBOSE_DEBUG)
			char buf[128];
			SEXP_string_cstr_r(ent_name, buf, sizeof buf);
			dI("1=\"%s\", 2=\"%s\", n=%u", buf, name, n);
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

	ent = probe_obj_getent(obj, name, n);
	val = probe_ent_getval(ent);

	SEXP_free(ent);

	return (val);
}

int probe_obj_getentvals(const SEXP_t * obj, const char *name, uint32_t n, SEXP_t ** res)
{
	SEXP_t *ent;
	int ret;

	ent = probe_obj_getent(obj, name, n);
	ret = probe_ent_getvals(ent, res);

	SEXP_free(ent);

	return (ret);
}

oval_version_t probe_obj_get_schema_version(const SEXP_t *obj)
{
	oval_schema_version_t version = probe_obj_get_platform_schema_version(obj);
	// oval_schema_version_to_cstr result has to be freed despite being
	// declared as const char* :-(
	char *version_str = (char*)oval_schema_version_to_cstr(version);
	oval_version_t old_version_format = oval_version_from_cstr(version_str);
	free(version_str);
	return old_version_format;
}

oval_schema_version_t probe_obj_get_platform_schema_version(const SEXP_t *obj)
{
	SEXP_t *sexp_ver;

	if (obj == NULL)
		return OVAL_SCHEMA_VERSION_INVALID;
	sexp_ver = probe_obj_getattrval(obj, "oval_version");

	if (!SEXP_stringp(sexp_ver)) {
		SEXP_free(sexp_ver);
		return OVAL_SCHEMA_VERSION_INVALID;
	}

	char *ver = SEXP_string_cstr(sexp_ver);
	SEXP_free(sexp_ver);
	oval_schema_version_t parsed_version = oval_schema_version_from_cstr(ver);
	free(ver);
	return parsed_version;
}



SEXP_t *probe_obj_getattrval(const SEXP_t * obj, const char *name)
{
	SEXP_t *obj_name;

	obj_name = SEXP_list_first(obj);

	if (SEXP_listp(obj_name)) {
		uint32_t i;
		SEXP_t *attr;
		char *name_buf = NULL;

		i = 2;

		while ((attr = SEXP_list_nth(obj_name, i)) != NULL) {
			if (SEXP_stringp(attr)) {
				if (SEXP_string_nth(attr, 1) == ':') {
					if (name_buf == NULL) {
						name_buf = oscap_sprintf(":%s", name);
					}
					if (SEXP_strcmp(attr, name_buf) == 0) {
						SEXP_t *val;

						val = SEXP_list_nth(obj_name, i + 1);
						free(name_buf);
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
		free(name_buf);
	}

	SEXP_free(obj_name);

	return (NULL);
}

bool probe_obj_attrexists(const SEXP_t * obj, const char *name)
{
	SEXP_t *obj_name;
	obj_name = SEXP_list_first(obj);

	if (SEXP_listp(obj_name)) {
		uint32_t i;
		SEXP_t *attr;
		char *name_buf = NULL;

		i = 2;

		while ((attr = SEXP_list_nth(obj_name, i)) != NULL) {
			if (SEXP_stringp(attr)) {
				if (SEXP_string_nth(attr, 1) == ':') {
					if (name_buf == NULL) {
						name_buf = oscap_sprintf(":%s", name);
					}
					if (SEXP_strcmp(attr, name_buf) == 0) {
						free(name_buf);
						SEXP_free(attr);
						SEXP_free(obj_name);

						return (true);
					}
					++i;
				} else {
					if (SEXP_strcmp(attr, name) == 0) {
						free(name_buf);
                                        SEXP_free(attr);
                                        SEXP_free(obj_name);
                                        return true;
                                    }
                                }
				++i;
			}

			SEXP_free(attr);
		}
		free(name_buf);
	}

	SEXP_free(obj_name);

	return (false);
}

int probe_obj_setstatus(SEXP_t * obj, oval_syschar_status_t status)
{
        SEXP_t *r0;

	probe_item_attr_add(obj, "status", r0 = SEXP_number_newi_32(status));
        SEXP_free(r0);

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
SEXP_t *probe_cobj_new(oval_syschar_collection_flag_t flag, SEXP_t *msg_list, SEXP_t *item_list, SEXP_t *mask_list)
{
	SEXP_t *cobj, *r0;

	msg_list = (msg_list == NULL) ? SEXP_list_new(NULL) : SEXP_ref(msg_list);
	item_list = (item_list == NULL) ? SEXP_list_new(NULL) : SEXP_ref(item_list);
        mask_list = mask_list == NULL ? SEXP_list_new(NULL) : SEXP_ref(mask_list);
	cobj = SEXP_list_new(r0 = SEXP_number_newu(flag),
			     msg_list,
			     item_list,
                             mask_list,
			     NULL);
	SEXP_free(msg_list);
	SEXP_free(item_list);
	SEXP_free(mask_list);
	SEXP_free(r0);

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

SEXP_t *probe_cobj_get_mask(const SEXP_t *cobj)
{
    return SEXP_list_nth(cobj, 4);
}

static SEXP_t *probe_item_optimize(const SEXP_t *item);

int probe_cobj_add_item(SEXP_t *cobj, const SEXP_t *item)
{
	SEXP_t *lst, *oitem;

	lst = SEXP_listref_nth(cobj, 3);
	oitem = probe_item_optimize(item);
	SEXP_list_add(lst, oitem);
	SEXP_free(lst);
	SEXP_free(oitem);

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
	dD("old flag: %d, new flag: %d.", of, flag);
}

oval_syschar_collection_flag_t probe_cobj_get_flag(const SEXP_t *cobj)
{
	SEXP_t *sflag;
	oval_syschar_collection_flag_t flag;

	sflag = SEXP_list_first(cobj);
	if (sflag == NULL) {
		dE("sflag == NULL.");
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

	dI("%s", message);
	lvl = SEXP_number_newu(level);
	str = SEXP_string_newf("%s", message);
	msg = SEXP_list_new(lvl, str, NULL);
	SEXP_free(lvl);
	SEXP_free(str);

	return msg;
}

SEXP_t *probe_msg_creatf(oval_message_level_t level, const char *fmt, ...)
{
	va_list alist;
	int len = 0;
	SEXP_t *lvl, *str, *msg;
	char *cstr = NULL;

	va_start(alist, fmt);
	len = vsnprintf(cstr, len, fmt, alist);
	va_end(alist);

	if (len < 0) {
		return NULL;
	}

	len++;
	cstr = malloc(len);

	va_start(alist, fmt);
	len = vsnprintf(cstr, len, fmt, alist);
	va_end(alist);

	if (len < 0) {
		free(cstr);
		return NULL;
	}

	dI("%s", cstr);
	str = SEXP_string_new(cstr, len);
	free(cstr);
	lvl = SEXP_number_newu(level);
	msg = SEXP_list_new(lvl, str, NULL);
	SEXP_free(lvl);
	SEXP_free(str);

	return msg;
}

/*
 * entities
 */
SEXP_t *probe_ent_creat(const char *name, SEXP_t * attrs, SEXP_t * val, ...)
{
	va_list ap;
	SEXP_t *ent_list, *ent;

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

	va_end(ap);

	return (ent_list);
}

SEXP_t *probe_ent_creat1(const char *name, SEXP_t * attrs, SEXP_t * val)
{
	SEXP_t *ent, *ns;

	ent = SEXP_list_new(NULL);
	ns  = probe_ncache_ref (OSCAP_GSYM(ncache), name);

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
	return (probe_item_attr_add(ent, name, val));
}

int probe_ent_getvals(const SEXP_t * ent, SEXP_t ** res)
{
	SEXP_t *val_lst;
	int len;

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
	return probe_obj_attrexists(ent, name);
}

int probe_ent_setdatatype(SEXP_t *ent, oval_datatype_t type)
{
	const char *strtype;

	_A(ent != NULL);

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
		dE("Unexpected SEXP datatype: %d, '%s'.", sdt, SEXP_strtype(val));
		return OVAL_DATATYPE_UNKNOWN;
	}
}

oval_datatype_t probe_ent_getdatatype(const SEXP_t * ent)
{
	const char *str;

	_A(ent != NULL);

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
	/* TBI */
	return (-1);
}

bool probe_ent_getmask(const SEXP_t * ent)
{
	/* TBI */
	return (false);
}

int probe_ent_setstatus(SEXP_t * ent, oval_syschar_status_t status)
{
        SEXP_t *r0;

        probe_item_attr_add (ent, "status", r0 = SEXP_number_newi_32(status));
        SEXP_free(r0);

	return (0);
}

oval_syschar_status_t probe_ent_getstatus(const SEXP_t * ent)
{
        SEXP_t *val;
        oval_syschar_status_t sta;

        val = probe_ent_getattrval (ent, "status");

        if (val != NULL) {
                sta = (oval_syschar_status_t) SEXP_number_geti_32 (val);
                SEXP_free (val);
        } else {
                sta = SYSCHAR_STATUS_EXISTS;
        }

	return (sta);
}

char *probe_ent_getname(const SEXP_t * ent)
{
	SEXP_t *ent_name;
	char *name_str;

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

SEXP_t *probe_item_create(oval_subtype_t item_subtype, probe_elmatr_t *item_attributes[],
                          /* const char *value_name, oval_datatype_t value_type, void *value, */ ...)
{
        va_list ap;
	SEXP_t *item, *name_sexp, *value_sexp = NULL, *entity;
        SEXP_t value_sexp_mem, entity_mem;
	const char *value_name, *subtype_name, *family_name;
	oval_family_t family;
	char item_name[128];
        oval_datatype_t value_type;

        char   *value_str, **value_stra;
        int64_t value_int;
        double  value_flt;
        bool    value_bool;
        bool    free_value = true;
        bool    multiplied;
        int     value_i, multiply;

        subtype_name = oval_subtype_to_str(item_subtype);

        if (subtype_name == NULL) {
                dE("Invalid/Unknown subtype: %d", (int)item_subtype);
                return (NULL);
        }

	family = oval_subtype_get_family(item_subtype);
	family_name = oval_family_get_text(family);
	snprintf(item_name, sizeof(item_name), "%s:%s_item", family_name, subtype_name);

	va_start(ap, item_attributes);

	item       = probe_item_new(item_name, NULL);
	value_name = va_arg(ap, const char *);

        while (value_name != NULL) {
                value_i    = 0;
                multiply   = 1;
                multiplied = false;
		value_type = va_arg(ap, oval_datatype_t);

                switch (value_type) {
                case OVAL_DATATYPE_STRING:
                        value_str  = va_arg(ap, char *);

                        if (value_str == NULL)
                                goto skip;

                        value_sexp = SEXP_string_new_r(&value_sexp_mem, value_str, strlen(value_str));
                        break;
                case OVAL_DATATYPE_STRING_M:
                        value_type = OVAL_DATATYPE_STRING;
                        value_stra = va_arg(ap, char **);
                        multiplied = true;
                        multiply   = 0;

                        if (value_stra == NULL)
                                goto skip;

                        while (value_stra[multiply] != NULL)
                                ++multiply;

                        value_sexp = malloc(sizeof(SEXP_t) * multiply);

                        for (value_i = 0; value_i < multiply; ++value_i)
                                SEXP_string_new_r(value_sexp + value_i, value_stra[value_i], strlen(value_stra[value_i]));

                        value_i = 0;
                        break;
                case OVAL_DATATYPE_BOOLEAN:
                        value_bool = (bool)va_arg(ap, int);
                        value_sexp = SEXP_number_newb_r(&value_sexp_mem, value_bool);
                        break;
                case OVAL_DATATYPE_INTEGER:
                        value_int  = va_arg(ap, int64_t);
                        value_sexp = SEXP_number_newi_64_r(&value_sexp_mem, value_int);
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
                case OVAL_DATATYPE_RECORD:
			entity = va_arg(ap, SEXP_t *);
			SEXP_list_add(item, entity);
			goto skip;
                case OVAL_DATATYPE_EVR_STRING:
                case OVAL_DATATYPE_DEBIAN_EVR_STRING:
                case OVAL_DATATYPE_FILESET_REVISION:
                case OVAL_DATATYPE_IOS_VERSION:
		case OVAL_DATATYPE_IPV4ADDR:
		case OVAL_DATATYPE_IPV6ADDR:
                case OVAL_DATATYPE_VERSION:
                        value_str  = va_arg(ap, char *);
                        value_sexp = SEXP_string_new_r(&value_sexp_mem, value_str, strlen(value_str));

                        break;
                        /* TODO */
                case OVAL_DATATYPE_BINARY:
                case OVAL_DATATYPE_UNKNOWN:
			dE("Unknown or unsupported OVAL datatype: %d, '%s', name: '%s'.",
			   value_type, oval_datatype_get_text(value_type), value_name);
                        SEXP_free(item);

			va_end(ap);
                        return (NULL);
                }

                name_sexp = probe_ncache_ref(OSCAP_GSYM(ncache), value_name);

                while(value_i < multiply) {
                        entity = SEXP_list_new_r(&entity_mem, name_sexp, value_sexp + value_i, NULL);

                        if (probe_ent_setdatatype(entity, value_type) != 0) {
                                SEXP_free_r(&entity_mem);
                                SEXP_free(name_sexp);
                                SEXP_free(item);

                                if (free_value) {
                                        while(value_i < multiply)
                                                SEXP_free_r(value_sexp + value_i++);
                                        if (multiplied)
                                                free(value_sexp);
                                }

				va_end(ap);
                                return (NULL);
                        }

			if (item == NULL || entity == NULL || name_sexp == NULL) {
				return NULL;
			}
                        SEXP_list_add(item, entity);
                        SEXP_free_r(&entity_mem);

                        if (free_value)
                                SEXP_free_r(value_sexp + value_i);
                        ++value_i;
                }

                SEXP_free(name_sexp);

                if (multiplied)
                        free(value_sexp);
        skip:
                value_name = va_arg(ap, const char *);
		free_value = true;
        }

	va_end(ap);
        return (item);
}

oval_operation_t probe_ent_getoperation(SEXP_t *entity, oval_operation_t default_op)
{
        oval_operation_t ret;
        SEXP_t *aval;

        if (entity == NULL) {
                dW("entity == NULL");
                return (OVAL_OPERATION_UNKNOWN);
        }

        aval = probe_ent_getattrval(entity, "operation");

        if (aval == NULL) {
                dW("Attribute \"operation\" not found. Using default.");
                return (default_op);
        }

        if (!SEXP_numberp(aval)) {
                dW("Invalid type");
                SEXP_free(aval);
                return (OVAL_OPERATION_UNKNOWN);
        }

        ret = SEXP_number_geti_32(aval);
        SEXP_free(aval);

        return (ret);
}

int probe_item_add_msg(SEXP_t *item, oval_message_level_t msglvl, char *msgfmt, ...)
{
    va_list ap;
    SEXP_t  lvl_sexp, msg_sexp;
    SEXP_t *attrs;
    char    msg_buffer[1024];
    int     msg_length;

    va_start(ap, msgfmt);

    msg_length = vsnprintf(msg_buffer, sizeof msg_buffer, msgfmt, ap);

    if (msg_length < 0) {
	dE("vsnprintf failed! errno=%u, %s.", errno, strerror(errno));
	va_end(ap);
	return (-1);
    }

    if ((size_t)msg_length >= sizeof msg_buffer) {
	dE("message too long!");
	va_end(ap);
	return (-1);
    }

    va_end(ap);

    SEXP_number_newu_32_r(&lvl_sexp, msglvl);
    SEXP_string_new_r(&msg_sexp, msg_buffer, msg_length);

    attrs = probe_attr_creat("level", &lvl_sexp, NULL);
    probe_item_ent_add(item, "message", attrs, &msg_sexp);

    SEXP_free_r(&lvl_sexp);
    SEXP_free_r(&msg_sexp);
    SEXP_free(attrs);

    return (0);
}

SEXP_t *probe_entval_from_cstr(oval_datatype_t type,
                               const char *value, size_t vallen)
{
  SEXP_t *ent_val = NULL;

  if (value == NULL || vallen == 0)
    return NULL;

	switch (type) {
	case OVAL_DATATYPE_FLOAT:
	{
		double val;
		char *end = NULL;

		val = strto_double(value, vallen, &end);
		ent_val = SEXP_number_newf(val);

	}	break;
	case OVAL_DATATYPE_INTEGER:
	{
		int64_t val;
		char *end;

		val = strto_int64(value, vallen, &end, 10);
		ent_val = SEXP_number_newi_64(val);

	}	break;
	case OVAL_DATATYPE_BOOLEAN:
		switch(vallen) {
		case 1:
			switch(*value)
			{
			case '1':
				ent_val = SEXP_number_newb(true);
				break;
			case '0':
				ent_val = SEXP_number_newb(false);
				break;
			}
			break;
		case 4:
			if (oscap_strncasecmp(value, "true", 4) == 0)
				ent_val = SEXP_number_newb(true);
			break;
		case 5:
			if (oscap_strncasecmp(value, "false", 5) == 0)
				ent_val = SEXP_number_newb(false);
			break;
		}

		if (ent_val == NULL)
			return NULL;
		break;


	case OVAL_DATATYPE_IPV4ADDR:
	{
		struct in_addr ip4;

		if (inet_pton(AF_INET, value, &ip4) != 1)
			return NULL;
	}	break;
	case OVAL_DATATYPE_IPV6ADDR:
	{
		struct in6_addr ip6;

		if (inet_pton(AF_INET6, value, &ip6) != 1)
			return NULL;
	}	break;

	case OVAL_DATATYPE_EVR_STRING:
	case OVAL_DATATYPE_DEBIAN_EVR_STRING:
	case OVAL_DATATYPE_FILESET_REVISION:
	case OVAL_DATATYPE_IOS_VERSION:
	case OVAL_DATATYPE_STRING:
	case OVAL_DATATYPE_VERSION:
		break;

	case OVAL_DATATYPE_BINARY:
	case OVAL_DATATYPE_SEXP:
	case OVAL_DATATYPE_UNKNOWN:
	case OVAL_DATATYPE_STRING_M:
	case OVAL_DATATYPE_RECORD:
	default:
		return NULL;
	}

	/*
	 * If we got here and ent_val is still NULL, then
	 * no special conversion procedure is needed and
	 * we can simply create an SEXP string...
	 */
	if (ent_val == NULL)
		ent_val = SEXP_string_new(value, vallen);

  return ent_val;
}

SEXP_t *probe_ent_from_cstr(const char *name, oval_datatype_t type,
                            const char *value, size_t vallen)
{
	SEXP_t *ent = NULL, *ent_val = NULL;

	if (name == NULL || value == NULL || vallen == 0)
		return NULL;

  ent_val = probe_entval_from_cstr(type, value, vallen);

  if (ent_val == NULL)
    return NULL;
  
	/* Create the entity... */
	ent = probe_ent_creat1(name, NULL, ent_val);	
	SEXP_free(ent_val);

	/* ...and annotate with the specified OVAL datatype */
	if (probe_ent_setdatatype(ent, type) != 0) {
		SEXP_free(ent);
		return NULL;
	}

	return ent;
}

SEXP_t *probe_obj_getmask(SEXP_t *obj)
{
    SEXP_t *objents, *ent, *ent_name, *mask;

    SEXP_VALIDATE(obj);

    if (obj == NULL)
       return NULL;

    ent     = NULL;
    mask    = SEXP_list_new(NULL);
    objents = SEXP_list_rest(obj);

    SEXP_list_foreach(ent, objents) {
        ent_name = SEXP_list_first(ent);

        if (SEXP_listp(ent_name)) {
            SEXP_t *nr;

            nr = SEXP_list_first(ent_name);
            SEXP_free(ent_name);
            ent_name = nr;

            if (probe_ent_attrexists(ent, "mask"))
                SEXP_list_add(mask, ent_name);
        }
        SEXP_free(ent_name);
    }

    SEXP_free(objents);
    return (mask);
}
/// @}
