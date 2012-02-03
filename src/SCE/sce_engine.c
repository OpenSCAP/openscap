/**
 * @file sce_engine.c
 * \brief Script Check Engine
 */

/*
 * Copyright 2011 Red Hat Inc., Durham, North Carolina.
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

#include "alloc.h"
#include "common/util.h"
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
#include <limits.h>
#include <unistd.h>

struct sce_parameters
{
	char * xccdf_directory;
	char * results_target_dir;
};

struct sce_parameters* sce_parameters_new(void)
{
	struct sce_parameters *ret = oscap_alloc(sizeof(struct sce_parameters));
	ret->xccdf_directory = 0;
	ret->results_target_dir = 0;

	return ret;
}

void sce_parameters_free(struct sce_parameters* v)
{
	oscap_free(v->xccdf_directory);
	oscap_free(v->results_target_dir);

	oscap_free(v);
}

void sce_parameters_set_xccdf_directory(struct sce_parameters* v, const char* value)
{
	oscap_free(v->xccdf_directory);
	v->xccdf_directory = value == 0 ? 0 : oscap_strdup(value);
}

const char* sce_parameters_get_xccdf_directory(struct sce_parameters* v)
{
	return v->xccdf_directory;
}

void sce_parameters_set_results_target_directory(struct sce_parameters* v, const char* value)
{
	oscap_free(v->results_target_dir);
	v->results_target_dir = value == 0 ? 0 : oscap_strdup(value);
}

const char* sce_parameters_get_results_target_directory(struct sce_parameters* v)
{
	return v->results_target_dir;
}

xccdf_test_result_type_t sce_engine_eval_rule(struct xccdf_policy *policy, const char *rule_id, const char *id,
			       const char *href, struct xccdf_value_binding_iterator *it, void *usr)
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
		printf("SCE couldn't find script file '%s'. Expected location: '%s'.\n", href, tmp_href);
		oscap_free(tmp_href);
		return XCCDF_RESULT_NOT_CHECKED;
	}

	if (access(tmp_href, F_OK | X_OK))
	{
		// again, only to provide helpful error message
		printf("SCE has found script file '%s' at '%s' but it isn't executable!\n", href, tmp_href);
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
	char ** env_values = oscap_alloc(9 * sizeof(char * ));
	size_t env_value_count = 9;

	env_values[0] = "XCCDF_RESULT_PASS=101";
	env_values[1] = "XCCDF_RESULT_FAIL=102";
	env_values[2] = "XCCDF_RESULT_ERROR=103";
	env_values[3] = "XCCDF_RESULT_UNKNOWN=104";
	env_values[4] = "XCCDF_RESULT_NOT_APPLICABLE=105";
	env_values[5] = "XCCDF_RESULT_NOT_CHECKED=106";
	env_values[6] = "XCCDF_RESULT_NOT_SELECTED=107";
	env_values[7] = "XCCDF_RESULT_INFORMATIONAL=108";
	env_values[8] = "XCCDF_RESULT_FIXED=109";

	while (xccdf_value_binding_iterator_has_more(it))
	{
		struct xccdf_value_binding* binding = xccdf_value_binding_iterator_next(it);

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

	// +8 = strlen(".results"), +1 for \0
	char * results_filename = NULL;
	if (parameters->results_target_dir)
	{
		char * href_copy = oscap_strdup(href);
		char * href_basename = basename(href_copy);
		results_filename = oscap_sprintf("%s/%s.result.xml", parameters->results_target_dir, href_basename);
		oscap_free(href_copy);
	}
	else
	{
		results_filename = oscap_strdup("/dev/null");
	}

	int results_file = creat(results_filename, S_IWUSR | S_IRUSR);
	oscap_free(results_filename);

	// FIXME: We definitely want to impose security restrictions in the forked child process in the future.
	//        This would prevent scripts from writing to files or deleting them.

	int fork_result = fork();
	if (fork_result >= 0)
	{
		// fork successful

		if (fork_result == 0)
		{
			dup2(results_file, fileno(stdout));
			dup2(results_file, fileno(stderr));

			// we are the child process
			printf("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
			// FIXME: We definitely should escape the attribute!
			printf("<sceres:sce_results xmlns:sceres=\"http://open-scap.org/page/SCE_result_file\" script-path=\"%s\">\n", basename(tmp_href));

			printf("\t<sceres:environment>\n");
			for (int i = 0; env_values[i]; i++)
			{
				printf("\t\t<sceres:entry><![CDATA[%s]]></sceres:entry>\n", env_values[i]);
			}
			printf("\t</sceres:environment>\n");

			printf("\t<sceres:stdout><![CDATA[\n");
			execve(tmp_href, argvp, env_values);

			// no need to check the return value of execve, if it returned at all we are in trouble
			printf("Unexpected error when executing script '%s'. Error message follows.\n", href);
			perror("execve");

			// the parent process considers us a script check, we have to return a value that will mean XCCDF_RESULT_ERROR
			exit(103);
		}
		else
		{
			// we are the parent process
			int wstatus;
			waitpid(fork_result, &wstatus, 0);
			dprintf(results_file, "\n]]>\n");
			dprintf(results_file, "\t</sceres:stdout>\n");
			oscap_free(tmp_href);

			// the first 9 values (0 to 8) are compiled in
			for (size_t i = 9; i < env_value_count; ++i)
			{
				oscap_free(env_values[i]);
			}
			oscap_free(env_values);

			// we subtract 100 here to shift the exit code to xccdf_test_result_type_t enum range
			int raw_result = WEXITSTATUS(wstatus) - 100;
			if (raw_result <= 0 || raw_result > XCCDF_RESULT_FIXED)
			{
				// the script returned invalid exit code, we need to safeguard us against that
				raw_result = XCCDF_RESULT_ERROR;
				dprintf(results_file, "\t<sceres:debug_message>The check script returned an invalid exit code %i, interpreting it as error.</sceres:debug_message>\n", WEXITSTATUS(wstatus));
			}

			dprintf(results_file, "\t<sceres:exit_code>%i</sceres:exit_code>\n", WEXITSTATUS(wstatus));
			dprintf(results_file, "\t<sceres:result>%i</sceres:result>\n", raw_result);
			dprintf(results_file, "</sceres:sce_results>\n");
			close(results_file);

			return (xccdf_test_result_type_t)raw_result;
		}
	}
	else
	{
		close(results_file);
		return XCCDF_RESULT_ERROR;
	}
}

bool xccdf_policy_model_register_engine_sce(struct xccdf_policy_model * model, struct sce_parameters *parameters)
{
	return xccdf_policy_model_register_engine_callback(model, "http://open-scap.org/page/SCE", sce_engine_eval_rule, (void*)parameters);
}
