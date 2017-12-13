#ifndef OVAL_VERSION_H
#define OVAL_VERSION_H

#include <stdint.h>
#include <stddef.h>
#include <oscap.h>
#include "oscap_export.h"

typedef uint32_t oval_version_t;

#define OVAL_VERSION_INVALID 0

/**
 * Convert the string representation of the version to the
 * internal representation
 * Returns
 * OVAL_VERSION_INVALID ... if the string representation
 *                          is not a valid OVAL version
 */
OSCAP_API OSCAP_DEPRECATED(oval_version_t oval_version_from_cstr(const char *version_str));

#define OVAL_VERSION(v) oval_version_from_cstr(#v)

/**
 * Convert the internal representation of the version to the
 * string representation
 * Returns
 *  0 ... on success
 *  1 ... no major component
 * -1 ... insufficient buffer space
 */
OSCAP_API OSCAP_DEPRECATED(int oval_version_to_cstr(oval_version_t version, char *buffer, size_t buflen));

/**
 * Get the major component of the version
 */
OSCAP_API OSCAP_DEPRECATED(uint8_t oval_version_major(oval_version_t version));

/**
 * Get the minor component of the version
 */
OSCAP_API OSCAP_DEPRECATED(uint8_t oval_version_minor(oval_version_t version));

/**
 * Get the patch component of the version
 */
OSCAP_API OSCAP_DEPRECATED(uint8_t oval_version_patch(oval_version_t version));

/**
 * Compare two versions in the internal representation
 * Returns
 *   0 ... if the versions are the same
 *  <0 ... if `v1' is older than `v2'
 *  >0 ... if `v1' is newer than `v2'
 */
OSCAP_API OSCAP_DEPRECATED(int oval_version_cmp(oval_version_t v1, oval_version_t v2));

#endif /* OVAL_VERSION_H */
