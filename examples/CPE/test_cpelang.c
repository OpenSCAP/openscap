#include <stdio.h>
#include <cpelang.h>

void test_platform_dump(struct cpe_platform * plat);
//void test_langexpr_dump(struct cpe_lang_expr * expr, int depth);

void test_platformspec_dump(struct cpe_platformspec * plat)
{
	// dump all platforms belonging to a platform specification
	struct cpe_platform_iterator* it = cpe_platformspec_get_items(plat);
	while (cpe_platform_iterator_has_more(it))
		test_platform_dump(cpe_platform_iterator_next(it));
}

void test_platform_dump(struct cpe_platform * plat)
{
	// print id, title, remark
	printf("ID: %s\n  Title:  %s\n  Remark: %s\n",
	       cpe_platform_get_id(plat), cpe_platform_get_title(plat), cpe_platform_get_remark(plat));
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

#ifndef CPELANG_TEST_NO_MAIN


void cpe_platform_iterator_free(struct cpe_platform_iterator* it){}

int main(int argc, char **argv)
{
	struct cpe_platformspec *plat;	// pointer to our platform specification
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
	plat = cpe_platformspec_new(argv[1]);

	if (plat != NULL) {
		// for each platform in platform specification
		/*
		struct cpe_platform_iterator* it = cpe_platformspec_items(plat);
		while (cpe_platform_iterator_has_more(it)) {
			struct cpe_platform* p = cpe_platform_iterator_next(it);
			// dump its contents
			test_platform_dump(p);
			// print whether list of CPEs matched
			printf("    = %sMATCH\n\n", cpe_platform_match_cpe(cpes, cpes_n, p) ? "" : "MIS");
		}
		cpe_platform_iterator_free(it);
		*/
		OSCAP_FOREACH (cpe_platform, p, cpe_platformspec_get_items(plat),
			// dump its contents
			test_platform_dump(p);
			// print whether list of CPEs matched
			printf("    = %sMATCH\n\n", cpe_platform_match_cpe(cpes, cpes_n, p) ? "" : "MIS");
		)
	}
	// free resources allocated for platform specification
	cpe_platformspec_free(plat);
	if (plat == NULL) return EXIT_FAILURE;

	// free list of CPEs
	for (i = 0; i < cpes_n; ++i)
		cpe_name_free(cpes[i]);
	free(cpes);

	oscap_cleanup(); // clean caches

	return EXIT_SUCCESS;
}

#endif
