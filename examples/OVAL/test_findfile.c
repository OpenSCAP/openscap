
#include <findfile.h>
#include <seap.h>
#include <probe.h>

int cb (const char *pathname, const char *filename, void *arg) {
	printf("pathname: %s, filename: %s\n", pathname, filename);
}


int main(int argc, char *argv[]) {
        SEXP_t *elm_list, *elm1, *elm2, *elm3;
	int rc;

        int number = -1;
        elm_list = SEXP_OVALelm_create( "path",
                                        SEXP_OVALattr_create("operation",SEXP_string_newf("pattern match"),NULL),
                                        SEXP_string_newf(argv[1]),

                                        "filename",
                                        SEXP_OVALattr_create("operation",SEXP_string_newf("pattern match"),NULL),
                                        SEXP_string_newf(argv[2]),

					"behaviors",
                                        SEXP_OVALattr_create("max_depth",SEXP_number_new(&number, NUM_INT32),
                                                            "recurse",SEXP_string_newf("both"),
                                                            "recurse_direction",SEXP_string_newf("down"),
                                                            "recurse_file_system",SEXP_string_newf("all"),
                                                             NULL),
                                        NULL,

                                        NULL);

        elm1 = SEXP_list_nth(elm_list,1);
        elm2 = SEXP_list_nth(elm_list,2);
        elm3 = SEXP_list_nth(elm_list,3);

	rc = find_files(elm1, elm2, elm3, &cb, NULL);
	printf("finds: %d", rc);


        return 0;
}

