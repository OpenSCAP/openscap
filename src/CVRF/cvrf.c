
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "public/cvrf.h"
#include "cvrf_priv.h"

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

	struct cvrf_model *cvrf;
	cvrf = cvrf_model_parse_xml(source);

	return cvrf;
}

/**
 * Public function to export CVRF model to OSCAP export target.
 */
void cvrf_model_export(struct cvrf_model *cvrf, struct oscap_source *export_source)
{

	__attribute__nonnull__(export_source);

	if (export_source == NULL)
		return;

	cvrf_model_export_xml(cvrf, export_source);
}

void cvrf_index_export(struct cvrf_index *index, struct oscap_source *export_source) {

	__attribute__nonnull__(export_source);

	if (export_source == NULL)
		return;

	cvrf_index_export_xml(index, export_source);
}


const char * cvrf_model_supported(void)
{
        return CVRF_SUPPORTED;
}
