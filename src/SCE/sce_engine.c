/**
 * @file sce_engine.c
 * \brief Script Check Engine
 */

/*
 * Copyright 2011--2013 Red Hat Inc., Durham, North Carolina.
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
 *      "Martin Preisler" <mpreisle@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "common/alloc.h"
#include "common/_error.h"
#include "common/util.h"
#include "common/list.h"
#include "common/oscap_acquire.h"
#include "sce_engine_api.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/types.h>
#if defined(__linux__)
#include <sys/prctl.h>
#endif
#include <limits.h>
#include <unistd.h>
#include <libgen.h>

struct sce_check_result
{
	char* href;
	char* basename;
	char* std_out;
	int exit_code;
	struct oscap_stringlist* environment_variables;
	xccdf_test_result_type_t xccdf_result;
};

struct sce_check_result* sce_check_result_new(void)
{
	struct sce_check_result* ret = oscap_alloc(sizeof(struct sce_check_result));
	ret->href = NULL;
	ret->basename = NULL;
	ret->std_out = NULL;
	ret->environment_variables = oscap_stringlist_new();
	ret->xccdf_result = XCCDF_RESULT_UNKNOWN;

	return ret;
}

void sce_check_result_free(struct sce_check_result* v)
{
	if (!v)
		return;

	if (v->href)
		oscap_free(v->href);
	if (v->basename)
		oscap_free(v->basename);
	if (v->std_out)
		oscap_free(v->std_out);

	oscap_stringlist_free(v->environment_variables);

	oscap_free(v);
}

void sce_check_result_set_href(struct sce_check_result* v, const char* href)
{
	if (v->href)
		oscap_free(v->href);

	v->href = strdup(href);
}

const char* sce_check_result_get_href(struct sce_check_result* v)
{
	return v->href;
}

void sce_check_result_set_basename(struct sce_check_result* v, const char* base_name)
{
	if (v->basename)
		oscap_free(v->basename);

	v->basename = strdup(base_name);
}

const char* sce_check_result_get_basename(struct sce_check_result* v)
{
	return v->basename;
}

void sce_check_result_set_stdout(struct sce_check_result* v, const char* _stdout)
{
	if (v->std_out)
		oscap_free(v->std_out);

	v->std_out = strdup(_stdout);
}

const char* sce_check_result_get_stdout(struct sce_check_result* v)
{
	return v->std_out;
}

void sce_check_result_set_exit_code(struct sce_check_result* v, int exit_code)
{
	v->exit_code = exit_code;
}

int sce_check_result_get_exit_code(struct sce_check_result* v)
{
	return v->exit_code;
}

void sce_check_result_reset_environment_variables(struct sce_check_result* v)
{
	oscap_stringlist_free(v->environment_variables);
	v->environment_variables = oscap_stringlist_new();
}

void sce_check_result_add_environment_variable(struct sce_check_result* v, const char* var)
{
	oscap_stringlist_add_string(v->environment_variables, var);
}

void sce_check_result_set_xccdf_result(struct sce_check_result* v, xccdf_test_result_type_t result)
{
	v->xccdf_result = result;
}

xccdf_test_result_type_t sce_check_result_get_xccdf_result(struct sce_check_result* v)
{
	return v->xccdf_result;
}

void sce_check_result_export(struct sce_check_result* v, const char* target_file)
{
	FILE* f = fopen(target_file, "w");
	if (!f)
	{
		oscap_seterr(OSCAP_EFAMILY_SCE, "Can't open file '%s' for writing.", target_file);
		return;
	}

	fprintf(f, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
	fprintf(f, "<sceres:sce_results xmlns:sceres=\"http://open-scap.org/page/SCE_result_file\" script-path=\"%s\">\n", sce_check_result_get_basename(v));
	fprintf(f, "\t<sceres:environment>\n");
	struct oscap_string_iterator* it = oscap_stringlist_get_strings(v->environment_variables);
	while (oscap_string_iterator_has_more(it))
	{
		const char* env = oscap_string_iterator_next(it);
		fprintf(f, "\t\t<sceres:entry><![CDATA[%s]]></sceres:entry>\n", env);
	}
	oscap_string_iterator_free(it);
	fprintf(f, "\t</sceres:environment>\n");
	fprintf(f, "\t<sceres:stdout><![CDATA[\n");
	fwrite(v->std_out, 1, strlen(v->std_out), f);
	fprintf(f, "\t]]></sceres:stdout>\n");
	fprintf(f, "\t<sceres:exit_code>%i</sceres:exit_code>\n", sce_check_result_get_exit_code(v));
	fprintf(f, "\t<sceres:result>%s</sceres:result>\n", xccdf_test_result_type_get_text(sce_check_result_get_xccdf_result(v)));
	fprintf(f, "</sceres:sce_results>\n");
	fclose(f);
}

struct sce_session
{
	struct oscap_list* results;
};

struct sce_session* sce_session_new(void)
{
	struct sce_session* ret = oscap_alloc(sizeof(struct sce_session));
	ret->results = oscap_list_new();

	return ret;
}

void sce_session_free(struct sce_session* s)
{
	if (!s)
		return;

	oscap_list_free(s->results, (oscap_destruct_func) sce_check_result_free);
	oscap_free(s);
}

void sce_session_reset(struct sce_session* s)
{
	oscap_list_free(s->results, (oscap_destruct_func) sce_check_result_free);
	s->results = oscap_list_new();
}

void sce_session_add_check_result(struct sce_session* s, struct sce_check_result* result)
{
	oscap_list_push(s->results, result);
}

OSCAP_ITERATOR_GEN(sce_check_result)

struct sce_check_result_iterator *sce_session_get_check_results(struct sce_session* s)
{
	return oscap_iterator_new(s->results);
}

void sce_session_export_to_directory(struct sce_session* s, const char* directory)
{
	struct sce_check_result_iterator * it = sce_session_get_check_results(s);

	while(sce_check_result_iterator_has_more(it))
	{
		struct sce_check_result * result = sce_check_result_iterator_next(it);
		char* target = oscap_sprintf("%s/%s.result.xml", directory, sce_check_result_get_basename(result));
		sce_check_result_export(result, target);
		oscap_free(target);
	}

	sce_check_result_iterator_free(it);
}

struct sce_parameters
{
	char* xccdf_directory;
	struct sce_session* session;
};

struct sce_parameters* sce_parameters_new(void)
{
	struct sce_parameters *ret = oscap_alloc(sizeof(struct sce_parameters));
	ret->xccdf_directory = NULL;
	ret->session = NULL;

	return ret;
}

void sce_parameters_free(struct sce_parameters* v)
{
	if (!v)
		return;

	if (v->xccdf_directory)
		oscap_free(v->xccdf_directory);
	if (v->session)
		sce_session_free(v->session);

	oscap_free(v);
}

void sce_parameters_set_xccdf_directory(struct sce_parameters* v, const char* value)
{
	if (v->xccdf_directory)
		oscap_free(v->xccdf_directory);

	v->xccdf_directory = value == NULL ? NULL : strdup(value);
}

const char* sce_parameters_get_xccdf_directory(struct sce_parameters* v)
{
	return v->xccdf_directory;
}

void sce_parameters_set_session(struct sce_parameters* v, struct sce_session* value)
{
	if (v->session)
	{
		sce_session_free(v->session);
		v->session = NULL;
	}

	v->session = value;
}

struct sce_session* sce_parameters_get_session(struct sce_parameters* v)
{
	return v->session;
}

void sce_parameters_allocate_session(struct sce_parameters* v)
{
	sce_parameters_set_session(v, sce_session_new());
}

xccdf_test_result_type_t sce_engine_eval_rule(struct xccdf_policy *policy, const char *rule_id, const char *id, const char *href,
		struct xccdf_value_binding_iterator *value_binding_it,
		struct xccdf_check_import_iterator *check_import_it,
		void *usr)
{
	struct sce_parameters* parameters = (struct sce_parameters*)usr;
	const char* xccdf_directory = parameters->xccdf_directory;

	char* tmp_href = oscap_sprintf("%s/%s", xccdf_directory, href);

	if (access(tmp_href, F_OK))
	{
		// we only do this check to provide helpful error message
		// there is an inherent race condition, the file might
		// not exist anymore at the time we execve it!

		// the script hasn't been found, perhaps another sce instance
		// with a different XCCDF directory can find it?
		oscap_seterr(OSCAP_EFAMILY_SCE, "SCE couldn't find script file '%s'. "
				"Expected location: '%s'.", href, tmp_href);
		oscap_free(tmp_href);
		return XCCDF_RESULT_NOT_CHECKED;
	}

	if (access(tmp_href, F_OK | X_OK))
	{
		// again, only to provide helpful error message
		oscap_seterr(OSCAP_EFAMILY_SCE, "SCE has found script file '%s' at '%s' "
				"but it isn't executable!", href, tmp_href);
		oscap_free(tmp_href);
		return XCCDF_RESULT_ERROR;
	}

	// all the result codes are shifted by 100, because otherwise syntax errors in scripts
	// or even their nonexistence would cause XCCDF_RESULT_PASS to be the result

	char* argvp[1 + 1] = {
		tmp_href,
		NULL
	};

	// bound values in KEY=VALUE form, ready to be passed as environment variables
	char ** env_values = oscap_alloc(10 * sizeof(char * ));
	size_t env_value_count = 10;

	env_values[0] = "PATH=/bin:/sbin:/usr/bin:/usr/sbin";

	env_values[1] = "XCCDF_RESULT_PASS=101";
	env_values[2] = "XCCDF_RESULT_FAIL=102";
	env_values[3] = "XCCDF_RESULT_ERROR=103";
	env_values[4] = "XCCDF_RESULT_UNKNOWN=104";
	env_values[5] = "XCCDF_RESULT_NOT_APPLICABLE=105";
	env_values[6] = "XCCDF_RESULT_NOT_CHECKED=106";
	env_values[7] = "XCCDF_RESULT_NOT_SELECTED=107";
	env_values[8] = "XCCDF_RESULT_INFORMATIONAL=108";
	env_values[9] = "XCCDF_RESULT_FIXED=109";

	while (xccdf_value_binding_iterator_has_more(value_binding_it))
	{
		struct xccdf_value_binding* binding = xccdf_value_binding_iterator_next(value_binding_it);

		env_values = oscap_realloc(env_values, (env_value_count + 3) * sizeof(char*));

		char* name = xccdf_value_binding_get_name(binding);
		xccdf_value_type_t type = xccdf_value_binding_get_type(binding);
		char* value = xccdf_value_binding_get_value(binding);
		xccdf_operator_t operator = xccdf_value_binding_get_operator(binding);

		char* type_str;
		switch (type)
		{
		case XCCDF_TYPE_BOOLEAN:
			type_str = "BOOLEAN";
			break;
		case XCCDF_TYPE_NUMBER:
			type_str = "NUMBER";
			break;
		case XCCDF_TYPE_STRING:
			type_str = "STRING";
			break;
		default:
			assert(0);
			type_str = NULL;
			break;
		}

		char* env_type_entry = oscap_sprintf("XCCDF_TYPE_%s=%s", name, type_str);
		char* env_value_entry = oscap_sprintf("XCCDF_VALUE_%s=%s", name, value);

		char* operator_str;
		switch (operator)
		{
		case XCCDF_OPERATOR_EQUALS:
			operator_str = "EQUALS";
			break;
		case XCCDF_OPERATOR_NOT_EQUAL:
			operator_str = "NOT_EQUAL";
			break;
		case XCCDF_OPERATOR_GREATER:
			operator_str = "GREATER";
			break;
		case XCCDF_OPERATOR_GREATER_EQUAL:
			operator_str = "GREATER_EQUAL";
			break;
		case XCCDF_OPERATOR_LESS:
			operator_str = "LESS";
			break;
		case XCCDF_OPERATOR_LESS_EQUAL:
			operator_str = "LESS_EQUAL";
			break;
		case XCCDF_OPERATOR_PATTERN_MATCH:
			operator_str = "PATTERN_MATCH";
			break;
		default:
			assert(0);
			operator_str = NULL;
			break;
		}

		char* env_operator_entry = oscap_sprintf("XCCDF_OPERATOR_%s=%s", name, operator_str);

		env_values[env_value_count] = env_type_entry;
		env_value_count++;
		env_values[env_value_count] = env_value_entry;
		env_value_count++;
		env_values[env_value_count] = env_operator_entry;
		env_value_count++;
	}

	env_values = oscap_realloc(env_values, (env_value_count + 1) * sizeof(char*));
	env_values[env_value_count] = NULL;

	// We open a pipe for communication with the forked process
	int pipefd[2];
	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		// the first 9 values (0 to 8) are compiled in
		for (size_t i = 9; i < env_value_count; ++i)
		{
			oscap_free(env_values[i]);
		}
		oscap_free(env_values);
		return XCCDF_RESULT_ERROR;
	}

	// FIXME: We definitely want to impose security restrictions in the forked child process in the future.
	//        This would prevent scripts from writing to files or deleting them.

	int fork_result = fork();
	if (fork_result >= 0)
	{
		// fork successful

		if (fork_result == 0)
		{
		    // we won't read from the pipe, so close the reading fd
		    close(pipefd[0]);

		    // forward stdout and stderr to the opened pipe
			dup2(pipefd[1], fileno(stdout));
			dup2(pipefd[1], fileno(stderr));

			// we duplicated the file description twice, we can close the original
			// one now, stdout and stderr will be closed properly after the execved
			// script/executable finishes
			close(pipefd[1]);

			// before we execute the script, lets make sure we get SIGTERM when
			// oscap is killed, crashes or otherwise terminates
#ifdef PR_SET_PDEATHSIG
			// requires Linux 2.1.57 or later
			prctl(PR_SET_PDEATHSIG, SIGTERM);
#else
			// TODO: Please provide alternatives
#endif

			// we are the child process
			execve(tmp_href, argvp, env_values);

			// no need to check the return value of execve, if it returned at all we are in trouble
			printf("Unexpected error when executing script '%s'. Error message follows.\n", href);
			perror("execve");

			// the parent process considers us a script check, we have to return a value that will mean XCCDF_RESULT_ERROR
			exit(103);
		}
		else
		{
			// we won't write to the pipe, so close the writing fd
			close(pipefd[1]);

			char* stdout_buffer = oscap_acquire_pipe_to_string(pipefd[0]);

			// we are the parent process
			int wstatus;
			waitpid(fork_result, &wstatus, 0);

			// we subtract 100 here to shift the exit code to xccdf_test_result_type_t enum range
			int raw_result = WEXITSTATUS(wstatus) - 100;
			if (raw_result <= 0 || raw_result > XCCDF_RESULT_FIXED)
			{
				// the script returned invalid exit code, we need to safeguard us against that
				raw_result = XCCDF_RESULT_ERROR;
			}

			struct sce_session* session = sce_parameters_get_session(parameters);
			if (session)
			{
				struct sce_check_result* check_result = sce_check_result_new();
				sce_check_result_set_href(check_result, tmp_href);
				sce_check_result_set_basename(check_result, basename(tmp_href));
				sce_check_result_set_stdout(check_result, stdout_buffer);
				sce_check_result_set_exit_code(check_result, WEXITSTATUS(wstatus));
				sce_check_result_set_xccdf_result(check_result, (xccdf_test_result_type_t)raw_result);

				for (size_t i = 0; i < env_value_count; ++i)
				{
					sce_check_result_add_environment_variable(check_result, env_values[i]);
				}

				sce_session_add_check_result(session, check_result);
			}

			// the first 10 values (0 to 9) are compiled in
			for (size_t i = 10; i < env_value_count; ++i)
			{
				oscap_free(env_values[i]);
			}
			oscap_free(env_values);

			// lets interpret the check imports passed to us
			xccdf_check_import_iterator_reset(check_import_it);
			while (xccdf_check_import_iterator_has_more(check_import_it))
			{
				struct xccdf_check_import * check_import = xccdf_check_import_iterator_next(check_import_it);
				const char *name = xccdf_check_import_get_name(check_import);

				if (strcmp(name, "stdout") == 0)
				{
					xccdf_check_import_set_content(check_import, stdout_buffer);
				}
			}

			oscap_free(tmp_href);
			oscap_free(stdout_buffer);

			return (xccdf_test_result_type_t)raw_result;
		}
	}
	else
	{
		// the first 9 values (0 to 8) are compiled in
		for (size_t i = 9; i < env_value_count; ++i)
		{
			oscap_free(env_values[i]);
		}
		oscap_free(env_values);

		close(pipefd[0]);
		close(pipefd[1]);
		return XCCDF_RESULT_ERROR;
	}
}

bool xccdf_policy_model_register_engine_sce(struct xccdf_policy_model * model, struct sce_parameters *parameters)
{
	return xccdf_policy_model_register_engine_and_query_callback(model,
		"http://open-scap.org/page/SCE", sce_engine_eval_rule, (void*)parameters, NULL);
}
