
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "public/cvrf.h"
#include "cvrf_priv.h"

#include "common/util.h"
#include "common/list.h"

#define CVRF_SUPPORTED "1.1"

// use oscap_source to get the xmlTextReaderPtr
//oscap_source_get_xmlTextReader(source)

/**
 * Public function to import CVRF model from OSCAP import source.
 * Function returns CVRF model, need to free source after calling this function
 */
struct cvrf_model *cve_model_import(const char *file)
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


const char * cvrf_model_supported(void)
{
        return CVRF_SUPPORTED;
}
