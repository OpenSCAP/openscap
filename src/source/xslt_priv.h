/*
 * Copyright 2014 Red Hat Inc., Durham, North Carolina.
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
 *
 */
#ifndef OSCAP_SOURCE_XSLT_H
#define OSCAP_SOURCE_XSLT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "common/public/oscap.h"
#include "common/util.h"
#include "source/public/oscap_source.h"

OSCAP_HIDDEN_START;

/**
 * Apply stylesheet on XML file (represented by oscap_source structure). If xsltfile
 * is an absolute path to the stylesheet, path_to_xslt will not be used.
 * @memberof oscap_source
 * @param source oscap_surce structure representing file to transform
 * @param xsltfile absolute path to the stylesheet document or relative given the path_to_xslt
 * @param outfile output filename
 * @param params external params for xsl transformation
 * @param path_to_xslt optional path to xsl transformations
 * @returns 0 on success
 */
int oscap_source_apply_xslt_path(struct oscap_source *source, const char *xsltfile, const char *outfile, const char **params, const char *path_to_xslt);

/**
 * Apply stylesheet on XML file (represented by oscap_source structure). If xsltfile
 * is an absolute path to the stylesheet, path_to_xslt will not be used.
 * @memberof oscap_source
 * @param source oscap_surce structure representing file to transform
 * @param xsltfile absolute path to the stylesheet document or relative given the path_to_xslt
 * @param params external params for xsl transformation
 * @param path_to_xslt optional path to xsl transformations
 * @returns newly allocated buffer containing result of XSLT application. NULL is returned on error
 */
char *oscap_source_apply_xslt_path_mem(struct oscap_source *source, const char *xsltfile, const char **params, const char *path_to_xslt);

OSCAP_HIDDEN_END;
#endif
