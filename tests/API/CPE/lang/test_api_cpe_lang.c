/*
 * Copyright 2008 Red Hat Inc., Durham, North Carolina.
 * All Rights Reserved.
 *
 * OpenScap CPE Lang Module Test Suite Helper
 *
 * Authors:
 *      Ondrej Moris <omoris@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <cpe_lang.h>
#include <cpe_name.h>
#include <oscap.h>

#define OSCAP_FOREACH_GENERIC(itype, vtype, val, init_val, code) \
    {                                                            \
        struct itype##_iterator *val##_iter = (init_val);        \
        vtype val;                                               \
        while (itype##_iterator_has_more(val##_iter)) {          \
            val = itype##_iterator_next(val##_iter);             \
            code                                                 \
        }                                                        \
        itype##_iterator_free(val##_iter);                       \
    }

#define OSCAP_FOREACH(type, val, init_val, code) \
        OSCAP_FOREACH_GENERIC(type, struct type *, val, init_val, code)

// Strings representing operators.
const char *CPE_OPER_STRS[] = { "", "AND", "OR", "" };

void print_usage(const char *, FILE *);

int print_expr_prefix_form(const struct cpe_testexpr *);

void print_platform(struct cpe_platform *);

int main (int argc, char *argv[])
{
	struct cpe_lang_model *lang_model = NULL;
	struct cpe_platform *platform = NULL, *new_platform = NULL;
	struct cpe_testexpr *testexpr = NULL;
	struct cpe_platform_iterator *platform_it = NULL;
	struct oscap_text_iterator *title_it = NULL;
	struct oscap_text *title = NULL;
	int ret_val = 0, i;

	struct oscap_source *source = oscap_source_new_from_file(argv[2]);

	if (argc == 2 && !strcmp(argv[1], "--help")) {
		print_usage(argv[0], stdout);
		ret_val = 0;
	}

	// Print complete content.
	else if (argc == 4 && !strcmp(argv[1], "--get-all")) {

		if ((lang_model = cpe_lang_model_import_source(source)) == NULL) {
			oscap_source_free(source);
			return 1;
		}

		//printf("%s:", cpe_lang_model_get_ns_href(lang_model));
		//printf("%s\n", cpe_lang_model_get_ns_prefix(lang_model));
		platform_it = cpe_lang_model_get_platforms(lang_model);
		while (cpe_platform_iterator_has_more(platform_it)) {
			print_platform(cpe_platform_iterator_next(platform_it));
		}
		cpe_platform_iterator_free(platform_it);
		cpe_lang_model_free(lang_model);
	}

	// Print platform of given key only.
	else if (argc == 5 && !strcmp(argv[1], "--get-key")) {

		if ((lang_model = cpe_lang_model_import_source(source)) == NULL) {
			oscap_source_free(source);
			return 1;
		}

		if ((platform = cpe_lang_model_get_item(lang_model, argv[4])) == NULL) {
			oscap_source_free(source);
			return 2;
		}

		print_platform(platform);

		cpe_lang_model_free(lang_model);
	}

	// Set ns_prefix, ns_href, add new platforms.
	else if (argc >= 6 && !strcmp(argv[1], "--set-all")) {
		if ((lang_model = cpe_lang_model_import_source(source)) == NULL) {
			oscap_source_free(source);
			return 1;
		}

		for (i = 6; i < argc; i++) {
			if ((new_platform =  cpe_platform_new()) == NULL) {
				oscap_source_free(source);
				return 1;
			}
			cpe_platform_set_id(new_platform, argv[i]);
			if (!cpe_lang_model_add_platform(lang_model, new_platform)) {
				oscap_source_free(source);
				return 2;
			}
		}

		cpe_lang_model_export(lang_model, argv[2]);
		cpe_lang_model_free(lang_model);
	}

	// Set id, change titles of platform of given key.
	else if (argc >= 6 && !strcmp(argv[1], "--set-key")) {
		if ((lang_model = cpe_lang_model_import_source(source)) == NULL) {
			oscap_source_free(source);
			return 1;
		}

		if ((platform = cpe_lang_model_get_item(lang_model, argv[4])) == NULL) {
			oscap_source_free(source);
			return 2;
		}

		if (strcmp(argv[5], "-"))
			cpe_platform_set_id(platform, argv[5]);

		i = 6;
		title_it = cpe_platform_get_titles(platform);
		while (i < argc && oscap_text_iterator_has_more(title_it)) {
			title = oscap_text_iterator_next(title_it);
			if (strcmp(argv[i], "-"))
				oscap_text_set_text(title, argv[i]);
			i++;
		}

		cpe_lang_model_export(lang_model, argv[2]);
		cpe_lang_model_free(lang_model);
	}

	// Create new content with new platforms.
	else if (argc >= 6 && !strcmp(argv[1], "--set-new")) {
		if ((lang_model = cpe_lang_model_new()) == NULL) {
			oscap_source_free(source);
			return 1;
		}

		for (i = 6; i < argc; i++) {
			if ((new_platform =  cpe_platform_new()) == NULL) {
				oscap_source_free(source);
				return 1;
			}
			cpe_platform_set_id(new_platform, argv[i]);
			/*
			   struct cpe_testexpr *expr = cpe_testexpr_new();
			   cpe_testexpr_set_oper(expr, CPE_LANG_OPER_MATCH);
			   cpe_testexpr_set_name(expr, cpe_name_new("cpe:/a:nevim"));
			   cpe_platform_set_expr(new_platform, expr);
			   */
			if (!cpe_lang_model_add_platform(lang_model, new_platform)) {
				oscap_source_free(source);
				return 2;
			}
		}

		cpe_lang_model_export(lang_model, argv[2]);
		cpe_lang_model_free(lang_model);
	}

	// Sanity checks.
	else if (argc == 2 && !strcmp(argv[1], "--smoke-test")) {

		if ((lang_model = cpe_lang_model_new()) == NULL) {
			oscap_source_free(source);
			return 1;
		} else {
			cpe_lang_model_free(lang_model);
		}

		if ((new_platform =  cpe_platform_new()) == NULL) {
			oscap_source_free(source);
			return 1;
		}
		else
			cpe_platform_free(new_platform);

		if ((testexpr = cpe_testexpr_new()) == NULL) {
			oscap_source_free(source);
			return 1;
		}
		else
			cpe_testexpr_free(testexpr);
	}

	else if (argc == 6 && !strcmp(argv[1], "--export-all")) {
		if ((lang_model = cpe_lang_model_import_source(source)) == NULL) {
			oscap_source_free(source);
			return 1;
		}

		cpe_lang_model_export(lang_model, argv[4]);
		cpe_lang_model_free(lang_model);
	}
	else {
		print_usage(argv[0], stderr);
		ret_val = 1;
	}

	oscap_source_free(source);
	oscap_cleanup();

	return ret_val;
}

// Print usage.
void print_usage(const char *program_name, FILE * out)
{
	fprintf(out,
		"Usage: \n\n"
		"  %s --export-all CPE_LANG_XML ENCODING CPE_EXPORT_XML ENCODING\n"
		"  %s --get-all CPE_LANG_XML ENCODING\n"
		"  %s --get-key CPE_LANG_XML ENCODING KEY\n"
		"  %s --set-all CPE_LANG_XML ENCODING NS_PREFIX NS_HREF (PLATFORM_ID)*\n"
		"  %s --set-key CPE_LANG_XML ENCODING KEY ID (TITLE)*\n"
		"  %s --set-new CPE_LANG_XML ENCODING NS_PREFIX NS_HREF (PLATFORM_ID)*\n"
		"  %s --smoke-test\n",
		program_name, program_name, program_name, program_name,
		program_name, program_name, program_name, program_name);
}

// Print expression in prefix form.
int print_expr_prefix_form(const struct cpe_testexpr *expr)
{
	//const struct cpe_testexpr *sub;

	putchar('(');

	if (cpe_testexpr_get_oper(expr) & CPE_LANG_OPER_NOT)
		printf("!");

	switch (cpe_testexpr_get_oper(expr) & CPE_LANG_OPER_MASK) {
	case CPE_LANG_OPER_AND:
	case CPE_LANG_OPER_OR:
		printf("%s",
		       CPE_OPER_STRS[cpe_testexpr_get_oper(expr) &
				     CPE_LANG_OPER_MASK]);
		//for (sub = cpe_testexpr_get_meta_expr(expr); cpe_testexpr_get_oper(sub); sub=cpe_testexpr_get_next(sub))
		OSCAP_FOREACH(cpe_testexpr, sub,
			      cpe_testexpr_get_meta_expr(expr),
			      print_expr_prefix_form(sub);
		    );
		break;
	case CPE_LANG_OPER_MATCH:
		printf("%s", cpe_name_get_as_str(cpe_testexpr_get_meta_cpe(expr)));
		break;
	default:
		return 1;
	}

	putchar(')');

	return 0;
}

// Print platform.
void print_platform(struct cpe_platform *platform)
{
	struct oscap_text *title = NULL;
	struct oscap_text_iterator *title_it = NULL;
	const char *remark, *id, *content, *language;

	id = cpe_platform_get_id(platform);
	printf("%s:", id == NULL ? "" : id);

	remark = cpe_platform_get_remark(platform);
	printf("%s:", remark == NULL ? "" : remark);

	title_it = cpe_platform_get_titles(platform);
	while (oscap_text_iterator_has_more(title_it)) {
		title = oscap_text_iterator_next(title_it);

		content = oscap_text_get_text(title);
		printf("%s.", content == NULL ? "" : content);

		language = oscap_text_get_lang(title);
		printf("%s,", language == NULL ? "" : language);
	}
	putchar(':');
	oscap_text_iterator_free(title_it);
	print_expr_prefix_form(cpe_platform_get_expr(platform));
	putchar('\n');
}
