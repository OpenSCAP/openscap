#include <stdio.h>
#include <cpelang.h>

void test_platform_dump(struct cpe_platform * plat);
//void test_langexpr_dump(struct cpe_lang_expr * expr, int depth);

void test_platform_dump(struct cpe_platform * plat)
{
	// print id, title, remark
	/*printf("ID: %s\n  Title:  %s\n  Remark: %s\n",
	       cpe_platform_get_id(plat), cpe_platform_get_title(plat), cpe_platform_get_remark(plat));*/
	// dump expression
	//test_langexpr_dump(&(plat->expr), 4);
}

// strings representing operators
const char *CPE_OPER_STRS[] = { " END", " AND", " OR", "" };

/*
void test_langexpr_dump(cpe_lang_expr_t * expr, int depth)
{
	int i;
	cpe_lang_expr_t *sub;

	// indentation
	for (i = 0; i < depth; ++i)
		printf(" ");
	printf("+");
	// print operand
	if (expr->oper & CPE_LANG_OPER_NOT)
		printf("N");
	printf("%s ", CPE_OPER_STRS[expr->oper & CPE_LANG_OPER_MASK]);

	// print arguments
	switch (expr->oper & CPE_LANG_OPER_MASK) {
	case CPE_LANG_OPER_AND:
	case CPE_LANG_OPER_OR:
		printf("\n");
		// print boolean operation subexpressions
		for (sub = expr->meta.expr; sub->oper; ++sub)
			test_langexpr_dump(sub, depth + 2);
		break;
	case CPE_LANG_OPER_MATCH:
		// print CPE to match against
		cpe_write(expr->meta.cpe, stdout);
		printf("\n");
		break;
	default:
		// unknown CPE language operator - program execution should never reach this
		printf("ERROR - unknown operator!!!\n");
		break;
	}
}
*/

void test_lang_model_export(struct cpe_lang_model * res){

	struct oscap_export_target *tgt = oscap_export_target_new("test_cpelang.out", NULL);
        cpe_lang_model_export(res, tgt);
	//oscap_export_target_free(tgt);
    fprintf(stdout, "Result saved in test_cpelang.out file\n");
}

#ifndef CPELANG_TEST_NO_MAIN


int main(int argc, char **argv)
{
	struct cpe_lang_model *plat;	// pointer to our platform specification
	unsigned int i;
	struct cpe_name **cpes;		// list of CPEs
	size_t cpes_n;		// number of CPEs

	if (argc < 2) {
		// not enough arguments
		fprintf(stderr, "Usage: %s filename.xml [CPEs...]\n", argv[0]);
		return EXIT_FAILURE;
	}
	// allocate space for list of CPEs
	cpes_n = argc - 2;
	cpes = malloc(cpes_n * sizeof(struct cpe_name *));

	// build list of CPEs from the command line
	for (i = 0; i < cpes_n; ++i)
		cpes[i] = cpe_name_new(argv[i + 2]);

	// load platform specification from file specified on the command line
	struct oscap_import_source *src = oscap_import_source_new(argv[1], NULL);
	plat = cpe_lang_model_import(src);
	oscap_import_source_free(src);

	if (plat == NULL) {
		fprintf(stderr, "ERROR: Could not load language model from '%s'.\n", argv[1]);
		return EXIT_FAILURE;
	}


	// modify it a bit
	struct cpe_platform *platform = cpe_lang_model_get_item(plat, "789");
	if (platform) {
		printf("Old ID: %s\n", cpe_platform_get_id(platform));
		cpe_platform_set_id(platform, "WOW");
		printf("New ID: %s\n", cpe_platform_get_id(platform));
	}


	/*if (plat != NULL) {
		OSCAP_FOREACH (cpe_platform, p, cpe_lang_model_get_platforms(plat),
			// dump its contents
			test_platform_dump(p);
			// print whether list of CPEs matched
			printf("    = %sMATCH\n\n", cpe_platform_match_cpe(cpes, cpes_n, p) ? "" : "MIS");
		)
	}*/
        //printf("RES_PREFIX:%s", plat->ns_prefix);
    test_lang_model_export(plat);
	// free resources allocated for platform specification
	//cpe_lang_model_free(plat);
	if (plat == NULL) return EXIT_FAILURE;

	// free list of CPEs
	for (i = 0; i < cpes_n; ++i)
		cpe_name_free(cpes[i]);
	free(cpes);

	oscap_cleanup(); // clean caches

	return EXIT_SUCCESS;
}

#endif
