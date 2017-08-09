/**
 * @file   xmlfilecontent.c
 * @brief  xmlfilecontent probe
 * @author "Tomas Heinrich" <theinric@redhat.com>
 *
 * 2010/06/13 dkopecek@redhat.com
 *  This probe is able to process an xmlfilecontent_object as defined in OVAL 5.4 and 5.5.
 *
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
 *      "Tomas Heinrich" <theinric@redhat.com>
 */

/*
 * xmlfilecontent probe:
 *
 *  xmlfilecontent_object
 *    xmlfilecontentbehaviors behaviors
 *    string path
 *    string filename
 *    string xpath
 *
 *  xmlfilecontent_item
 *    attrs
 *      id
 *      status_enum status
 *    string path
 *    string filename
 *    string xpath
 *    string value_of
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include <seap.h>
#include <probe-api.h>
#include <probe/probe.h>
#include <probe/option.h>
#include <oval_fts.h>
#include <common/debug_priv.h>

#define FILE_SEPARATOR '/'

struct pfdata {
	SEXP_t *filename_ent;
	char *xpath;
        probe_ctx *ctx;
};

static void dummy_err_func(void * ctx, const char * msg, ...)
{
}

void *probe_init(void)
{
	/* init libxml */
	//LIBXML_TEST_VERSION;
	xmlInitParser();
	xmlSetGenericErrorFunc(NULL, dummy_err_func);
	probe_setoption(PROBEOPT_OFFLINE_MODE_SUPPORTED, PROBE_OFFLINE_CHROOT);

	return NULL;
}

void probe_fini(void *arg)
{
        (void)arg;
	/* deinit libxml */
	xmlCleanupParser();
}

static int process_file(const char *path, const char *filename, void *arg)
{
	struct pfdata *pfd = (struct pfdata *) arg;
	int ret = 0, path_len, filename_len;
	char *whole_path = NULL;
	xmlDoc *doc = NULL;
	xmlXPathContext *xpath_ctx = NULL;
	xmlXPathObject *xpath_obj = NULL;
	SEXP_t *item = NULL;
        SEXP_t *r0;
        char filepath[PATH_MAX+1];

	if (filename == NULL)
		goto cleanup;

	path_len     = strlen(path);
	filename_len = strlen(filename);
	whole_path   = malloc(sizeof (char) * (path_len + filename_len + 2));

	memcpy (whole_path, path, sizeof (char) * path_len);

	if (whole_path[path_len - 1] != FILE_SEPARATOR) {
		whole_path[path_len] = FILE_SEPARATOR;
		++path_len;
	}

	memcpy(whole_path + path_len, filename, filename_len + 1);

	/* evaluate xpath */

	doc = xmlParseFile(whole_path);
	if (doc == NULL) {
                SEXP_t *msg;
                msg = probe_msg_creatf(OVAL_MESSAGE_LEVEL_ERROR, "Can't parse '%s'.", whole_path);
                probe_cobj_add_msg(probe_ctx_getresult(pfd->ctx), msg);
                SEXP_free(msg);
                probe_cobj_set_flag(probe_ctx_getresult(pfd->ctx), SYSCHAR_FLAG_ERROR);

		ret = -1;
		goto cleanup;
	}

	xpath_ctx = xmlXPathNewContext(doc);
	if (xpath_ctx == NULL) {
                SEXP_t *msg;
                msg = probe_msg_creatf(OVAL_MESSAGE_LEVEL_ERROR, "xmlXPathNewContext() error.");
                probe_cobj_add_msg(probe_ctx_getresult(pfd->ctx), msg);
                SEXP_free(msg);
                probe_cobj_set_flag(probe_ctx_getresult(pfd->ctx), SYSCHAR_FLAG_ERROR);

		ret = -2;
		goto cleanup;
	}

	xpath_obj = xmlXPathEvalExpression(BAD_CAST pfd->xpath, xpath_ctx);
	if (xpath_obj == NULL) {
                SEXP_t *msg;
                msg = probe_msg_creatf(OVAL_MESSAGE_LEVEL_ERROR, "xmlXPathEvalExpression() error");
                probe_cobj_add_msg(probe_ctx_getresult(pfd->ctx), msg);
                SEXP_free(msg);
                probe_cobj_set_flag(probe_ctx_getresult(pfd->ctx), SYSCHAR_FLAG_ERROR);

		ret = -3;
		goto cleanup;
	}

        snprintf(filepath, PATH_MAX, "%s%c%s", path, FILE_SEPARATOR, filename);

        item = probe_item_create(OVAL_INDEPENDENT_XML_FILE_CONTENT, NULL,
                                 "filepath", OVAL_DATATYPE_STRING, filepath,
                                 "path",     OVAL_DATATYPE_STRING, path,
                                 "filename", OVAL_DATATYPE_STRING, filename,
                                 "xpath",    OVAL_DATATYPE_STRING, pfd->xpath,
                                 NULL);

	dI("xpath obj type: %d.", xpath_obj->type);
	switch(xpath_obj->type) {
	case XPATH_BOOLEAN:
	{
		SEXP_t *val;
		int b;

		b = xmlXPathCastToBoolean(xpath_obj);
		val = SEXP_number_newb(b);
		probe_item_ent_add(item, "value_of", NULL, val);
		SEXP_free(val);
		break;
	}
	case XPATH_NUMBER:
	{
		SEXP_t *val;
		double d;

		d = xmlXPathCastToNumber(xpath_obj);
		val = SEXP_number_newf(d);
		probe_item_ent_add(item, "value_of", NULL, val);
		SEXP_free(val);
		break;
	}
	case XPATH_STRING:
	{
		SEXP_t *val;
		char *s;

		s = (char *) xmlXPathCastToString(xpath_obj);
		val = SEXP_string_newf("%s", s);
		xmlFree(s);
		probe_item_ent_add(item, "value_of", NULL, val);
		SEXP_free(val);
		break;
	}
	case XPATH_NODESET:
	{
		int node_cnt, i;
		xmlNodeSet *nodes;
		xmlNode *cur_node, **node_tab;

		nodes = xpath_obj->nodesetval;
		if (nodes == NULL) {
			ret = -4;
			goto cleanup;
		}

		node_cnt = nodes->nodeNr;
		dI("node_cnt: %d.", node_cnt);
		if (node_cnt == 0) {
			probe_item_setstatus(item, SYSCHAR_STATUS_DOES_NOT_EXIST);
			probe_item_ent_add(item, "value_of", NULL, NULL);
			probe_itement_setstatus(item, "value_of", 1, SYSCHAR_STATUS_DOES_NOT_EXIST);
		} else {
			node_tab = nodes->nodeTab;
			for (i = 0; i < node_cnt; ++i) {
				cur_node = node_tab[i];
				dI("node[%d] line: %d, name: '%s', type: %d.",
				   i, cur_node->line, cur_node->name, cur_node->type);
				if (cur_node->type == XML_ATTRIBUTE_NODE
				    || cur_node->type == XML_TEXT_NODE) {
					xmlChar *value;

					value = xmlNodeGetContent(cur_node);
					probe_item_ent_add(item, "value_of", NULL,
							   r0 = SEXP_string_newf ("%s", (char *) value));
					xmlFree(value);
					SEXP_free (r0);
				}
			}
		}
		break;
	}
	default:
		probe_item_setstatus(item, SYSCHAR_STATUS_DOES_NOT_EXIST);
		break;
	}

        probe_item_collect(pfd->ctx, item);
        item = NULL;
 cleanup:
	if (item != NULL)
		SEXP_free(item);
	if (xpath_obj != NULL)
		xmlXPathFreeObject(xpath_obj);
	if (xpath_ctx != NULL)
		xmlXPathFreeContext(xpath_ctx);
	if (doc != NULL)
		xmlFreeDoc(doc);
	if (whole_path != NULL)
		free(whole_path);

	return ret;
}

int probe_main(probe_ctx *ctx, void *arg)
{
	SEXP_t *path_ent, *filename_ent, *xpath_ent, *behaviors_ent, *filepath_ent, *probe_in;
	SEXP_t *r0;

	OVAL_FTS    *ofts;
	OVAL_FTSENT *ofts_ent;

        (void)arg;

        probe_in = probe_ctx_getobject(ctx);

        path_ent = probe_obj_getent(probe_in, "path", 1);
	filename_ent = probe_obj_getent(probe_in, "filename", 1);
	xpath_ent = probe_obj_getent(probe_in, "xpath", 1);
	filepath_ent = probe_obj_getent(probe_in, "filepath", 1);
	behaviors_ent = probe_obj_getent(probe_in, "behaviors", 1);

        /* we want (path+filename or filepath) + xpath */
        if ( ((path_ent == NULL || filename_ent == NULL) && filepath_ent==NULL ) || 
             xpath_ent==NULL) {
                SEXP_free (path_ent);
                SEXP_free (filename_ent);
                SEXP_free (filepath_ent);
                SEXP_free (xpath_ent);
		SEXP_free (behaviors_ent);

                return PROBE_ENOELM;
        }

        /* behaviours are not important if filepath is used */
        if(filepath_ent != NULL && behaviors_ent != NULL) {
                SEXP_free (behaviors_ent);
                behaviors_ent = NULL;
        }

	probe_filebehaviors_canonicalize(&behaviors_ent);

	struct pfdata pfd;

	pfd.xpath = SEXP_string_cstr(r0 = probe_ent_getval(xpath_ent));
        SEXP_free (r0);

	pfd.filename_ent = filename_ent;
        pfd.ctx = ctx;

	if ((ofts = oval_fts_open(path_ent, filename_ent, filepath_ent, behaviors_ent, probe_ctx_getresult(ctx))) != NULL) {
		while ((ofts_ent = oval_fts_read(ofts)) != NULL) {
			process_file(ofts_ent->path, ofts_ent->file, &pfd);
			oval_ftsent_free(ofts_ent);
		}

		oval_fts_close(ofts);
	}

        free(pfd.xpath);
        SEXP_free (path_ent);
        SEXP_free (filename_ent);
        SEXP_free (xpath_ent);
        SEXP_free (behaviors_ent);
        SEXP_free (filepath_ent);

	return 0;
}
