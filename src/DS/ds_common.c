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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:
 *      Martin Preisler <mpreisle@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "ds_common.h"
#include "common/_error.h"

#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <libxml/tree.h>

#ifndef MAXPATHLEN
#   define MAXPATHLEN 1024
#endif

int ds_common_mkdir_p(const char* path)
{
	// NOTE: This assumes a UNIX VFS path, C:\\folder\\folder would break it!

	if (strlen(path) > MAXPATHLEN) {
		return -1;
	}
	else {
		char temp[MAXPATHLEN + 1]; // +1 for \0
		unsigned int i;

		for (i = 0; i <= strlen(path); i++) {
			if (path[i] == '/' || path[i] == '\0') {
				strncpy(temp, path, i);
				temp[i] = '\0';

				// skip leading '/', we will never be creating the root anyway
				if (strlen(temp) == 0)
					continue;

				if (mkdir(temp, S_IRWXU) != 0 && errno != EEXIST) {
					oscap_seterr(OSCAP_EFAMILY_GLIBC,
						"Error making directory '%s', while doing recursive mkdir for '%s', error was '%s'.",
						temp, path, strerror(errno));
					return -1;
				}
			}
		}

		return 0;
	}
}

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
