#include <stdio.h>
#include <cpelang.h>

void test_platform_dump(cpe_platform_t * plat);
void test_langexpr_dump(cpe_lang_expr_t * expr, int depth);

void test_platformspec_dump(cpe_platform_spec_t * plat)
{
	int i;
	// dump all platforms belonging to a platform specification
	for (i = 0; i < plat->platforms_n; ++i)
		test_platform_dump(plat->platforms[i]);
}

void test_platform_dump(cpe_platform_t * plat)
{
	// print id, title, remark
	printf("ID: %s\n  Title:  %s\n  Remark: %s\n  Expr:\n",
	       plat->id, plat->title, plat->remark);
	// dump expression
	test_langexpr_dump(&(plat->expr), 4);
}

// strings representing operators
const char *CPE_OPER_STRS[] = { " END", " AND", " OR", "" };

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

#ifndef CPELANG_TEST_NO_MAIN

int main(int argc, char **argv)
{
	cpe_platform_spec_t *plat;	// pointer to our platform specification
	int i;
	cpe_t **cpes;		// list of CPEs
	size_t cpes_n;		// number of CPEs

	if (argc < 2) {
		// not enough arguments
		fprintf(stderr, "Usage: %s filename.xml [CPEs...]\n", argv[0]);
		return EXIT_FAILURE;
	}
	// allocate space for list of CPEs
	cpes_n = argc - 2;
	cpes = malloc(cpes_n * sizeof(cpe_t *));

	// build list of CPEs from the command line
	for (i = 0; i < cpes_n; ++i)
		cpes[i] = cpe_new(argv[i + 2]);

	// load platform specification from file specified on the command line
	plat = cpe_platformspec_new(argv[1]);

	if (plat != NULL) {
		// for each platform in platform specification
		for (i = 0; i < plat->platforms_n; ++i) {
			// dump its contents
			test_platform_dump(plat->platforms[i]);
			// print whether list of CPEs matched
			printf("    = %sMATCH\n\n",
			       cpe_language_match_cpe(cpes, cpes_n,
						      plat->
						      platforms[i]) ? "" :
			       "MIS");
		}
	}
	// free resources allocated for platform specification
	cpe_platformspec_delete(plat);

	// free list of CPEs
	for (i = 0; i < cpes_n; ++i)
		cpe_delete(cpes[i]);
	free(cpes);

	return EXIT_SUCCESS;
}

#endif
