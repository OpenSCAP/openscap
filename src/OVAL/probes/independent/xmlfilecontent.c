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
#include <probe.h>
#include <findfile.h>

#define FILE_SEPARATOR '/'

struct pfdata {
	SEXP_t *filename_elm;
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

static int report_missing(SEXP_t *elm)
{
	oval_operation_enum op;

	op = SEXP_number_getd(SEXP_OVALelm_getattrval(elm, "operation"));
	if (op == OPERATION_EQUALS)
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
	SEXP_t *attrs, *item;

	if (filename == NULL) {
		if (report_missing(pfd->filename_elm)) {
			attrs = SEXP_OVALattr_create("id", SEXP_number_newd(-1), // todo: id
						     "status", SEXP_number_newd(OVAL_STATUS_DOESNOTEXIST),
						     NULL);
			item = SEXP_OVALobj_create("xmlfilecontent_item",
						   attrs,
						   "path", NULL,
						   SEXP_string_newf(path),
						   "filename", NULL,
						   SEXP_string_newf(filename),
						   NULL);
			SEXP_OVALobj_setelmstatus(item, "path", 1, OVAL_STATUS_EXISTS);
			SEXP_OVALobj_setelmstatus(item, "filename", 1, OVAL_STATUS_DOESNOTEXIST);
		} else {
			attrs = SEXP_OVALattr_create("id", SEXP_number_newd(-1), // todo: id
						     "status", SEXP_number_newd(OVAL_STATUS_EXISTS),
						     NULL);
			item = SEXP_OVALobj_create("xmlfilecontent_item",
						   attrs,
						   "path", NULL,
						   SEXP_string_newf(path),
						   NULL);
			SEXP_OVALobj_setelmstatus(item, "path", 1, OVAL_STATUS_EXISTS);
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

	attrs = SEXP_OVALattr_create("id", SEXP_number_newd(-1), // todo: id
				     NULL);
	item = SEXP_OVALobj_create("xmlfilecontent_item",
				   attrs,
				   "path", NULL,
				   SEXP_string_newf(path),
				   "filename", NULL,
				   SEXP_string_newf(filename),
				   "xpath", NULL,
				   SEXP_string_newf(pfd->xpath),
				   NULL);
	SEXP_OVALobj_setelmstatus(item, "path", 1, OVAL_STATUS_EXISTS);
	SEXP_OVALobj_setelmstatus(item, "filename", 1, OVAL_STATUS_EXISTS);
	SEXP_OVALobj_setelmstatus(item, "xpath", 1, OVAL_STATUS_EXISTS);

	node_cnt = nodes->nodeNr;
	if (node_cnt == 0) {
		SEXP_OVALobj_setstatus(item, OVAL_STATUS_DOESNOTEXIST);
		SEXP_OVALobj_elm_add(item, "value_of", NULL, SEXP_string_newf(""));
		SEXP_OVALobj_setelmstatus(item, "value_of", 1, OVAL_STATUS_DOESNOTEXIST);
	} else {
		SEXP_OVALobj_setstatus(item, OVAL_STATUS_EXISTS);
		node_tab = nodes->nodeTab;
		for (i = 0; i < node_cnt; ++i) {
			cur_node = nodes->nodeTab[i];
			if (cur_node->type == XML_ATTRIBUTE_NODE ||
			    cur_node->type == XML_TEXT_NODE) {
				value = xmlNodeGetContent(cur_node);
				SEXP_OVALobj_elm_add(item, "value_of", NULL, SEXP_string_newf((char *) value));
				SEXP_OVALobj_setelmstatus(item, "value_of", i + 1, OVAL_STATUS_EXISTS);
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
	SEXP_t *path_elm, *filename_elm, *xpath_elm, *behaviors_elm;

	if (probe_in == NULL) {
		*err = PROBE_EINVAL;
		return NULL;
	}

	/* parse request */
	if ( (path_elm = SEXP_OVALobj_getelm(probe_in, "path", 1)) == NULL ||
	     (filename_elm = SEXP_OVALobj_getelm(probe_in, "filename", 1)) == NULL ||
	     (xpath_elm = SEXP_OVALobj_getelm(probe_in, "xpath", 1)) == NULL) {
		*err = PROBE_ENOELM;
		return NULL;
	}
	behaviors_elm = SEXP_OVALobj_getelm(probe_in, "behaviors", 1);

	int fcnt;
	struct pfdata pfd;

	pfd.xpath = SEXP_string_cstr(SEXP_OVALelm_getval(xpath_elm, 1));
	pfd.filename_elm = filename_elm;
	pfd.item_list = SEXP_list_new();

	fcnt = find_files(path_elm, filename_elm, behaviors_elm,
			  process_file, (void *) &pfd);
	if (fcnt == 0) {
		if (report_missing(pfd.filename_elm)) {
			SEXP_t *item, *attrs;
			attrs = SEXP_OVALattr_create("id", SEXP_number_newd(-1), // todo: id
						     "status", SEXP_number_newd(OVAL_STATUS_DOESNOTEXIST),
						     NULL);
			item = SEXP_OVALobj_create("xmlfilecontent_item",
						   attrs,
						   "path", NULL,
						   SEXP_OVALelm_getval(path_elm, 1),
						   NULL);
			SEXP_OVALobj_setelmstatus(item, "path", 1, OVAL_STATUS_DOESNOTEXIST);
			SEXP_list_add(pfd.item_list, item);
		}
	} else if (fcnt < 0) {
		SEXP_t *item, *attrs;
		attrs = SEXP_OVALattr_create("id", SEXP_number_newd(-1), // todo: id
					     "status", SEXP_number_newd(OVAL_STATUS_ERROR),
					     NULL);
		item = SEXP_OVALobj_create("xmlfilecontent_item",
					   attrs,
					   "path", NULL,
					   SEXP_OVALelm_getval(path_elm, 1),
					   NULL);
		SEXP_list_add(pfd.item_list, item);
	}

	*err = 0;
	return pfd.item_list;
}
