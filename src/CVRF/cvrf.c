
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "public/cvrf.h"
#include "cvrf_priv.h"

#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/parser.h>
#include <libxml/xpathInternals.h>

#include "common/_error.h"
#include "common/util.h"
#include "common/list.h"

#define CVRF_SUPPORTED "1.1"



struct cvrf_index *cvrf_index_import(struct oscap_source *index_source) {

	__attribute__nonnull__(index_source);

	if (index_source == NULL)
		return NULL;

	struct cvrf_index *index;
	index = cvrf_index_parse_xml(index_source);

	return index;
}

/**
 * Public function to import CVRF model from OSCAP import source.
 * Function returns CVRF model, need to free source after calling this function
 */
struct cvrf_model *cvrf_model_import(struct oscap_source *source)
{

	__attribute__nonnull__(source);

	if (source == NULL)
		return NULL;

	struct xmlTextReaderPtr *reader = oscap_source_get_xmlTextReader(source);
	if (!reader) {
		oscap_source_free(source);
		return NULL;
	}

	int rc = xmlTextReaderNextNode(reader);
	if (rc == -1) {
		xmlFreeTextReader(reader);
		oscap_source_free(source);
		return NULL;
	}

	struct cvrf_model *cvrf = cvrf_model_parse(reader);

	xmlFreeTextReader(reader);
	oscap_source_free(source);
	return cvrf;
}


/**
 * Public function to export CVRF model to OSCAP export target.
 */

int cvrf_model_export(struct cvrf_model *cvrf, const char *export_file) {
	__attribute__nonnull__(export_file);

	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.1");
	if (doc == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return NULL;
	}

	cvrf_model_to_dom(cvrf, doc, NULL, NULL);
	struct oscap_source *source = oscap_source_new_from_xmlDoc(doc, export_file);
	if (source == NULL) {
		return -1;
	}

	int ret = oscap_source_save_as(source, NULL);
	oscap_source_free(source);
	return ret;
}

/*
void cvrf_index_export(struct cvrf_index *index, struct oscap_source *export_source) {

	__attribute__nonnull__(export_source);

	if (export_source == NULL)
		return;

	cvrf_index_export_xml(index, export_source);
}
*/

const char * cvrf_model_supported(void)
{
        return CVRF_SUPPORTED;
}
