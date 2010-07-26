/*
 * Copyright 2010 Red Hat Inc., Durham, North Carolina.
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:
 *      Maros Barabas  <mbarabas@redhat.com>
 */

/* Standard header files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

/* Header files for curl */
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

#include "oscap-tool.h"

int VERBOSE = 1;

static struct oscap_action *oscap_action_new(void);
static void oscap_action_free(struct oscap_action *action);
static void print_usage(const char *pname, FILE * out);
static void print_versions(void);
static char *app_curl_download(char *url);
static int app_validate_xml(const struct oscap_action *action);


int main(int argc, char **argv)
{
	/**************** GETOPT  ***************/
	int c;
	struct oscap_action *action = oscap_action_new();
	if (action == NULL)
		return 1;

	while (1) {

		int option_index = 0;
		static struct option long_options[] = {
			/* Long options. */
			{"quiet", 0, 0, 'q'},
			{"help", 0, 0, 'h'},
			{"version", 0, 0, 'V'},
			{0, 0, 0, 0}
		};

		c = getopt_long(argc, argv, "+qhV", long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 'q':	/* QUIET */
			VERBOSE = -1;
			break;
		case 'h':	/* HELP */
			print_usage(argv[0], stdout);
			return 0;
		case 'V':	/* VERSIONS */
			print_versions();
			return 0;
		default:
			break;
		}
	}

	/* MODULE */
	if (optind >= argc) {
		print_usage(argv[0], stderr);
		return 1;
	}

	if ((!strcmp(argv[optind], "xccdf")) || (!strcmp(argv[optind], "XCCDF"))) {
#ifdef ENABLE_XCCDF
		if (getopt_xccdf(argc, argv, action) == -1)
			return 1;
#endif
	} else if ((!strcmp(argv[optind], "oval")) || (!strcmp(argv[optind], "OVAL"))) {
#ifdef ENABLE_OVAL
		if (getopt_oval(argc, argv, action) == -1)
			return 1;
#endif
	} else if ((!strcmp(argv[optind], "cvss")) || (!strcmp(argv[optind], "CVSS"))) {
#ifdef ENABLE_CVSS
		if (getopt_cvss(argc, argv, action) == -1)
			return 1;
#endif
	}

	/* Post processing of options */
	/* fetch file from remote source */
	if (action->url_xccdf != NULL) {

		action->f_xccdf = app_curl_download(action->url_xccdf);
		if (!action->f_xccdf) {
			if (VERBOSE >= 0)
				fprintf(stderr, "Error: (%d) %s\n", oscap_err_code(), oscap_err_desc());
			return 1;
		}
	}
	if (action->url_oval != NULL) {
		action->f_oval = app_curl_download(action->url_oval);
		if (!action->f_oval) {
			if (VERBOSE >= 0)
				fprintf(stderr, "Error: (%d) %s\n", oscap_err_code(), oscap_err_desc());
			return 1;
		}
	}

	/**************** GETOPT END ***************/
	int retval = 0;

	switch (action->std) {
	case OSCAP_STD_XCCDF:
#ifdef ENABLE_XCCDF
		switch (action->op) {
		case OSCAP_OP_VALIDATE_XML:
			retval = app_validate_xml(action);
			break;
		case OSCAP_OP_EVAL:
			retval = app_evaluate_xccdf(action);
			break;
		case OSCAP_OP_RESOLVE:
#ifdef ENABLE_XCCDF
			retval = app_xccdf_resolve(action);
#else
			fprintf(stderr,
				"OSCAP is not compiled with XCCDF support ! Please configure OSCAP library with option --enable-xccdf !\n");
#endif
			break;
		default:
			break;
		}
#endif
		break;
	case OSCAP_STD_OVAL:
#ifdef ENABLE_OVAL
		switch (action->op) {
		case OSCAP_OP_VALIDATE_XML:
			retval = app_validate_xml(action);
			break;
		case OSCAP_OP_COLLECT:
			retval = app_collect_oval(action);
			break;
		case OSCAP_OP_EVAL:
			retval = app_evaluate_oval(action);
			break;
		default:
			break;
		}
#endif
		break;
	case OSCAP_STD_CVSS:
#ifdef ENABLE_CVSS
		switch (action->op) {
		case OSCAP_OP_BASE: {
			double base_score;
			cvss_base_score(action->cvss_metrics->ave, 
					action->cvss_metrics->ace, 
					action->cvss_metrics->aue, 
					action->cvss_metrics->cie, 
					action->cvss_metrics->iie, 
					action->cvss_metrics->aie, 
					&base_score, NULL, NULL);
			fprintf(stdout, "Base score: %f\n", base_score);
		}
		break;
		case OSCAP_OP_TEMP: {
			double temp_score;
			cvss_temp_score(action->cvss_metrics->exe, 
					action->cvss_metrics->rle, 
					action->cvss_metrics->rce, 
					action->cvss_metrics->base, 
					&temp_score);
			fprintf(stdout, "Temporal score: %f\n", temp_score);
		}
		break;
		case OSCAP_OP_ENV: {
			double temp_env;
			cvss_env_score(	action->cvss_metrics->cde, action->cvss_metrics->tde,
					action->cvss_metrics->cre, action->cvss_metrics->ire,
					action->cvss_metrics->are, action->cvss_metrics->ave,
					action->cvss_metrics->ace, action->cvss_metrics->aue,
					action->cvss_metrics->cie, action->cvss_metrics->iie,
					action->cvss_metrics->aie, action->cvss_metrics->exe,
					action->cvss_metrics->rle, action->cvss_metrics->rce,
					&temp_env);
			fprintf(stdout, "Environmental score: %f\n", temp_env);
		}
		break;
		default:
			break;
		}
#endif
		break;
	default:
		printf("%s unrecognized module '%s' or module support is not compiled in.\n", argv[0], argv[1]);
		print_usage(argv[0], stderr);
		break;
	}

	oscap_action_free(action);
	oscap_cleanup();

	return retval;
}


static struct oscap_action *oscap_action_new(void)
{
	struct oscap_action *action;
	action = malloc(sizeof(struct oscap_action));
	if (action == NULL)
		return NULL;

	action->std = OSCAP_STD_UNKNOWN;
	action->op = OSCAP_OP_UNKNOWN;
	action->f_xccdf = NULL;
	action->f_oval = NULL;
	action->f_results = NULL;
	action->url_xccdf = NULL;
	action->url_oval = NULL;
	action->profile = NULL;
	action->file_version = NULL;
#ifdef ENABLE_CVSS
	action->cvss_metrics = NULL;
	action->force = false;
#endif
	return action;
}

static void oscap_action_free(struct oscap_action *action)
{
	if (action == NULL)
		return;

	if (action->f_xccdf)
		free(action->f_xccdf);
	if (action->f_oval)
		free(action->f_oval);
	action->f_results = NULL;
	action->url_oval = NULL;
	action->url_xccdf = NULL;
#ifdef ENABLE_CVSS
	if (action->cvss_metrics != NULL) {
		free(action->cvss_metrics);
		action->cvss_metrics = NULL;
	}
#endif
	//free(action);
}

/**
 * @param pname name of program, standard usage argv[0]
 * @param out output stream for fprintf function, standard usage stdout or stderr
 */
static void print_usage(const char *pname, FILE * out)
{

	fprintf(out,
		"Usage: %s [general-options] module operation [operation-options-and-arguments]\n"
		"\n"
		"General options:\n"
		"   -h --help\r\t\t\t\t - show this help\n"
		"   -q --quiet\r\t\t\t\t - quiet mode\n"
		"   -V --version\r\t\t\t\t - print info about supported SCAP versions\n"
		"\n" "Module specific help\n" " %s [oval|xccdf|cvss] --help\n", pname, pname);
}

static void print_versions(void)
{
	fprintf(stdout,
		"OSCAP util (oscap) 0.5.12\n" "Copyright 2009,2010 Red Hat Inc., Durham, North Carolina.\n" "\n");
#ifdef ENABLE_XCCDF
	fprintf(stdout, "OVAL Version: \r\t\t%s\n", oval_definition_model_supported());
#endif
#ifdef ENABLE_XCCDF
	fprintf(stdout, "XCCDF Version: \r\t\t%s\n", xccdf_benchmark_supported());
#endif
#ifdef ENABLE_CVSS
	fprintf(stdout, "CVSS Version: \r\t\t%s\n", cvss_model_supported());
#endif
}

/**
 * @param url URL or PATH to file
 * @return path to local file
 */
static char *app_curl_download(char *url)
{

	struct stat buf;
	/* Is the file local ? */
	if (lstat(url, &buf) == 0)
		return strdup(url);

	/* Remote file will be stored in this xml */
	char *outfile = strdup("definition_file.xml");

	CURL *curl;
	FILE *fp;
	CURLcode res;
	/* Initialize CURL for download remote file */
	curl = curl_easy_init();
	if (!curl)
		return NULL;

	fp = fopen(outfile, "wb");
	/* Set options for download file to *fp* from *url* */
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
	res = curl_easy_perform(curl);
	if (res != 0) {
		curl_easy_cleanup(curl);
		fclose(fp);
		return NULL;
	}

	curl_easy_cleanup(curl);
	fclose(fp);

	return outfile;
}


static int app_validate_xml(const struct oscap_action *action)
{
	const char *xml_file = action->f_oval;
	if (!xml_file)
		xml_file = action->f_xccdf;
	if (!xml_file)
		return 2;

	if (!oscap_validate_document
	    (xml_file, action->doctype, action->file_version, (VERBOSE >= 0 ? oscap_reporter_fd : NULL), stdout)) {
		if (oscap_err()) {
			fprintf(stderr, "ERROR: %s\n", oscap_err_desc());
			return 2;
		}
		return 1;
	}
	return 0;
}

