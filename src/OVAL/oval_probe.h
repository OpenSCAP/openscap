#pragma once
#ifndef OVAL_PROBE_H
#define OVAL_PROBE_H

#include "oval_definitions_impl.h"
#include "oval_agent_api_impl.h"
#include "oval_parser_impl.h"
#include "oval_string_map_impl.h"
#include "api/oval_system_characteristics.h"

#define OVAL_PROBE_DIR "/usr/libexec/openscap/oval"

typedef struct {
        oval_subtype_enum typenum;
        char             *typestr;
        char             *path;
} oval_probe_t;

struct oval_iterator_syschar *sexp_to_oval_state (SEXP_t *sexp);

#endif /* OVAL_PROBE_H */
