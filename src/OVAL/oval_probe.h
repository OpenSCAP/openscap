#pragma once
#ifndef OVAL_PROBE_H
#define OVAL_PROBE_H

#include "oval_definitions_impl.h"
#include "oval_agent_api_impl.h"
#include "oval_parser_impl.h"
#include "oval_string_map_impl.h"

#define OVAL_PROBE_DIR "/usr/libexec/openscap/oval"

typedef struct {
        oval_subtype_enum typenum;
        char             *typestr;
        char             *path;
} oval_probe_t;

#endif /* OVAL_PROBE_H */
