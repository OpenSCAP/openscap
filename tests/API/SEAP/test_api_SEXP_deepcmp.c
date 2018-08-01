
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sexp.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include "_sexp-manip.h"
#include "_sexp-output.h"

int main (void)
{
	SEXP_t *cmp_l1, *cmp_l2, *cmp_l3, *cmp_l4;

	setbuf (stdout, NULL);

#define LIST1(dst, a, b, c, d, e, f, g, h, i, j, k) do {	\
		SEXP_t *l2, *l3, *v1, *v2, *v3, *v4, *v5;	\
		SEXP_t *v6, *v7, *v8, *v9, *v10, *v11;		\
								\
		dst = SEXP_list_new(NULL);			\
								\
		v1 = SEXP_number_newu(a);			\
		v2 = SEXP_number_newu(b);			\
		v3 = SEXP_number_newu(c);			\
		v4 = SEXP_string_newf(d);			\
		v5 = SEXP_string_newf(e);			\
		v6 = SEXP_string_newf(f);			\
		v7 = SEXP_string_newf(g);			\
		v8 = SEXP_number_newu(h);			\
		v9 = SEXP_number_newu(i);			\
		v10 = SEXP_number_newu(j);			\
		v11 = SEXP_number_newu(k);			\
								\
		SEXP_list_add(dst, v1);				\
		SEXP_list_add(dst, v2);				\
		SEXP_list_add(dst, v3);				\
								\
		l2 = SEXP_list_new(v4, v5, v6, v7, NULL);	\
								\
		SEXP_list_add(dst, l2);				\
		SEXP_free(l2);					\
								\
		l2 = SEXP_list_new(v8, v9, v10, NULL);		\
		l3 = SEXP_list_new(v11, NULL);			\
								\
		SEXP_list_add(l2, l3);				\
		SEXP_list_add(dst, l2);				\
								\
		SEXP_free(l2);					\
		SEXP_free(l3);					\
								\
		SEXP_free(v1);						\
		SEXP_free(v2);						\
		SEXP_free(v3);						\
		SEXP_free(v4);						\
		SEXP_free(v5);						\
		SEXP_free(v6);						\
		SEXP_free(v7);						\
		SEXP_free(v8);						\
		SEXP_free(v9);						\
		SEXP_free(v10);						\
		SEXP_free(v11);						\
		printf("%s = ", #dst);				\
		SEXP_fprintfa(stdout, dst);			\
		printf("\n");					\
	} while(0)

#define LIST2(dst, a, b, c, d, e, f, g, h, i, j, k) do {	\
		SEXP_t *l2, *v1, *v2, *v3, *v4, *v5;		\
		SEXP_t *v6, *v7, *v8, *v9, *v10, *v11;		\
								\
		dst = SEXP_list_new(NULL);			\
								\
		v1 = SEXP_number_newu(a);			\
		v2 = SEXP_number_newu(b);			\
		v3 = SEXP_number_newu(c);			\
		v4 = SEXP_string_newf(d);			\
		v5 = SEXP_string_newf(e);			\
		v6 = SEXP_string_newf(f);			\
		v7 = SEXP_string_newf(g);			\
		v8 = SEXP_number_newu(h);			\
		v9 = SEXP_number_newu(i);			\
		v10 = SEXP_number_newu(j);			\
		v11 = SEXP_number_newu(k);			\
								\
		SEXP_list_add(dst, v1);				\
		SEXP_list_add(dst, v2);				\
		SEXP_list_add(dst, v3);				\
								\
		l2 = SEXP_list_new(v4, v5, v6, v7, NULL);	\
								\
		SEXP_list_add(dst, l2);				\
		SEXP_free(l2);					\
								\
		l2 = SEXP_list_new(v8, v9, v10, NULL);		\
		SEXP_list_add(dst, v11);			\
		SEXP_list_add(dst, l2);				\
								\
		SEXP_free(l2);					\
								\
		SEXP_free(v1);						\
		SEXP_free(v2);						\
		SEXP_free(v3);						\
		SEXP_free(v4);						\
		SEXP_free(v5);						\
		SEXP_free(v6);						\
		SEXP_free(v7);						\
		SEXP_free(v8);						\
		SEXP_free(v9);						\
		SEXP_free(v10);						\
		SEXP_free(v11);						\
		printf("%s = ", #dst);				\
		SEXP_fprintfa(stdout, dst);			\
		printf("\n");					\
	} while(0)

#define BOOLEXP(e) ((e) == true ? "true" : "false")
#define TESTCMP(a1, a2, r)						\
	do {								\
		bool __res;						\
		printf("%s ?= %s (expecting: %s)\n", #a1, #a2, #r);	\
		printf("=> %s\n", BOOLEXP(__res = SEXP_deepcmp(a1, a2))); \
		if (__res != r)						\
			return (1);					\
	} while(0)

	LIST1(cmp_l1,
	      1, 2, 3, "a", "ab", "abc", "abcd", 4, 5, 6, 7);
	LIST1(cmp_l2,
	      1, 2, 3, "a", "ab", "abc", "abcd", 4, 5, 6, 8);
	LIST2(cmp_l3,
	      1, 2, 3, "a", "ab", "abc", "abcd", 4, 5, 6, 8);
	LIST2(cmp_l4,
	      1, 2, 3, "a", "ab", "abc", "abcd", 4, 5, 6, 8);

	TESTCMP(NULL,   NULL,   true);
	TESTCMP(cmp_l1, NULL,   false);
	TESTCMP(NULL,   cmp_l1, false);
	TESTCMP(cmp_l1, cmp_l1, true);
	TESTCMP(cmp_l2, cmp_l2, true);
	TESTCMP(cmp_l1, cmp_l2, false);
	TESTCMP(cmp_l2, cmp_l1, false);
	TESTCMP(cmp_l2, cmp_l3, false);
	TESTCMP(cmp_l3, cmp_l2, false);
	TESTCMP(cmp_l1, cmp_l3, false);
	TESTCMP(cmp_l3, cmp_l1, false);
	TESTCMP(cmp_l3, cmp_l3, true);
	TESTCMP(cmp_l4, cmp_l4, true);
	TESTCMP(cmp_l4, cmp_l3, true);
	TESTCMP(cmp_l3, cmp_l4, true);

	SEXP_free(cmp_l1);
	SEXP_free(cmp_l2);
	SEXP_free(cmp_l3);
	SEXP_free(cmp_l4);

	return (0);
}
