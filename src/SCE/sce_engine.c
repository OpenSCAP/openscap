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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Authors:
 *      "Martin Preisler" <mpreisle@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "common/_error.h"
#include "common/util.h"
#include "common/list.h"
#include "common/oscap_acquire.h"
#include "common/oscap_string.h"
#include "common/debug_priv.h"
#include "sce_engine_api.h"
#include "oscap_helpers.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#if defined(OS_FREEBSD)
#include <sys/procctl.h>
#include <signal.h>
#include <sys/wait.h>
#else
#include <wait.h>
#endif

#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/types.h>
#if defined(OS_LINUX)
#include <sys/prctl.h>
#endif
#include <limits.h>
#include <unistd.h>
#include <libgen.h>

#define SCE_SCRIPT "oscap-run-sce-script"

struct sce_check_result
{
	char* href;
	char* basename;
	char* std_out;
	char* std_err;
	int exit_code;
	struct oscap_stringlist* environment_variables;
	xccdf_test_result_type_t xccdf_result;
};

struct sce_check_result* sce_check_result_new(void)
{
	struct sce_check_result* ret = malloc(sizeof(struct sce_check_result));
	ret->href = NULL;
	ret->basename = NULL;
	ret->std_out = NULL;
	ret->std_err = NULL;
	ret->environment_variables = oscap_stringlist_new();
	ret->xccdf_result = XCCDF_RESULT_UNKNOWN;

	return ret;
}

void sce_check_result_free(struct sce_check_result* v)
{
	if (!v)
		return;

	free(v->href);
	free(v->basename);
	free(v->std_out);
	free(v->std_err);

	oscap_stringlist_free(v->environment_variables);

	free(v);
}

void sce_check_result_set_href(struct sce_check_result* v, const char* href)
{
	free(v->href);
	v->href = oscap_strdup(href);
}

const char* sce_check_result_get_href(struct sce_check_result* v)
{
	return v->href;
}

void sce_check_result_set_basename(struct sce_check_result* v, const char* base_name)
{
	free(v->basename);
	v->basename = oscap_strdup(base_name);
}

const char* sce_check_result_get_basename(struct sce_check_result* v)
{
	return v->basename;
}

void sce_check_result_set_stdout(struct sce_check_result* v, const char* _stdout)
{
	free(v->std_out);
	v->std_out = oscap_strdup(_stdout);
}

const char* sce_check_result_get_stdout(struct sce_check_result* v)
{
	return v->std_out;
}

void sce_check_result_set_stderr(struct sce_check_result* v, const char* _stderr)
{
	free(v->std_err);
	v->std_err = oscap_strdup(_stderr);
}

const char* sce_check_result_get_stderr(struct sce_check_result* v)
{
	return v->std_err;
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
	size_t ret = fwrite(v->std_out, 1, strlen(v->std_out), f);
	if (ret < strlen(v->std_out))
		oscap_seterr(OSCAP_EFAMILY_SCE, "Failed to write stdout result to %s.\n", target_file);
	fprintf(f, "\t]]></sceres:stdout>\n");
	fprintf(f, "\t<sceres:stderr><![CDATA[\n");
	ret = fwrite(v->std_err, 1, strlen(v->std_err), f);
	if (ret < strlen(v->std_err))
		oscap_seterr(OSCAP_EFAMILY_SCE, "Failed to write stderr result to %s.\n", target_file);
	fprintf(f, "\t]]></sceres:stderr>\n");
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
	struct sce_session* ret = malloc(sizeof(struct sce_session));
	ret->results = oscap_list_new();

	return ret;
}

void sce_session_free(struct sce_session* s)
{
	if (!s)
		return;

	oscap_list_free(s->results, (oscap_destruct_func) sce_check_result_free);
	free(s);
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
		free(target);
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
	struct sce_parameters *ret = malloc(sizeof(struct sce_parameters));
	ret->xccdf_directory = NULL;
	ret->session = NULL;

	return ret;
}

void sce_parameters_free(struct sce_parameters* v)
{
	if (!v)
		return;

	free(v->xccdf_directory);
	sce_session_free(v->session);

	free(v);
}

void sce_parameters_set_xccdf_directory(struct sce_parameters* v, const char* value)
{
	if (v->xccdf_directory)
		free(v->xccdf_directory);

	v->xccdf_directory = oscap_strdup(value);
}

const char* sce_parameters_get_xccdf_directory(struct sce_parameters* v)
{
	return v->xccdf_directory;
}

void sce_parameters_set_session(struct sce_parameters* v, struct sce_session* value)
{
	sce_session_free(v->session);
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

static void _pipe_try_read_into_string(int fd, struct oscap_string *string, bool *eof)
{
	// FIXME: Read by larger chunks in the future
	char readbuf;
	while (true) {
		const int read_status = read(fd, &readbuf, 1);
		if (read_status == 1) {  // successful read
			if (readbuf == '&') {
				// & is a special case, we have to "escape" it manually
				// (all else will eventually get handled by libxml)
				oscap_string_append_string(string, "&amp;");
			} else {
				oscap_string_append_char(string, readbuf);
			}
		}
		else if (read_status == 0) {  // EOF
			*eof = true;
			break;
		}
		else {
			if (errno == EAGAIN) {
				// NOOP, we are waiting for more input
				break;
			}
			else {
				*eof = true;  // signal EOF to exit the loops
				break;
			}
		}
	}
}


static void free_env_values(char **env_values, size_t index_of_first_env_value_not_compiled_in, size_t real_env_values_count) {
	for (size_t i = index_of_first_env_value_not_compiled_in; i < real_env_values_count; i++) {
		free(env_values[i]);
	}
	free(env_values);
}


xccdf_test_result_type_t sce_engine_eval_rule(struct xccdf_policy *policy, const char *rule_id, const char *id, const char *href,
		struct xccdf_value_binding_iterator *value_binding_it,
		struct xccdf_check_import_iterator *check_import_it,
		void *usr)
{
	dI("Executing SCE check '%s'", href);
	struct sce_parameters* parameters = (struct sce_parameters*)usr;
	const char* xccdf_directory = parameters->xccdf_directory;
	bool use_sce_wrapper = false; // use osca-run-sce-script ?

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
		free(tmp_href);
		return XCCDF_RESULT_NOT_CHECKED;
	}

	if (access(tmp_href, F_OK | X_OK))
	{
		// use the sce wrapper if it's not possible to acquire +x rights
		use_sce_wrapper = true;
		dI("%s isn't executable, oscap-run-sce-script will be used.", tmp_href);
	}

	// all the result codes are shifted by 100, because otherwise syntax errors in scripts
	// or even their nonexistence would cause XCCDF_RESULT_PASS to be the result

	char* argvp[3] = {
		tmp_href,
		tmp_href, // the second tmp_href is added in case we use the wrapper (oscap-run-sce-script)
		NULL      // which need the path of the script to eval as first parameter.
	};

	// bound values in KEY=VALUE form, ready to be passed as environment variables
	char ** env_values = malloc(10 * sizeof(char * ));
	size_t env_value_count = 10;
	const size_t index_of_first_env_value_not_compiled_in = 10;

	env_values[0] = "PATH=/bin:/sbin:/usr/bin:/usr/local/bin:/usr/sbin";

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

		void *new_env_values = realloc(env_values, (env_value_count + 3) * sizeof(char *));
		if (new_env_values == NULL) {
			dE("Unable to re-allocate memory");
			free_env_values(env_values, index_of_first_env_value_not_compiled_in, env_value_count);
			return XCCDF_RESULT_ERROR;
		}
		env_values = new_env_values;

		char* name = xccdf_value_binding_get_name(binding);
		xccdf_value_type_t type = xccdf_value_binding_get_type(binding);
		char* value = xccdf_value_binding_get_setvalue(binding);
		if (value == NULL)
		{
			value = xccdf_value_binding_get_value(binding);
			if (value == NULL) {
				value = "";
			}
		}
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

	void *new_env_values = realloc(env_values, (env_value_count + 1) * sizeof(char*));
	if (new_env_values == NULL) {
		dE("Unable to re-allocate memory");
		free_env_values(env_values, index_of_first_env_value_not_compiled_in, env_value_count);
		return XCCDF_RESULT_ERROR;
	}
	env_values = new_env_values;
	env_values[env_value_count] = NULL;

	// We open a pipe for communication with the forked process
	int stdout_pipefd[2];
	int stderr_pipefd[2];
	if (pipe(stdout_pipefd) == -1 || pipe(stderr_pipefd) == -1)
	{
		dE("Error in pipe");
		free_env_values(env_values, index_of_first_env_value_not_compiled_in, env_value_count);
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
			// we won't read from the pipes, so close the reading fd
			close(stdout_pipefd[0]);
			close(stderr_pipefd[0]);

			// forward stdout and stderr to our custom opened pipes
			dup2(stdout_pipefd[1], fileno(stdout));
			dup2(stderr_pipefd[1], fileno(stderr));

			// we duplicated the file descriptors twice, we can close the original
			// ones now, stdout and stderr will be closed properly after the execved
			// script/executable finishes
			close(stdout_pipefd[1]);
			close(stderr_pipefd[1]);

			// before we execute the script, lets make sure we get SIGTERM when
			// oscap is killed, crashes or otherwise terminates
#if defined(PR_SET_PDEATHSIG)
			// requires Linux 2.1.57 or later
			prctl(PR_SET_PDEATHSIG, SIGTERM);
#elif defined(OS_FREEBSD)
			int sig = SIGTERM;
			procctl(P_PID, getpid(), PROC_PDEATHSIG_CTL, &sig);
#else
			// TODO: Please provide alternatives
#endif

			// we are the child process
			if(use_sce_wrapper) {
#if defined(OS_FREEBSD)
				size_t k;

				// Setup environment beforehand as FreeBSD does not have execvpe()
				for (k = 0; k < env_value_count; k++) {
					putenv(env_values[k]);
				}

				execvp("oscap-run-sce-script", argvp);
#else
				execvpe("oscap-run-sce-script", argvp, env_values);
#endif
			} else {
				execve(tmp_href, argvp, env_values);
			}

			free_env_values(env_values, index_of_first_env_value_not_compiled_in, env_value_count);

			// no need to check the return value of execve, if it returned at all we are in trouble
			printf("Unexpected error when executing script '%s'. Error message follows.\n", href);
			perror("execve");

			// the parent process considers us a script check, we have to return a value that will mean XCCDF_RESULT_ERROR
			exit(103);
		}
		else
		{
			// we won't write to the pipes, so close the writing fd
			close(stdout_pipefd[1]);
			close(stderr_pipefd[1]);

			const int flag_stdout = fcntl(stdout_pipefd[0], F_GETFL, 0);
			if (flag_stdout == -1) {
				oscap_seterr(OSCAP_EFAMILY_SCE, "Failed to obtain status of stdout pipe: %s",
						strerror(errno));
				free_env_values(env_values, index_of_first_env_value_not_compiled_in, env_value_count);
				return XCCDF_RESULT_ERROR;
			}
			int retval = fcntl(stdout_pipefd[0], F_SETFL, flag_stdout | O_NONBLOCK);
			if (retval == -1) {
				oscap_seterr(OSCAP_EFAMILY_SCE,
						"Failed to set nonblocking flag on stdout pipe: %s",
						strerror(errno));
				free_env_values(env_values, index_of_first_env_value_not_compiled_in, env_value_count);
				return XCCDF_RESULT_ERROR;
			}

			const int flag_stderr = fcntl(stderr_pipefd[0], F_GETFL, 0);
			if (flag_stderr == -1) {
				oscap_seterr(OSCAP_EFAMILY_SCE,
						"Failed to obtain status of stderr pipe: %s",
						strerror(errno));
				free_env_values(env_values, index_of_first_env_value_not_compiled_in, env_value_count);
				return XCCDF_RESULT_ERROR;
			}
			retval = fcntl(stderr_pipefd[0], F_SETFL, flag_stderr | O_NONBLOCK);
			if (retval == -1) {
				oscap_seterr(OSCAP_EFAMILY_SCE,
						"Failed to set nonblocking flag on stderr pipe: %s",
						strerror(errno));
				free_env_values(env_values, index_of_first_env_value_not_compiled_in, env_value_count);
				return XCCDF_RESULT_ERROR;
			}

			// we have to read from both pipes at the same time to avoid stalling
			struct oscap_string *stdout_string = oscap_string_new();
			struct oscap_string *stderr_string = oscap_string_new();

			bool stdout_eof = false;
			bool stderr_eof = false;

			while (!stdout_eof || !stderr_eof) {
				if (!stdout_eof)
					_pipe_try_read_into_string(stdout_pipefd[0], stdout_string, &stdout_eof);

				if (!stderr_eof)
					_pipe_try_read_into_string(stderr_pipefd[0], stderr_string, &stderr_eof);

				// sleep for 10ms to avoid wasting CPU
				usleep(10 * 1000);
			}

			close(stdout_pipefd[0]);
			close(stderr_pipefd[0]);

			char *stdout_buffer = oscap_string_bequeath(stdout_string);
			char *stderr_buffer = oscap_string_bequeath(stderr_string);

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
				char *base_name = oscap_basename(tmp_href);
				sce_check_result_set_basename(check_result, base_name);
				free(base_name);
				sce_check_result_set_stdout(check_result, stdout_buffer);
				sce_check_result_set_stderr(check_result, stderr_buffer);
				sce_check_result_set_exit_code(check_result, WEXITSTATUS(wstatus));
				sce_check_result_set_xccdf_result(check_result, (xccdf_test_result_type_t)raw_result);

				for (size_t i = 0; i < env_value_count; ++i)
				{
					sce_check_result_add_environment_variable(check_result, env_values[i]);
				}

				sce_session_add_check_result(session, check_result);
			}

			free_env_values(env_values, index_of_first_env_value_not_compiled_in, env_value_count);

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
				else if (strcmp(name, "stderr") == 0)
				{
					xccdf_check_import_set_content(check_import, stderr_buffer);
				}
			}

			free(tmp_href);
			free(stdout_buffer);
			free(stderr_buffer);

			return (xccdf_test_result_type_t)raw_result;
		}
	}
	else
	{
		free_env_values(env_values, index_of_first_env_value_not_compiled_in, env_value_count);

		close(stdout_pipefd[0]);
		close(stdout_pipefd[1]);
		close(stderr_pipefd[0]);
		close(stderr_pipefd[1]);
		return XCCDF_RESULT_ERROR;
	}
}

bool xccdf_policy_model_register_engine_sce(struct xccdf_policy_model * model, struct sce_parameters *parameters)
{
	return xccdf_policy_model_register_engine_and_query_callback(model,
		"http://open-scap.org/page/SCE", sce_engine_eval_rule, (void*)parameters, NULL);
}
