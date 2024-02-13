/*
 * Copyright 2009--2013 Red Hat Inc., Durham, North Carolina.
 * All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Authors:
 *      "Peter Vrabec" <pvrabec@redhat.com>
 *      Šimon Lukašík
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "oval_cmp_evr_string_impl.h"
#include "oval_definitions.h"
#include "oval_types.h"

#include "common/_error.h"

#ifdef HAVE_RPMVERCMP
#ifdef RPM418_FOUND
#include <rpm/rpmver.h>
#include <rpm/rpmstring.h>
#else
#include <rpm/rpmlib.h>
#endif
#else
#ifdef OS_WINDOWS
#include <malloc.h>
#elif !defined(OS_FREEBSD)
#include <alloca.h>
#endif
static int rpmvercmp(const char *a, const char *b);
static int risdigit(int c) {
	// locale independent
	return (c >= '0' && c <= '9');
}
#endif

static inline int rpmevrcmp(const char *a, const char *b);
static int compare_values(const char *str1, const char *str2);
static void parseEVR(char *evr, const char **ep, const char **vp, const char **rp);

oval_result_t oval_evr_string_cmp(const char *state, const char *sys, oval_operation_t operation)
{
	if (state == NULL || sys == NULL) {
		return OVAL_RESULT_ERROR;
	}
	int result = rpmevrcmp(sys, state);

	if (operation == OVAL_OPERATION_EQUALS) {
		return ((result == 0) ? OVAL_RESULT_TRUE : OVAL_RESULT_FALSE);
	} else if (operation == OVAL_OPERATION_NOT_EQUAL) {
		return ((result != 0) ? OVAL_RESULT_TRUE : OVAL_RESULT_FALSE);
	} else if (operation == OVAL_OPERATION_GREATER_THAN) {
		return ((result == 1) ? OVAL_RESULT_TRUE : OVAL_RESULT_FALSE);
	} else if (operation == OVAL_OPERATION_GREATER_THAN_OR_EQUAL) {
		return ((result != -1) ? OVAL_RESULT_TRUE : OVAL_RESULT_FALSE);
	} else if (operation == OVAL_OPERATION_LESS_THAN) {
		return ((result == -1) ? OVAL_RESULT_TRUE : OVAL_RESULT_FALSE);
	} else if (operation == OVAL_OPERATION_LESS_THAN_OR_EQUAL) {
		return ((result != 1) ? OVAL_RESULT_TRUE : OVAL_RESULT_FALSE);
	}

	oscap_seterr(OSCAP_EFAMILY_OVAL, "Invalid type of operation in rpm version comparison: %d.", operation);
	return OVAL_RESULT_ERROR;
}

static inline int rpmevrcmp(const char *a, const char *b)
{
	/* This mimics rpmevrcmp which is not exported by rpmlib version 4.
	 * Code inspired by rpm.labelCompare() from rpm4/python/header-py.c
	 */
	const char *a_epoch = NULL, *a_version = NULL, *a_release = NULL;
	const char *b_epoch = NULL, *b_version = NULL, *b_release = NULL;
	char *a_copy, *b_copy;
	int result;

	a_copy = oscap_strdup(a);
	b_copy = oscap_strdup(b);
	parseEVR(a_copy, &a_epoch, &a_version, &a_release);
	parseEVR(b_copy, &b_epoch, &b_version, &b_release);

	result = compare_values(a_epoch, b_epoch);
	if (!result) {
		result = compare_values(a_version, b_version);
		if (!result)
			result = compare_values(a_release, b_release);
	}

	free(a_copy);
	free(b_copy);
	return result;
}

static int compare_values(const char *str1, const char *str2)
{
	/*
	 * Code copied from rpm4/python/header-py.c
	 */
	if (!str1 && !str2)
		return 0;
	else if (str1 && !str2)
		return 1;
	else if (!str1 && str2)
		return -1;
	return rpmvercmp(str1, str2);
}

static void parseEVR(char *evr, const char **ep, const char **vp, const char **rp)
{
	/*
	 * Code copied from rpm4/lib/rpmds.c
	 */
	const char *epoch;
	const char *version;			/* assume only version is present */
	const char *release;
	char *s, *se;

	if (!evr)
		return;

	s = evr;
	while (*s && risdigit(*s)) s++;		/* s points to epoch terminator */
	se = strrchr(s, '-');			/* se points to version terminator */

	if (*s == ':') {
		epoch = evr;
		*s++ = '\0';
		version = s;
		if (*epoch == '\0')
			epoch = "0";
	} else {
		epoch = NULL;			/* XXX disable epoch compare if missing */
		version = evr;
	}
	if (se) {
		*se++ = '\0';
		release = se;
	} else {
		release = NULL;
	}

	if (ep) *ep = epoch;
	if (vp) *vp = version;
	if (rp) *rp = release;
}

#ifndef HAVE_RPMVERCMP
/*
 * code from http://rpm.org/api/4.4.2.2/rpmvercmp_8c-source.html
 */

/* compare alpha and numeric segments of two versions */
/* return 1: a is newer than b */
/*        0: a and b are the same version */
/*       -1: b is newer than a */
static int rpmvercmp(const char *a, const char *b)
{
	char oldch1, oldch2;
	char *str1, *str2;
	char *one, *two;
	int rc;
	int isnum;

	/* easy comparison to see if versions are identical */
	if (!strcmp(a, b))
		return 0;

	/* TODO: make new malloca */
	str1 = alloca(strlen(a) + 1);
	str2 = alloca(strlen(b) + 1);

	strcpy(str1, a);
	strcpy(str2, b);

	one = str1;
	two = str2;

	/* loop through each version segment of str1 and str2 and compare them */
	while (*one && *two) {
		while (*one && !isalnum(*one))
			one++;
		while (*two && !isalnum(*two))
			two++;

		/* If we ran to the end of either, we are finished with the loop */
		if (!(*one && *two))
			break;

		str1 = one;
		str2 = two;

		/* grab first completely alpha or completely numeric segment */
		/* leave one and two pointing to the start of the alpha or numeric */
		/* segment and walk str1 and str2 to end of segment */
		if (isdigit(*str1)) {
			while (*str1 && isdigit(*str1))
				str1++;
			while (*str2 && isdigit(*str2))
				str2++;
			isnum = 1;
		} else {
			while (*str1 && isalpha(*str1))
				str1++;
			while (*str2 && isalpha(*str2))
				str2++;
			isnum = 0;
		}

		/* save character at the end of the alpha or numeric segment */
		/* so that they can be restored after the comparison */
		oldch1 = *str1;
		*str1 = '\0';
		oldch2 = *str2;
		*str2 = '\0';

		/* this cannot happen, as we previously tested to make sure that */
		/* the first string has a non-null segment */
		if (one == str1)
			return -1;	/* arbitrary */

		/* take care of the case where the two version segments are */
		/* different types: one numeric, the other alpha (i.e. empty) */
		/* numeric segments are always newer than alpha segments */
		/* result_test See patch #60884 (and details) from bugzilla #50977. */
		if (two == str2)
			return (isnum ? 1 : -1);

		if (isnum) {
			/* this used to be done by converting the digit segments */
			/* to ints using atoi() - it's changed because long  */
			/* digit segments can overflow an int - this should fix that. */

			/* throw away any leading zeros - it's a number, right? */
			while (*one == '0')
				one++;
			while (*two == '0')
				two++;

			/* whichever number has more digits wins */
			if (strlen(one) > strlen(two))
				return 1;
			if (strlen(two) > strlen(one))
				return -1;
		}

		/* strcmp will return which one is greater - even if the two */
		/* segments are alpha or if they are numeric.  don't return  */
		/* if they are equal because there might be more segments to */
		/* compare */
		rc = strcmp(one, two);
		if (rc)
			return (rc < 1 ? -1 : 1);

		/* restore character that was replaced by null above */
		*str1 = oldch1;
		one = str1;
		*str2 = oldch2;
		two = str2;
	}
	/* this catches the case where all numeric and alpha segments have */
	/* compared identically but the segment sepparating characters were */
	/* different */
	if ((!*one) && (!*two))
		return 0;

	/* whichever version still has characters left over wins */
	if (!*one)
		return -1;
	else
		return 1;
}
#endif

/*
 * based on code from dpkg: lib/dpkg/version.c
 * Mino changes to use isdigit() and isalpha()
 */
/**
 * Give a weight to the character to order in the version comparison.
 *
 * @param c An ASCII character.
 */
static int order(int c)
{
	if (isdigit(c))
		return 0;
	else if (isalpha(c))
		return c;
	else if (c == '~')
		return -1;
	else if (c)
		return c + 256;
	else
		return 0;
}

/*
 * based on code from dpkg: lib/dpkg/version.c
 * Minor changes to use isdigit()
 */
static int verrevcmp(const char *a, const char *b)
{
	if (a == NULL)
		a = "";
	if (b == NULL)
		b = "";

	while (*a || *b) {
		int first_diff = 0;

		while ((*a && !isdigit(*a)) || (*b && !isdigit(*b))) {
			int ac = order(*a);
			int bc = order(*b);

			if (ac != bc)
				return ac - bc;

			a++;
			b++;
		}
		while (*a == '0')
			a++;
		while (*b == '0')
			b++;
		while (isdigit(*a) && isdigit(*b)) {
			if (!first_diff)
				first_diff = *a - *b;
			a++;
			b++;
		}

		if (isdigit(*a))
			return 1;
		if (isdigit(*b))
			return -1;
		if (first_diff)
			return first_diff;
	}

	return 0;
}

/*
 * Code copied from lib/dpkg/version.c
 */
/**
 * Compares two Debian versions.
 *
 * This function follows the convention of the comparator functions used by
 * qsort().
 *
 * @see deb-version(5)
 *
 * @param a The first version.
 * @param b The second version.
 *
 * @retval 0 If a and b are equal.
 * @retval <0 If a is smaller than b.
 * @retval >0 If a is greater than b.
 */
static int dpkg_version_compare(struct dpkg_version *a, struct dpkg_version *b)
{
	int rc;

	if (a->epoch > b->epoch)
		return 1;
	if (a->epoch < b->epoch)
		return -1;

	rc = verrevcmp(a->version, b->version);
	if (rc)
		return rc;

	return verrevcmp(a->revision, b->revision);
}

oval_result_t oval_debian_evr_string_cmp(const char *state, const char *sys, oval_operation_t operation)
{
	struct dpkg_version a, b;
	const char *a_epoch = NULL, *a_version = NULL, *a_release = NULL;
	const char *b_epoch = NULL, *b_version = NULL, *b_release = NULL;
	char *a_copy, *b_copy;
	long aux;

	a_copy = oscap_strdup(sys);
	b_copy = oscap_strdup(state);
	parseEVR(a_copy, &a_epoch, &a_version, &a_release);
	parseEVR(b_copy, &b_epoch, &b_version, &b_release);

	if (!a_epoch)
		a_epoch = "0";
	if (!b_epoch)
		b_epoch = "0";

	aux = strtol(a_epoch, NULL, 10);
	if (aux < INT_MIN || aux > INT_MAX) {
		free(a_copy);
		free(b_copy);
		return OVAL_RESULT_ERROR; // Outside int range
	}
	a.epoch = (int) aux;

	aux = strtol(b_epoch, NULL, 10);
	if (aux < INT_MIN || aux > INT_MAX) {
		free(a_copy);
		free(b_copy);
		return OVAL_RESULT_ERROR; // Outside int range
	}
	b.epoch = (int) aux;

	a.version = a_version;
	a.revision = a_release;
	b.version = b_version;
	b.revision = b_release;
	int result = dpkg_version_compare(&a, &b);

	free(a_copy);
	free(b_copy);
	switch (operation) {
	case OVAL_OPERATION_EQUALS:
		return ((result == 0) ? OVAL_RESULT_TRUE : OVAL_RESULT_FALSE);
	case OVAL_OPERATION_NOT_EQUAL:
		return ((result != 0) ? OVAL_RESULT_TRUE : OVAL_RESULT_FALSE);
	case OVAL_OPERATION_GREATER_THAN:
		return ((result > 0) ? OVAL_RESULT_TRUE : OVAL_RESULT_FALSE);
	case OVAL_OPERATION_GREATER_THAN_OR_EQUAL:
		return ((result >= 0) ? OVAL_RESULT_TRUE : OVAL_RESULT_FALSE);
	case OVAL_OPERATION_LESS_THAN:
		return ((result < 0) ? OVAL_RESULT_TRUE : OVAL_RESULT_FALSE);
	case OVAL_OPERATION_LESS_THAN_OR_EQUAL:
		return ((result <= 0) ? OVAL_RESULT_TRUE : OVAL_RESULT_FALSE);
	default:
		oscap_seterr(OSCAP_EFAMILY_OVAL, "Invalid type of operation in dpkg version comparison: %d.", operation);
	}

	return OVAL_RESULT_ERROR;
}

oval_result_t oval_versiontype_cmp(const char *state, const char *syschar, oval_operation_t operation)
{
	int state_idx = 0;
	int sys_idx = 0;
	int result = -1;
	/* int is_equal = 1; */
	for (state_idx = 0, sys_idx = 0; (((state[state_idx]) || (syschar[sys_idx])) && (result == -1));) {	// keep going as long as there is data in either the state or sysitem
		int tmp_state_int, tmp_sys_int;
		tmp_state_int = atoi(&state[state_idx]);	// look at the current data field (if we're at the end, atoi should return 0)
		tmp_sys_int = atoi(&syschar[sys_idx]);
                /* o rly?
		if (tmp_state_int != tmp_sys_int)
			is_equal = 0;	// we might need this later (if we don't terminate early)
                */
		if (operation == OVAL_OPERATION_EQUALS) {
			if (tmp_state_int != tmp_sys_int)
				return (OVAL_RESULT_FALSE);
		} else if (operation == OVAL_OPERATION_NOT_EQUAL) {
			if (tmp_state_int != tmp_sys_int)
				return (OVAL_RESULT_TRUE);
		} else if ((operation == OVAL_OPERATION_GREATER_THAN)
			   || (operation == OVAL_OPERATION_GREATER_THAN_OR_EQUAL)) {
			if (tmp_sys_int > tmp_state_int)
				return (OVAL_RESULT_TRUE);
			if (tmp_sys_int < tmp_state_int)
				return (OVAL_RESULT_FALSE);
		} else if ((operation == OVAL_OPERATION_LESS_THAN)
			   || (operation == OVAL_OPERATION_LESS_THAN_OR_EQUAL)) {
			if (tmp_sys_int < tmp_state_int)
				return (OVAL_RESULT_TRUE);
			if (tmp_sys_int > tmp_state_int)
				return (OVAL_RESULT_FALSE);
		} else {
			oscap_seterr(OSCAP_EFAMILY_OVAL, "Invalid type of operation in version comparison: %d.", operation);
			return OVAL_RESULT_ERROR;
		}

		if (state[state_idx])
			++state_idx;
		/* move to the next field within the version string (if there is one) */
		while ((state[state_idx]) && (isdigit(state[state_idx])))
			++state_idx;
		if ((state[state_idx]) && (!isdigit(state[state_idx])))
			++state_idx;

		if (syschar[sys_idx])
			++sys_idx;
		/* move to the next field within the version string (if there is one) */
		while ((syschar[sys_idx]) && (isdigit(syschar[sys_idx])))
			++sys_idx;
		if ((syschar[sys_idx]) && (!isdigit(syschar[sys_idx])))
			++sys_idx;
	}

	// OK, we did not terminate early, and we're out of data, so we now know what to return
	if (operation == OVAL_OPERATION_EQUALS) {
		return (OVAL_RESULT_TRUE);
	} else if (operation == OVAL_OPERATION_NOT_EQUAL) {
		return (OVAL_RESULT_FALSE);
	} else if (operation == OVAL_OPERATION_GREATER_THAN) {
		return (OVAL_RESULT_FALSE);
	} else if (operation == OVAL_OPERATION_GREATER_THAN_OR_EQUAL) {
		return (OVAL_RESULT_TRUE);
	} else if (operation == OVAL_OPERATION_LESS_THAN) {
		return (OVAL_RESULT_FALSE);
	} else if (operation == OVAL_OPERATION_LESS_THAN_OR_EQUAL) {
		return (OVAL_RESULT_TRUE);
	}		// we have already filtered out the invalid ones
	assert(0);
	return OVAL_RESULT_ERROR;

}
