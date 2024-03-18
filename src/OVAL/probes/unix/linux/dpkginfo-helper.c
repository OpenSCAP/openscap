#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

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

struct dpkginfo_reply_t* dpkginfo_get_by_name(const char *name, int *err)
{
	FILE *f;
	char buf[DPKG_STATUS_BUFFER_SIZE], path[PATH_MAX], *root, *key, *value;
	struct dpkginfo_reply_t *reply;

	*err = 0;
	reply = NULL;

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

	dD("Searching package \"%s\".", name);

	while (fgets(buf, DPKG_STATUS_BUFFER_SIZE, f)) {
		if (buf[0] == '\n') {
			// New package entry.
			if (reply != NULL) {
				// Package found.
				goto out;
			}
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
			if (strcmp(value, name) == 0) {
				if (reply != NULL)
					continue;
				reply = calloc(1, sizeof(*reply));
				if (reply == NULL)
					goto err;
				reply->name = strdup(value);
				if (reply->name == NULL)
					goto err;
			}
		} else if (reply != NULL) {
			if (strcmp(key, "Status") == 0) {
				if (strncmp(value, "install", 7) != 0) {
					// Package deinstalled.
					dD("Package \"%s\" has been deinstalled.", name);
					dpkginfo_free_reply(reply);
					reply = NULL;
					continue;
				}
			} else if (strcmp(key, "Architecture") == 0) {
				reply->arch = strdup(value);
				if (reply->arch == NULL)
					goto err;
			} else if (strcmp(key, "Version") == 0) {
				reply->evr = strdup(value);
				if (reply->evr == NULL)
					goto err;
				if (version(reply) < 0)
					goto err;
			}
		}
	}

	// Reached end of file.

out:
	if (reply != NULL) {
		// Package found.
		dD("Package \"%s\" found (arch=%s evr=%s epoch=%s version=%s release=%s).",
			name, reply->arch, reply->evr, reply->epoch, reply->version, reply->release);
		*err = 1;
	}
	fclose(f);
	return reply;
err:
	dW("Insufficient memory available to allocate duplicate string.");
	fclose(f);
	dpkginfo_free_reply(reply);
	*err = -1;
	return NULL;
}

void dpkginfo_free_reply(struct dpkginfo_reply_t *reply)
{
	if (reply) {
		free(reply->name);
		free(reply->arch);
		free(reply->epoch);
		free(reply->release);
		free(reply->version);
		free(reply->evr);
		free(reply);
	}
}
