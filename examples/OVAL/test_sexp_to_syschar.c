#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "api/oval_agent_api.h"
#include "seap.h"
#include "oval_probe.h"
#include "probes/probe.h"
#include "oval_system_characteristics_impl.h"

#include <stdarg.h>

SEXP_t* SEXP_path(SEXP_t* sexp, unsigned nth, ...)
{
	va_list args;
	va_start(args, nth);

	while (nth != 0 && sexp != NULL) {
		sexp = SEXP_list_nth(sexp, nth);
		nth = va_arg(args, unsigned);
	}

	va_end(args);
	return sexp;
}


int main(int argc, char **argv)
{

	SEXP_t *probe_out = SEXP_list_new();
	SEXP_t *item_sexp = SEXP_OVALobj_create (
		"rpminfo_item",    
			//SEXP_OVALattr_create("nevimco", SEXP_string_newf("rtfm"), NULL),
			//SEXP_OVALattr_create(NULL),
			NULL,
		"name",            NULL, SEXP_string_newf("foo-package"),
		"arch",            NULL, SEXP_string_newf("i686"),
		"epoch",           NULL, SEXP_string_newf("0"),
		"release",         NULL, SEXP_string_newf("1.fc9"),
		"version",         NULL, SEXP_string_newf("2.3.8"),
		"evr",             NULL, SEXP_string_newf("0:2.3.8-1.fc9"),
		"signature_keyid", NULL, SEXP_string_newf("ABCD1234"),
		"some_number",     NULL, SEXP_number_newd(123),
		NULL
	);
	SEXP_OVALobj_setstatus(item_sexp, OVAL_STATUS_EXISTS);
	SEXP_OVALobj_setstatus(SEXP_OVALobj_getelm(item_sexp, "evr", 1), OVAL_STATUS_ERROR);
	SEXP_OVALelm_setdatatype(SEXP_OVALobj_getelm(item_sexp, "version", 1), 1, OVAL_DATATYPE_VERSION);
	SEXP_OVALelm_setmask(SEXP_OVALobj_getelm(item_sexp, "version", 1), true);
	SEXP_list_add(probe_out, item_sexp);

	SEXP_printfa(probe_out);
	printf("\n\n");

	struct oval_syschar* syschar = oval_syschar_new(NULL);

	oval_syschar_apply_sexp(syschar, probe_out, NULL);

	oval_syschar_to_print(syschar, "", 0);
	printf("\n");

	oval_syschar_free(syschar);

	SEXP_free(probe_out);
        
	return 0;
}

