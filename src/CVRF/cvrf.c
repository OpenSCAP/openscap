
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "public/cvrf.h"
#include "cvrf_priv.h"

#include "common/util.h"
#include "common/list.h"

#define CVRF_SUPPORTED "1.1"



struct cvrf_index *cvrf_index_import(const char *index_file) {

	__attribute__nonnull__(index_file);

	if (index_file == NULL)
		return NULL;

	struct cvrf_index *index;
	index = cvrf_index_parse_xml(index_file);

	return index;
}

/**
 * Public function to import CVRF model from OSCAP import source.
 * Function returns CVRF model, need to free source after calling this function
 */
struct cvrf_model *cvrf_model_import(const char *file)
{

	__attribute__nonnull__(file);

	if (file == NULL)
		return NULL;

	struct cvrf_model *cvrf;
	cvrf = cvrf_model_parse_xml(file);

	return cvrf;
}

/**
 * Public function to export CVRF model to OSCAP export target.
 */
void cvrf_model_export(struct cvrf_model *cvrf, const char *file)
{

	__attribute__nonnull__(file);

	if (file == NULL)
		return;

	cvrf_model_export_xml(cvrf, file);
}

void cvrf_index_export(struct cvrf_index *index, const char *file) {

	__attribute__nonnull__(file);

	if (file == NULL)
		return;

	cvrf_index_export_xml(index, file);
}


const char * cvrf_model_supported(void)
{
        return CVRF_SUPPORTED;
}
