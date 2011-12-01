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

#include "oscap-tool.h"
#include <stdio.h>
#include <wait.h>
#include <unistd.h>
#include <sys/stat.h>

xccdf_test_result_type_t sce_engine_eval_rule(struct xccdf_policy *policy, const char *rule_id, const char *id,
			       const char *href, struct xccdf_value_binding_iterator *it, void *usr)
{
	const char* xccdf_directory = (const char * )usr;

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

	// FIXME: We definitely want to impose security restrictions in the forked child process in the future.
	//        This would prevent scripts from writing to files or deleting them.

	int fork_result = fork();
	if (fork_result >= 0)
	{
		// fork successful

		// all the result codes are shifted by 100, because otherwise syntax errors in scripts
		// or even their nonexistence would cause XCCDF_RESULT_PASS to be the result

		char* argvp[1 + 1] = {
				tmp_href,
				0
		};

		char* envp[9 + 1] = {
				"XCCDF_RESULT_PASS=101",
				"XCCDF_RESULT_FAIL=102",
				"XCCDF_RESULT_ERROR=103",
				"XCCDF_RESULT_UNKNOWN=104",
				"XCCDF_RESULT_NOT_APPLICABLE=105",
				"XCCDF_RESULT_NOT_CHECKED=106",
				"XCCDF_RESULT_NOT_SELECTED=107",
				"XCCDF_RESULT_INFORMATIONAL=108",
				"XCCDF_RESULT_FIXED=109",
				0
		};

		if (fork_result == 0)
		{
			// we are the child process
			execve(tmp_href, argvp, envp);

			// no need to check the return value of execve, if it returned at all we are in trouble
			printf("Unexpected error when executing script '%s'. Error message follows. ", href);
			perror("execve");

			// the parent process considers us a script check, we have to return a value that will mean XCCDF_RESULT_NOT_CHECKED
			exit(103);
		}
		else
		{
			// we are the parent process
			int wstatus;
			wait(&wstatus);
			free(tmp_href);

			// we subtract 100 here to shift the exit code to xccdf_test_result_type_t enum range
			int raw_result = WEXITSTATUS(wstatus) - 100;
			if (raw_result <= 0 || raw_result > XCCDF_RESULT_FIXED)
			{
				// the script returned invalid exit code, we need to safeguard us against that
				raw_result = XCCDF_RESULT_ERROR;
			}

			return (xccdf_test_result_type_t)raw_result;
		}
	}
	else
	{
		return XCCDF_RESULT_ERROR;
	}
}

bool sce_register_engine(struct xccdf_policy_model * model, const char *xccdf_directory)
{
	return xccdf_policy_model_register_engine_callback(model, "http://open-scap.org/XMLSchema/SCE-definitions-1", sce_engine_eval_rule, (void*)xccdf_directory);
}
