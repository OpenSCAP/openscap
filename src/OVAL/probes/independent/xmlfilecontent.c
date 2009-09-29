#ifndef __STUB_PROBE
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

#include <stdlib.h>
#include <string.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include <seap.h>
#include <probe-api.h>
#include <findfile.h>

#define FILE_SEPARATOR '/'

struct pfdata {
	SEXP_t *filename_ent;
	char *xpath;
	SEXP_t *item_list;
};

void dummy_err_func(void * ctx, const char * msg, ...)
{
}

void *probe_init(void)
{
	/* init libxml */
	//LIBXML_TEST_VERSION;
	xmlInitParser();
	xmlSetGenericErrorFunc(NULL, dummy_err_func);

	return NULL;
}

void probe_fini(void __attribute__((unused)) *arg)
{
	/* deinit libxml */
	xmlCleanupParser();
}

static int report_missing(SEXP_t *ent)
{
	oval_operation_t op;

	op = SEXP_number_geti_32(probe_ent_getattrval(ent, "operation"));
	if (op == OVAL_OPERATION_EQUALS)
		return 1;
	else
		return 0;
}

static int process_file(const char *path, const char *filename, void *arg)
{
	struct pfdata *pfd = (struct pfdata *) arg;
	int i, ret = 0, path_len, filename_len, node_cnt;
	char *whole_path = NULL;
	xmlDocPtr doc = NULL;
	xmlXPathContextPtr xpath_ctx = NULL;
	xmlXPathObjectPtr xpath_obj = NULL;
	xmlNodeSetPtr nodes;
	xmlNodePtr cur_node, *node_tab;
	xmlChar *value;
	SEXP_t *item;

	if (filename == NULL) {
		if (report_missing(pfd->filename_ent)) {
                        item = probe_item_creat("xmlfilecontent_item", NULL,
                                                /* entities */
                                                "path", NULL,
                                                SEXP_string_newf(path),
                                                "filename", NULL,
                                                SEXP_string_newf(filename),
                                                NULL);
                        probe_item_setstatus(item, OVAL_STATUS_DOESNOTEXIST);
                        probe_itement_setstatus(item, "filename", 1, OVAL_STATUS_DOESNOTEXIST);
		} else {
                        item = probe_item_creat("xmlfilecontent_item", NULL,
                                                /* entities */
                                                "path", NULL,
                                                SEXP_string_newf(path),
                                                NULL);
		}
		SEXP_list_add(pfd->item_list, item);

		goto cleanup;
	}

	path_len = strlen(path);
	filename_len = strlen(filename);
	whole_path = malloc(path_len + filename_len + 2);
	memcpy(whole_path, path, path_len);
	if (whole_path[path_len - 1] != FILE_SEPARATOR) {
		whole_path[path_len] = FILE_SEPARATOR;
		++path_len;
	}
	memcpy(whole_path + path_len, filename, filename_len + 1);


	/* evaluate xpath */

	doc = xmlParseFile(whole_path);
	if (doc == NULL) {
		ret = -1;
		goto cleanup;
	}

	xpath_ctx = xmlXPathNewContext(doc);
	if (xpath_ctx == NULL) {
		ret = -2;
		goto cleanup;
	}

	xpath_obj = xmlXPathEvalExpression(BAD_CAST pfd->xpath, xpath_ctx);
	if (xpath_obj == NULL) {
		ret = -3;
		goto cleanup;
	}

	nodes = xpath_obj->nodesetval;
	if (nodes == NULL) {
		ret = -4;
		goto cleanup;
	}

        item = probe_item_creat("xmlfilecontent_item", NULL,
                                /* entities */
                                "path", NULL,
                                SEXP_string_newf(path),
                                "filename", NULL,
                                SEXP_string_newf(filename),
                                "xpath", NULL,
                                SEXP_string_newf(pfd->xpath),
                                NULL);

	node_cnt = nodes->nodeNr;
	if (node_cnt == 0) {
		probe_item_setstatus(item, OVAL_STATUS_DOESNOTEXIST);
		probe_item_ent_add(item, "value_of", NULL, SEXP_string_newf(""));
		probe_itement_setstatus(item, "value_of", 1, OVAL_STATUS_DOESNOTEXIST);
	} else {
		node_tab = nodes->nodeTab;
		for (i = 0; i < node_cnt; ++i) {
			cur_node = nodes->nodeTab[i];
			if (cur_node->type == XML_ATTRIBUTE_NODE ||
			    cur_node->type == XML_TEXT_NODE) {
				value = xmlNodeGetContent(cur_node);
				probe_item_ent_add(item, "value_of", NULL, SEXP_string_newf((char *) value));
				xmlFree(value);
			}
		}
	}

	SEXP_list_add(pfd->item_list, item);

 cleanup:
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

SEXP_t *probe_main(SEXP_t *probe_in, int *err, void __attribute__((unused)) *arg)
{
	SEXP_t *path_ent, *filename_ent, *xpath_ent, *behaviors_ent;

	if (probe_in == NULL) {
		*err = PROBE_EINVAL;
		return NULL;
	}

	/* parse request */
	if ( (path_ent = probe_obj_getent(probe_in, "path", 1)) == NULL ||
	     (filename_ent = probe_obj_getent(probe_in, "filename", 1)) == NULL ||
	     (xpath_ent = probe_obj_getent(probe_in, "xpath", 1)) == NULL) {
		*err = PROBE_ENOELM;
		return NULL;
	}

	/* canonicalize behaviors */
	behaviors_ent = probe_obj_getent(probe_in, "behaviors", 1);
	if (behaviors_ent == NULL) {
		SEXP_t * behaviors_new;
		behaviors_new = probe_ent_creat("behaviors",
                                                probe_attr_creat("max_depth", SEXP_string_newf ("1"),
                                                                 "recurse_direction", SEXP_string_newf ("none"),
                                                                 NULL),
                                                NULL /* val */,
                                                NULL /* end */);
		behaviors_ent = SEXP_list_first(behaviors_new);
	}
	else {
		if (!probe_ent_attrexists (behaviors_ent, "max_depth"))
			probe_ent_attr_add (behaviors_ent,"max_depth", SEXP_string_newf ("1"));
		if (!probe_ent_attrexists (behaviors_ent, "recurse_direction"))
			probe_ent_attr_add (behaviors_ent,"recurse_direction", SEXP_string_newf ("none"));
	}

	int fcnt;
	struct pfdata pfd;

	pfd.xpath = SEXP_string_cstr(probe_ent_getval(xpath_ent));
	pfd.filename_ent = filename_ent;
	pfd.item_list = SEXP_list_new(NULL);

	fcnt = find_files(path_ent, filename_ent, behaviors_ent,
			  process_file, (void *) &pfd);
	if (fcnt == 0) {
		if (report_missing(pfd.filename_ent)) {
			SEXP_t *item;

			item = probe_item_creat("xmlfilecontent_item", NULL,
                                                /* entities */
                                                "path", NULL,
                                                probe_ent_getval(path_ent),
                                                NULL);
                        probe_item_setstatus(item, OVAL_STATUS_DOESNOTEXIST);
			probe_itement_setstatus(item, "path", 1, OVAL_STATUS_DOESNOTEXIST);
			SEXP_list_add(pfd.item_list, item);
		}
	} else if (fcnt < 0) {
		SEXP_t *item;

		item = probe_item_creat("xmlfilecontent_item", NULL,
                                        /* entities */
                                        "path", NULL,
                                        probe_ent_getval(path_ent),
                                        NULL);
                probe_item_setstatus(item, OVAL_STATUS_ERROR);
		SEXP_list_add(pfd.item_list, item);
	}

	*err = 0;
	return pfd.item_list;
}
#endif
