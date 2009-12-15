#pragma once
#ifndef OSCAP_ERROR_H
#define OSCAP_ERROR_H

#include <stdint.h>
#include <stdbool.h>

typedef uint16_t oscap_errfamily_t;
typedef uint16_t oscap_errcode_t;

/* Error level (as defined in XML error handler library */
/* Maybe for future use                                 */
/********************************************************/
#define ERR_LEVEL_NONE      0   /* None */
#define ERR_LEVEL_WARNING   1   /* A simple warning */
#define ERR_LEVEL_ERROR     2   /* A recoverable error */
#define ERR_LEVEL_FATAL     3   /* A fatal error */

/* For oscap error family there are : */
/**************************************/
#define ERR_FAMILY_NONE     0   /* None */
#define ERR_FAMILY_GLIBC    1   /* Errno errors */
#define ERR_FAMILY_XML      2   /* Libxml - xmlerror errors */
#define ERR_FAMILY_OSCAP    3   /* OSCAP errors */

/* For oscap error of family */
/*****************************/

#define OSCAP_ENONE         0   /* None */
#define OSCAP_EALLOC        1   /* OSCAP allocation error*/
#define OSCAP_EXMLELEM      2   /* Unknown element in XML */
#define OSCAP_EXMLNODETYPE  3   /* Bad node type in XML */
#define OSCAP_EXMLNOELEMENT 4   /* No expected element */
#define OSCAP_ECPEINVOP     5   /* Invalid CPE Language operation */
#define OSCAP_EUSER1        201 /* User defined error */
#define OSCAP_EUSER2        202 /* User defined error */
#define OSCAP_EUNKNOWN      255 /* Unknown/Unexpected error */

#define oscap_seterr(family, code, desc) __oscap_seterr (__FILE__, __LINE__, __PRETTY_FUNCTION__, family, code, desc)

void  __oscap_seterr (const char *file, uint32_t line, const char *func,
                      oscap_errfamily_t family, oscap_errcode_t code, const char *desc);

void oscap_clearerr(void);
bool oscap_err(void);

oscap_errfamily_t oscap_err_family (void);
oscap_errcode_t   oscap_err_code (void);
const char       *oscap_err_desc (void);

#endif /* OSCAP_ERROR_H */
