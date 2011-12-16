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

#include "oscap-tool.h"
#include <stdio.h>
#include <wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>
#include <fcntl.h>

#include "xccdf_policy.h"

xccdf_test_result_type_t sce_engine_eval_rule(struct xccdf_policy *policy, const char *rule_id, const char *id,
			       const char *href, struct xccdf_value_binding_iterator *it, void *usr)
{
	struct sce_parameters* parameters = (struct sce_parameters*)usr;
	const char* xccdf_directory = parameters->xccdf_directory;

	// +1 for /, +1 for the terminating \0
	char *tmp_href = malloc((strlen(xccdf_directory) + 1 + strlen(href) + 1) * sizeof(char));
	sprintf(tmp_href, "%s/%s", xccdf_directory, href);

	if (access(tmp_href, F_OK))
	{
		// we only do this check to provide helpful error message
		// there is an inherent race condition, the file might
		// not exist anymore at the time we execve it!

		// the script hasn't been found, perhaps another sce instance
		// with a different XCCDF directory can find it?
		printf("SCE couldn't find script file '%s'. Expected location: '%s'.\n", href, tmp_href);
		free(tmp_href);
		return XCCDF_RESULT_NOT_CHECKED;
	}

	if (access(tmp_href, F_OK | X_OK))
	{
		// again, only to provide helpful error message
		printf("SCE has found script file '%s' at '%s' but it isn't executable!\n", href, tmp_href);
		free(tmp_href);
		return XCCDF_RESULT_ERROR;
	}

	// all the result codes are shifted by 100, because otherwise syntax errors in scripts
	// or even their nonexistence would cause XCCDF_RESULT_PASS to be the result

	char* argvp[1 + 1] = {
		tmp_href,
		NULL
	};

	// bound values in KEY=VALUE form, ready to be passed as environment variables
	char ** env_values = malloc(9 * sizeof(char * ));
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

		env_values = realloc(env_values, (env_value_count + 3) * sizeof(char*));

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

		// +11 because of XCCDF_TYPE_, +1 because of = in the middle, +1 because of \0
		char* env_type_entry = malloc((11 + strlen(name) + 1 + strlen(type_str) + 1) * sizeof(char));
		sprintf(env_type_entry, "XCCDF_TYPE_%s=%s", name, type_str);

		// +12 because of XCCDF_VALUE_, +1 because of = in the middle, +1 because of \0
		char* env_value_entry = malloc((12 + strlen(name) + 1 + strlen(value) + 1) * sizeof(char));
		sprintf(env_value_entry, "XCCDF_VALUE_%s=%s", name, value);

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

		// +15 because of XCCDF_OPERATOR_, +1 because of = in the middle, +1 because of \0
		char* env_operator_entry = malloc(15 + strlen(name) + 1 + strlen(operator_str) + 1);
		sprintf(env_operator_entry, "XCCDF_OPERATOR_%s=%s", name, operator_str);

		env_values[env_value_count] = env_type_entry;
		env_value_count++;
		env_values[env_value_count] = env_value_entry;
		env_value_count++;
		env_values[env_value_count] = env_operator_entry;
		env_value_count++;
	}

	env_values = realloc(env_values, (env_value_count + 1) * sizeof(char*));
	env_values[env_value_count] = NULL;

	// +8 = strlen(".results"), +1 for \0
	char * href_copy = strdup(href);
	char * href_basename = basename(href_copy);
	char * results_filename = NULL;
	if (parameters->results_target_dir)
	{
		results_filename = malloc((strlen(parameters->results_target_dir) + 1 + strlen(href_basename) + 8 + 1) * sizeof(char));
		sprintf(results_filename, "%s/%s.results", parameters->results_target_dir, href_basename);
	}
	else
	{
		results_filename = strdup("/dev/null");
	}

	free(href_copy);
	int results_file = creat(results_filename, S_IWUSR | S_IRUSR);
	free(results_filename);

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
			printf("SCE results for '%s'\n", basename(tmp_href));
			printf("***********************************************************\n");

			printf("[I] The following environment variables are being passed:\n");
			for (int i = 0; env_values[i]; i++)
			{
				printf("%s\n", env_values[i]);
			}

			printf("[I] raw stdout and stderr output from the script:\n");
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
			wait(&wstatus);
			dprintf(results_file, "[I] end of script output\n");
			free(tmp_href);

			// the first 9 values (0 to 8) are compiled in
			for (size_t i = 9; i < env_value_count; ++i)
			{
				free(env_values[i]);
			}
			free(env_values);

			// we subtract 100 here to shift the exit code to xccdf_test_result_type_t enum range
			int raw_result = WEXITSTATUS(wstatus) - 100;
			if (raw_result <= 0 || raw_result > XCCDF_RESULT_FIXED)
			{
				// the script returned invalid exit code, we need to safeguard us against that
				raw_result = XCCDF_RESULT_ERROR;
				dprintf(results_file, "[E] The check script returned an invalid exit code %i, interpreting it as error.\n", WEXITSTATUS(wstatus));
			}

			dprintf(results_file, "[I] exit code: %i, final XCCDF result: %i\n", WEXITSTATUS(wstatus), raw_result);
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

bool sce_register_engine(struct xccdf_policy_model * model, struct sce_parameters *parameters)
{
	return xccdf_policy_model_register_engine_callback(model, "http://open-scap.org/XMLSchema/SCE-definitions-1", sce_engine_eval_rule, (void*)parameters);
}
