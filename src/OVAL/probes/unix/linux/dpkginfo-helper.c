#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>

#include "debug_priv.h"
#include "dpkginfo-helper.h"

#define DPKG_STATUS_BUFFER_SIZE 4096

static char* trimleft(char *str)
{
	while (isspace((unsigned char)*str))
		str++;

	if (*str == 0)
		return str;

	return str;
}

static int version(struct dpkginfo_reply_t *reply)
{
	char *evr, *epoch, *version, *release;

	if (reply->evr == NULL)
		return -1;

	evr = strdup(reply->evr);
	if (evr == NULL)
		return -1;

	if ((epoch = strchr(evr, ':')) != NULL) {
		*epoch++ = '\0';
		reply->epoch = strdup(evr);
		if (reply->epoch == NULL)
			goto err;
	} else {
		reply->epoch = strdup("0");
		if (reply->epoch == NULL)
			goto err;
		epoch = evr;
	}

	version = epoch;
	if ((release = strchr(version, '-')) != NULL) {
		*release++ = '\0';
		reply->release = strdup(release);
		if (reply->release == NULL)
			goto err;
	}
	reply->version = strdup(version);
	if (reply->version == NULL)
		goto err;

	free(evr);
	return 0;
err:
	free(evr);
	return -1;
}

static void dpkginfo_clear_reply(struct dpkginfo_reply_t *reply)
{
	if (reply == NULL)
		return;
	free(reply->name);
	free(reply->arch);
	free(reply->epoch);
	free(reply->release);
	free(reply->version);
	free(reply->evr);
	memset(reply, 0, sizeof(*reply));
}

static char *strdup_nullable(const char *str)
{
	return str == NULL ? NULL : strdup(str);
}

static int dpkginfo_copy_reply(struct dpkginfo_reply_t *dst, const struct dpkginfo_reply_t *src, const char *name)
{
	memset(dst, 0, sizeof(*dst));
	dst->name = strdup(name);
	dst->arch = strdup_nullable(src->arch);
	dst->epoch = strdup_nullable(src->epoch);
	dst->release = strdup_nullable(src->release);
	dst->version = strdup_nullable(src->version);
	dst->evr = strdup_nullable(src->evr);

	if (dst->name == NULL || (src->arch != NULL && dst->arch == NULL) ||
			(src->epoch != NULL && dst->epoch == NULL) ||
			(src->release != NULL && dst->release == NULL) ||
			(src->version != NULL && dst->version == NULL) ||
			(src->evr != NULL && dst->evr == NULL)) {
		dpkginfo_clear_reply(dst);
		return -1;
	}

	return 0;
}

static int append_reply(struct dpkginfo_reply_t **replies, size_t *reply_count, const struct dpkginfo_reply_t *reply, const char *name)
{
	void *new_replies = realloc(*replies, (*reply_count + 1) * sizeof(**replies));
	if (new_replies == NULL)
		return -1;

	*replies = new_replies;
	if (dpkginfo_copy_reply(*replies + *reply_count, reply, name) != 0)
		return -1;

	(*reply_count)++;
	return 0;
}

static int append_string(char ***strings, size_t *string_count, const char *str)
{
	void *new_strings = realloc(*strings, (*string_count + 1) * sizeof(**strings));
	if (new_strings == NULL)
		return -1;

	*strings = new_strings;
	(*strings)[*string_count] = strdup(str);
	if ((*strings)[*string_count] == NULL)
		return -1;

	(*string_count)++;
	return 0;
}

static void free_strings(char **strings, size_t string_count)
{
	for (size_t i = 0; i < string_count; i++)
		free(strings[i]);
	free(strings);
}

static int finish_package(const struct dpkginfo_reply_t *package, bool not_installed,
		char **provides, size_t provides_count, struct dpkginfo_reply_t **replies, size_t *reply_count)
{
	if (package->name == NULL || not_installed)
		return 0;

	if (append_reply(replies, reply_count, package, package->name) != 0)
		return -1;

	for (size_t i = 0; i < provides_count; i++) {
		if (append_reply(replies, reply_count, package, provides[i]) != 0)
			return -1;
	}

	return 0;
}

static int add_provides(char *value, char ***provides, size_t *provides_count)
{
	char *state = NULL;
	char *provider = strtok_r(value, ",", &state);

	while (provider != NULL) {
		provider = trimleft(provider);
		char *version_separator = strchr(provider, ' ');
		if (version_separator != NULL)
			*version_separator = '\0';
		if (*provider != '\0' && append_string(provides, provides_count, provider) != 0)
			return -1;
		provider = strtok_r(NULL, ",", &state);
	}

	return 0;
}

struct dpkginfo_reply_t *dpkginfo_get_all(size_t *reply_count, int *err)
{
	FILE *f;
	char buf[DPKG_STATUS_BUFFER_SIZE], path[PATH_MAX], *root, *key, *value;
	struct dpkginfo_reply_t package = { 0 };
	struct dpkginfo_reply_t *replies = NULL;
	char **provides = NULL;
	size_t provides_count = 0;
	bool not_installed = false;

	*err = 0;
	*reply_count = 0;

	root = getenv("OSCAP_PROBE_ROOT");
	if (root != NULL)
		snprintf(path, PATH_MAX, "%s/var/lib/dpkg/status", root);
	else
		snprintf(path, PATH_MAX, "/var/lib/dpkg/status");

	f = fopen(path, "r");
	if (f == NULL) {
		dW("%s not found.", path);
		*err = -1;
		return NULL;
	}

	while (fgets(buf, DPKG_STATUS_BUFFER_SIZE, f)) {
		if (buf[0] == '\n') {
			// New package entry.
			if (finish_package(&package, not_installed, provides, provides_count, &replies, reply_count) != 0)
				goto err;
			dpkginfo_clear_reply(&package);
			free_strings(provides, provides_count);
			provides = NULL;
			provides_count = 0;
			not_installed = false;
			continue;
		}
		if (isspace(buf[0])) {
			// Ignore line beginning by a space.
			continue;
		}
		buf[strcspn(buf, "\n")] = 0;
		key = buf;
		value = strchr(buf, ':');
		if (value == NULL) {
			// Ignore truncated line.
			continue;
		}
		*value++ = '\0';
		value = trimleft(value);
		// Package should be the first line.
		if (strcmp(key, "Package") == 0) {
			if (finish_package(&package, not_installed, provides, provides_count, &replies, reply_count) != 0)
				goto err;
			dpkginfo_clear_reply(&package);
			free_strings(provides, provides_count);
			provides = NULL;
			provides_count = 0;
			not_installed = false;

			package.name = strdup(value);
			if (package.name == NULL)
				goto err;
		} else if (package.name != NULL) {
			if (strcmp(key, "Status") == 0) {
				not_installed = strncmp(value, "install", 7) != 0;
			} else if (strcmp(key, "Architecture") == 0) {
				package.arch = strdup(value);
				if (package.arch == NULL)
					goto err;
			} else if (strcmp(key, "Version") == 0) {
				package.evr = strdup(value);
				if (package.evr == NULL)
					goto err;
				if (version(&package) < 0)
					goto err;
			} else if (strcmp(key, "Provides") == 0) {
				if (add_provides(value, &provides, &provides_count) != 0)
					goto err;
			}
		}
	}

	// Reached end of file.
	if (finish_package(&package, not_installed, provides, provides_count, &replies, reply_count) != 0)
		goto err;

	if (*reply_count > 0) {
		dD("Found %zu dpkg package entries.", *reply_count);
		*err = 1;
	}
	dpkginfo_clear_reply(&package);
	free_strings(provides, provides_count);
	fclose(f);
	return replies;
err:
	dW("Insufficient memory available to allocate duplicate string.");
	fclose(f);
	dpkginfo_clear_reply(&package);
	free_strings(provides, provides_count);
	dpkginfo_free_replies(replies, *reply_count);
	*reply_count = 0;
	*err = -1;
	return NULL;
}

struct dpkginfo_reply_t* dpkginfo_get_by_name(const char *name, int *err)
{
	size_t reply_count = 0;
	struct dpkginfo_reply_t *replies = dpkginfo_get_all(&reply_count, err);

	if (replies == NULL)
		return NULL;

	for (size_t i = 0; i < reply_count; i++) {
		if (strcmp(replies[i].name, name) == 0) {
			struct dpkginfo_reply_t *reply = malloc(sizeof(*reply));
			if (reply == NULL)
				goto err;
			*reply = replies[i];
			memset(replies + i, 0, sizeof(replies[i]));
			dpkginfo_free_replies(replies, reply_count);
			*err = 1;
			return reply;
		}
	}

	dpkginfo_free_replies(replies, reply_count);
	*err = 0;
	return NULL;

err:
	dpkginfo_free_replies(replies, reply_count);
	*err = -1;
	return NULL;
}

void dpkginfo_free_reply(struct dpkginfo_reply_t *reply)
{
	if (reply) {
		dpkginfo_clear_reply(reply);
		free(reply);
	}
}

void dpkginfo_free_replies(struct dpkginfo_reply_t *replies, size_t reply_count)
{
	for (size_t i = 0; i < reply_count; i++)
		dpkginfo_clear_reply(replies + i);
	free(replies);
}
