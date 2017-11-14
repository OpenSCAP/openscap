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
#ifndef OSCAP_SOURCE_BZIP2_H
#define OSCAP_SOURCE_BZIP2_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "common/public/oscap.h"
#include "common/util.h"
#include <libxml/tree.h>

OSCAP_HIDDEN_START;

#ifdef BZIP2_FOUND

/**
 * Parse *.xml.bz2 file to XML DOM
 * @param fd The file descriptor to bz2 file
 * @returns DOM representation of the file
 */
xmlDoc *bz2_fd_read_doc(int fd);

/**
 * Parse bzip2ed memory to XML DOM.
 * @param buffer data in memory to process (contains bzip2ed XML)
 * @param size length of data
 * @returns DOM representation of the data
 */
xmlDoc *bz2_mem_read_doc(const char *buffer, size_t size);

#endif // BZIP2_FOUND

/**
 * Recognize whether the file can be parsed by this
 * bz2 parser. Do not close the file.
 * @param file descriptor to opened file
 * @returns true if can be parsed.
 */
bool bz2_fd_is_bzip(int fd);

/**
 * @brief Recognize whether the file can be parsed by this
 * bz2 parser
 * @param memory Raw memory with file content
 * @param size Size of memory
 * @return true if can be parsed
 */
bool bz2_memory_is_bzip(const char* memory, const size_t size);

OSCAP_HIDDEN_END;

#endif // OSCAP_SOURCE_BZIP2_H
