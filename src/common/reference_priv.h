/*
 * Copyright 2010 Red Hat Inc., Durham, North Carolina.
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
 *       Lukas Kuklinek <lkuklinek@redhat.com>
 */


#pragma once
#ifndef OSCAP_REFERENCE_PRIV_H_
#define OSCAP_REFERENCE_PRIV_H_


#include "util.h"
#include "public/reference.h"
#include "elements.h"
#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

struct oscap_reference {
    char *title;
    char *creator;
    char *subject;
    char *description;
    char *publisher;
    char *contributor;
    char *date;
    char *type;
    char *format;
    char *identifier;
    char *source;
    char *language;
    char *relation;
    char *coverage;
    char *rights;
    bool is_dublincore;
    char *href;
};

xmlNode *oscap_reference_to_dom(struct oscap_reference *ref, xmlNode *parent, xmlDoc *doc, const char *elname);
struct oscap_reference *oscap_reference_new_parse(xmlTextReaderPtr reader);

#endif // OSCAP_REFERENCE_PRIV_H_

