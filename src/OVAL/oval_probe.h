#pragma once
#ifndef OVAL_PROBE_H
#define OVAL_PROBE_H

#define OVAL_PROBE_DIR "/usr/libexec/openscap/oval"

typedef struct {
        oval_subtype_enum typenum;
        char             *typestr;
        char             *path;
} oval_probe_t;

#endif /* OVAL_PROBE_H */
