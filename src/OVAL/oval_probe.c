/*
 * Copyright 2009-2010 Red Hat Inc., Durham, North Carolina.
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
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#if defined(OSCAP_THREAD_SAFE)
# include <pthread.h>
#endif

#include "config.h"
#include "oval_probe.h"
#include "oval_system_characteristics.h"
#include "common/_error.h"
#include "common/assume.h"

#include "oval_probe_impl.h"
#include "oval_system_characteristics_impl.h"
#include "common/util.h"
#include "common/bfind.h"
#include "common/debug_priv.h"

#include "_oval_probe_session.h"
#include "_oval_probe_handler.h"

/* Sorted by subtype (first column) */
oval_subtypedsc_t __s2n_tbl[] = {
        /*     2 */ {OVAL_SUBTYPE_SYSINFO,                  "system_info"         },
	/*  7001 */ {OVAL_INDEPENDENT_FAMILY,               "family"              },
        /*  7002 */ {OVAL_INDEPENDENT_FILE_MD5,             "filemd5"             },
        /*  7003 */ {OVAL_INDEPENDENT_FILE_HASH,            "filehash"            },
        /*  7004 */ {OVAL_INDEPENDENT_ENVIRONMENT_VARIABLE, "environmentvariable" },
	/*  7006 */ {OVAL_INDEPENDENT_TEXT_FILE_CONTENT_54, "textfilecontent54"   },
	/*  7007 */ {OVAL_INDEPENDENT_TEXT_FILE_CONTENT,    "textfilecontent"     },
        /*  7009 */ {OVAL_INDEPENDENT_VARIABLE,             "variable"            },
	/*  7010 */ {OVAL_INDEPENDENT_XML_FILE_CONTENT,     "xmlfilecontent"      },
	/*  9001 */ {OVAL_LINUX_DPKG_INFO,                  "dpkginfo"            },
	/*  9002 */ {OVAL_LINUX_INET_LISTENING_SERVERS,     "inetlisteningservers"},
	/*  9003 */ {OVAL_LINUX_RPM_INFO,                   "rpminfo"             },
	/* 13001 */ {OVAL_UNIX_FILE,                        "file"                },
	/* 13003 */ {OVAL_UNIX_INTERFACE,                   "interface"           },
	/* 13004 */ {OVAL_UNIX_PASSWORD,                    "password"            },
	/* 13005 */ {OVAL_UNIX_PROCESS,                     "process"             },
	/* 13006 */ {OVAL_UNIX_RUNLEVEL,                    "runlevel"            },
	/* 13008 */ {OVAL_UNIX_SHADOW,                      "shadow"              },
	/* 13009 */ {OVAL_UNIX_UNAME,                       "uname"               }
};

#define __s2n_tbl_count (sizeof __s2n_tbl / sizeof(oval_subtypedsc_t))

static int __s2n_tbl_cmp(oval_subtype_t *type, oval_subtypedsc_t *dsc)
{
        return (*type - dsc->type);
}

/* Sorted by name (second column) */
oval_subtypedsc_t __n2s_tbl[] = {
	/*  9001 */ {OVAL_LINUX_DPKG_INFO,                  "dpkginfo"            },
        /*  7004 */ {OVAL_INDEPENDENT_ENVIRONMENT_VARIABLE, "environmentvariable" },
	/*  7001 */ {OVAL_INDEPENDENT_FAMILY,               "family"              },
	/* 13001 */ {OVAL_UNIX_FILE,                        "file"                },
        /*  7003 */ {OVAL_INDEPENDENT_FILE_HASH,            "filehash"            },
        /*  7002 */ {OVAL_INDEPENDENT_FILE_MD5,             "filemd5"             },
	/*  9002 */ {OVAL_LINUX_INET_LISTENING_SERVERS,     "inetlisteningservers"},
	/* 13003 */ {OVAL_UNIX_INTERFACE,                   "interface"           },
	/* 13004 */ {OVAL_UNIX_PASSWORD,                    "password"            },
	/* 13005 */ {OVAL_UNIX_PROCESS,                     "process"             },
	/*  9003 */ {OVAL_LINUX_RPM_INFO,                   "rpminfo"             },
	/* 13006 */ {OVAL_UNIX_RUNLEVEL,                    "runlevel"            },
	/* 13008 */ {OVAL_UNIX_SHADOW,                      "shadow"              },
        /*     2 */ {OVAL_SUBTYPE_SYSINFO,                  "system_info"         },
	/*  7007 */ {OVAL_INDEPENDENT_TEXT_FILE_CONTENT,    "textfilecontent"     },
	/*  7006 */ {OVAL_INDEPENDENT_TEXT_FILE_CONTENT_54, "textfilecontent54"   },
	/* 13009 */ {OVAL_UNIX_UNAME,                       "uname"               },
        /*  7009 */ {OVAL_INDEPENDENT_VARIABLE,             "variable"            },
	/*  7010 */ {OVAL_INDEPENDENT_XML_FILE_CONTENT,     "xmlfilecontent"      }
};

#define __n2s_tbl_count (sizeof __n2s_tbl / sizeof(oval_subtypedsc_t))

static int __n2s_tbl_cmp(const char *name, oval_subtypedsc_t *dsc)
{
        return strcmp(name, dsc->name);
}

#if defined(ENABLE_PROBES)
/*
 * Library side entity name cache. Initialization needs to be
 * thread-safe and is done by oval_probe_session_new. Freeing
 * of memory used by this cache is done at exit using a hook
 * registered with atexit().
 */
encache_t *OSCAP_GSYM(encache) = NULL;
struct id_desc_t OSCAP_GSYM(id_desc);
#endif

#define __ERRBUF_SIZE 128

const char *oval_subtype2str(oval_subtype_t subtype)
{
        oval_subtypedsc_t *d;

        d = oscap_bfind(__s2n_tbl, __s2n_tbl_count, sizeof(oval_subtypedsc_t), &subtype,
                        (int(*)(void *, void *))__s2n_tbl_cmp);

        return (d == NULL ? NULL : d->name);
}

oval_subtype_t oval_str2subtype(const char *str)
{
        oval_subtypedsc_t *d;

        d = oscap_bfind(__n2s_tbl, __n2s_tbl_count, sizeof(oval_subtypedsc_t), (void *)str,
                        (int(*)(void *, void *))__n2s_tbl_cmp);

        return (d == NULL ? OVAL_SUBTYPE_UNKNOWN : d->type);
}

struct oval_syschar *oval_probe_object_eval(oval_probe_session_t *psess, struct oval_object *object, int flags)
{
        struct oval_syschar *o_sys;
        oval_subtype_t type;
        oval_ph_t *ph;

        type = oval_object_get_subtype(object);
        ph   = oval_probe_handler_get(psess->ph, type);

        if (ph == NULL) {
                oscap_seterr (OSCAP_EFAMILY_OVAL, OVAL_EPROBENOTSUPP, "OVAL object not supported");
                return(NULL);
        }

        o_sys = NULL;

        if (ph->func(type, ph->uptr, PROBE_HANDLER_ACT_EVAL, object, &o_sys, flags) != 0) {
                return(NULL);
        }

        return(o_sys);
}

struct oval_sysinfo *oval_probe_sysinf_eval(oval_probe_session_t *sess)
{
	struct oval_sysinfo *sysinf;
        oval_ph_t *ph;

        ph = oval_probe_handler_get(sess->ph, OVAL_SUBTYPE_SYSINFO);

        if (ph == NULL) {
                oscap_seterr (OSCAP_EFAMILY_OVAL, OVAL_EPROBENOTSUPP, "OVAL object not supported");
                return(NULL);
        }

        sysinf = NULL;

        if (ph->func(OVAL_SUBTYPE_SYSINFO, ph->uptr, PROBE_HANDLER_ACT_EVAL, NULL, &sysinf, 0) != 0) {
                return(NULL);
        }

        return(sysinf);
}

static int oval_psess_probe_criteria(oval_probe_session_t *sess, struct oval_criteria_node *cnode);
static int oval_psess_probe_object(oval_probe_session_t *sess, struct oval_object *object);

int oval_psess_probe_sysinfo(oval_probe_session_t *sess) {
        struct oval_syschar_model *syschar_model;
	struct oval_sysinfo *sysinfo;

        sysinfo = oval_probe_sysinf_eval(sess);
        if (sysinfo == NULL) {
		/* Report error in this case */
		return -1;
	}

        syschar_model = sess->sys_model;
        oval_syschar_model_set_sysinfo(syschar_model, sysinfo);
        oval_sysinfo_free(sysinfo);
	return 0;
}

int oval_psess_probe_objects(oval_probe_session_t *sess)
{
	struct oval_syschar_model * syschar_model;
	struct oval_definition_model *definition_model;
	int ret;

	syschar_model = sess->sys_model;
	definition_model = oval_syschar_model_get_definition_model(syschar_model);

	if (definition_model) {
		struct oval_object_iterator *objects = oval_definition_model_get_objects(definition_model);
		while (oval_object_iterator_has_more(objects)) {
			struct oval_object *object = oval_object_iterator_next(objects);
			ret = oval_psess_probe_object(sess, object);
			if( ret != 0 ) {
				oval_object_iterator_free(objects);
				return -1;
			}
		}
		oval_object_iterator_free(objects);
	}
	return 0;
}


int oval_psess_probe_definition(oval_probe_session_t *sess, const char *id) {

	struct oval_syschar_model * syschar_model;
        struct oval_definition_model *definition_model;
	struct oval_definition *definition;
	int ret;

	syschar_model = sess->sys_model;
        definition_model = oval_syschar_model_get_definition_model(syschar_model);
	definition = oval_definition_model_get_definition(definition_model, id);

	struct oval_criteria_node * cnode = oval_definition_get_criteria(definition);
	if (cnode == NULL)
		return 0;

	ret = oval_psess_probe_criteria(sess, cnode);

	return ret;
}

/**
 * @returns 0 on success
 */
static int oval_psess_probe_criteria(oval_probe_session_t *sess, struct oval_criteria_node *cnode) {
        switch (oval_criteria_node_get_type(cnode)) {
                /* Criterion node is final node that has reference to test */
        case OVAL_NODETYPE_CRITERION:{
                        /* There should be a test .. */
                        struct oval_test *test = oval_criteria_node_get_test(cnode);
                        if (test == NULL)
                                return 0;
			struct oval_object *object = oval_test_get_object(test);
			if (object == NULL)
				return 0;
                        /* .. probe object and return */
                        return oval_psess_probe_object(sess, object);

                }
                break;
                /* Criteria node is type of set that contains more criterias. Criteria node
                 * child can be also type of criteria, criterion or extended definition */
        case OVAL_NODETYPE_CRITERIA:{
                        int ret;
                        /* group of criterion nodes, get subnodes, continue recursive */
                        struct oval_criteria_node_iterator *cnode_it = oval_criteria_node_get_subnodes(cnode);
                        if (cnode_it == NULL)
                                return 0;
                        /* we have subnotes */
                        struct oval_criteria_node *node;
                        while (oval_criteria_node_iterator_has_more(cnode_it)) {
                                node = oval_criteria_node_iterator_next(cnode_it);
                                ret = oval_psess_probe_criteria(sess, node);
                                if (ret != 0) {
                                        oval_criteria_node_iterator_free(cnode_it);
                                        return -1;
                                }
                        }
                        oval_criteria_node_iterator_free(cnode_it);
			return 0;
                }
                break;
                /* Extended definition contains reference to definition, we need criteria of this
                 * definition to be evaluated completely */
        case OVAL_NODETYPE_EXTENDDEF:{
                        struct oval_definition *oval_def = oval_criteria_node_get_definition(cnode);
			struct oval_criteria_node *node =  oval_definition_get_criteria(oval_def);
                        return oval_psess_probe_criteria(sess, node);
                }
                break;
        case OVAL_NODETYPE_UNKNOWN:
                break;
        }

	/* we shouldn't get here */
        return -1;
}

/**
 * @returns 0 on success
 */
static int oval_psess_probe_object(oval_probe_session_t *sess, struct oval_object *object) {

	char *objid = oval_object_get_id(object);
	struct oval_syschar_model *sys_model = sess->sys_model;
	struct oval_syschar *syschar = oval_syschar_model_get_syschar(sys_model, objid);

	if (syschar == NULL) {
		syschar = oval_probe_object_eval(sess, object, 0);
		if (syschar == NULL) {
			if(  oscap_err() ) {
		        	/* does it make sense to continue? */
				 return -1;
			}
                       	syschar = oval_syschar_new(sys_model, object);
	                oval_syschar_set_flag(syschar, SYSCHAR_FLAG_NOT_COLLECTED);
		}
		oval_syschar_model_add_syschar(sys_model, syschar);
	}
	return 0;
}

