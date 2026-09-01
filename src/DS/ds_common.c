/*
 * Copyright 2013 Red Hat Inc., Durham, North Carolina.
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
 *      Martin Preisler <mpreisle@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "ds_common.h"
#include "common/_error.h"
#include "common/list.h"
#include "common/oscap_acquire.h"
#include "source/oscap_source_priv.h"
#include "source/public/oscap_source.h"
#include "oscap_helpers.h"

#include <libxml/tree.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#ifdef OS_WINDOWS
#include <io.h>
#else
#include <unistd.h>
#endif

#ifndef OS_WINDOWS
#ifndef O_NOFOLLOW
#error Secure SDS output requires O_NOFOLLOW
#endif
#endif

xmlDoc *ds_doc_from_foreign_node(xmlNode *node, xmlDoc *parent)
{
	xmlDOMWrapCtxtPtr wrap_ctxt = xmlDOMWrapNewCtxt();
	xmlDocPtr new_doc = xmlNewDoc(BAD_CAST "1.0");
	xmlNodePtr res_node = NULL;
	if (xmlDOMWrapCloneNode(wrap_ctxt, parent, node, &res_node, new_doc, NULL, 1, 0) != 0)
	{
		oscap_seterr(OSCAP_EFAMILY_XML, "Error when cloning node '%s' while dumping component "
				"from DataStream", node->name);
			xmlFreeDoc(new_doc);
			xmlDOMWrapFreeCtxt(wrap_ctxt);
			return NULL;
	}
	xmlDocSetRootElement(new_doc, res_node);
	if (xmlDOMWrapReconcileNamespaces(wrap_ctxt, res_node, 0) != 0)
	{
		oscap_seterr(OSCAP_EFAMILY_XML, "Internal libxml error when reconciling namespaces "
				"for node '%s' while dumping component.", node->name);
		xmlFreeDoc(new_doc);
		xmlDOMWrapFreeCtxt(wrap_ctxt);
		return NULL;
	}
	xmlDOMWrapFreeCtxt(wrap_ctxt);
	return new_doc;
}

bool ds_path_is_null(const char *path) {
	return path == NULL || *path == '\0';
}

bool ds_path_has_upward_component(const char *path)
{
	if (*path == '/' || *path == '\\') {
		return true;
	}
	const char *start = path;
	while (*start) {
		const char *end = start;
		while (*end != '\0' && *end != '/' && *end != '\\')
			end++;
		if ((end - start) == 2 && start[0] == '.' && start[1] == '.')
			return true;
		start = (*end == '\0') ? end : end + 1;
	}
	return false;
}

/* A component name is a path relative to the output directory, not a path to
 * resolve against the host filesystem. Reject it before any component is
 * registered (including SCE scripts, which are written during registration). */
bool ds_path_is_valid_filename(const char *path)
{
	if (ds_path_is_null(path) || ds_path_has_upward_component(path) ||
			path[strlen(path) - 1] == '/' || path[strlen(path) - 1] == '\\')
		return false;
	char *copy = oscap_strdup(path);
	char *base = oscap_basename(copy);
	bool valid = !ds_path_is_null(base) && strcmp(base, ".") != 0;
	free(base);
	free(copy);
	return valid;
}

int ds_write_file(const char *target_dir, const char *relative_path, int mode,
		int (*write_contents)(int fd, void *data), void *data)
{
	if (!ds_path_is_valid_filename(relative_path)) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Invalid component output path '%s'.", relative_path ? relative_path : "(null)");
		return -1;
	}
	if (oscap_acquire_mkdir_p(target_dir) != 0)
		return -1;

#ifndef OS_WINDOWS
	int dirfd = open(target_dir, O_RDONLY | O_DIRECTORY | O_NOFOLLOW);
	if (dirfd < 0)
		goto open_error;

	char *path = oscap_strdup(relative_path);
	char *part = path;
	char *slash;
	while ((slash = strchr(part, '/')) != NULL) {
		*slash = '\0';
		if (*part && strcmp(part, ".") != 0) {
			if (mkdirat(dirfd, part, 0700) != 0 && errno != EEXIST)
				goto path_error;
			int next = openat(dirfd, part, O_RDONLY | O_DIRECTORY | O_NOFOLLOW);
			if (next < 0)
				goto path_error;
			close(dirfd);
			dirfd = next;
		}
		part = slash + 1;
	}
	if (*part == '\0' || strcmp(part, ".") == 0) {
		errno = EINVAL;
		goto path_error;
	}

	/* Create a new inode rather than truncating a destination that might be a
	 * symlink or hard link. renameat replaces a symlink, never its target. */
	char temporary[80];
	int fd = -1;
	static unsigned int next_temp;
	for (unsigned int i = 0; i < 100; ++i) {
		unsigned int serial = __sync_fetch_and_add(&next_temp, 1);
		snprintf(temporary, sizeof(temporary), ".oscap-sds-%ld-%u", (long)getpid(), serial);
		fd = openat(dirfd, temporary, O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW, 0600);
		if (fd >= 0 || errno != EEXIST)
			break;
	}
	if (fd < 0)
		goto path_error;

	int ret = write_contents(fd, data);
	/* Scripts are executable only after their contents have been written.
	 * Keep XML private rather than making the new file writable by others. */
	if (ret == 0 && mode == 0700 && fchmod(fd, mode) != 0)
		ret = -1;
	if (close(fd) != 0)
		ret = -1;
	if (ret == 0 && renameat(dirfd, temporary, dirfd, part) != 0)
		ret = -1;
	if (ret != 0) {
		oscap_seterr(OSCAP_EFAMILY_GLIBC, "Could not write component '%s': %s.", relative_path, strerror(errno));
		unlinkat(dirfd, temporary, 0);
	}
	free(path);
	close(dirfd);
	return ret;

path_error:
	oscap_seterr(OSCAP_EFAMILY_GLIBC, "Could not create component path '%s': %s.", relative_path, strerror(errno));
	free(path);
	close(dirfd);
	return -1;
open_error:
	oscap_seterr(OSCAP_EFAMILY_GLIBC, "Could not open output directory '%s': %s.", target_dir, strerror(errno));
	return -1;
#else
	/* Windows does not provide the dirfd interfaces. Keep the same relative
	 * name validation and use the platform's regular output handling. */
	char *filename = oscap_sprintf("%s/%s", target_dir, relative_path);
	if (oscap_acquire_ensure_parent_dir(filename) != 0) {
		free(filename);
		return -1;
	}
	int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, mode);
	int ret = fd < 0 ? -1 : write_contents(fd, data);
	if (fd >= 0 && close(fd) != 0)
		ret = -1;
	free(filename);
	return ret;
#endif
}

static int ds_write_xml(int fd, void *data)
{
	return oscap_source_to_fd(data, fd);
}

int ds_dump_component_sources(struct oscap_htable *component_sources, const char *target_dir)
{
	struct oscap_htable_iterator *hit = oscap_htable_iterator_new(component_sources);
	while (oscap_htable_iterator_has_more(hit)) {
		struct oscap_source *s = oscap_htable_iterator_next_value(hit);
		char *filename;
		if (target_dir == NULL) {
			filename = oscap_strdup(oscap_source_readable_origin(s));
		} else {
			int ret = ds_write_file(target_dir, oscap_source_readable_origin(s), 0600, ds_write_xml, s);
			if (ret != 0) {
				oscap_htable_iterator_free(hit);
				return ret;
			}
			continue;
		}
		int ret = oscap_acquire_ensure_parent_dir(filename);
		if (ret != 0) {
			free(filename);
			oscap_htable_iterator_free(hit);
			return ret;
		}
		ret = oscap_source_save_as(s, filename);
		free(filename);
		if (ret != 0) {
			oscap_htable_iterator_free(hit);
			return ret;
		}
	}
	oscap_htable_iterator_free(hit);
	return 0;
}
