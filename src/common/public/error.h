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
#define OSCAP_ELEVEL_NONE      0   /* None */
#define OSCAP_ELEVEL_WARNING   1   /* A simple warning */
#define OSCAP_ELEVEL_ERROR     2   /* A recoverable error */
#define OSCAP_ELEVEL_FATAL     3   /* A fatal error */

/* For oscap error family there are : */
/**************************************/
#define OSCAP_EFAMILY_NONE     0   /* None */
#define OSCAP_EFAMILY_GLIBC    1   /* Errno errors */
#define OSCAP_EFAMILY_XML      2   /* Libxml - xmlerror errors */
#define OSCAP_EFAMILY_OSCAP    3   /* OSCAP errors */

/* For oscap error of family */
/*****************************/

#define OSCAP_ENONE         0   /* None */
#define OSCAP_EALLOC        1   /* OSCAP allocation error*/
#define OSCAP_EXMLELEM      2   /* Unknown element in XML */
#define OSCAP_EXMLNODETYPE  3   /* Bad node type in XML */
#define OSCAP_EXMLNOELEMENT 4   /* No expected element */
#define OSCAP_ECPEINVOP     5   /* Invalid CPE Language operation */
#define OSCAP_EOVALINT      6   /* OVAL internal error */
#define OSCAP_EREGEXP       7   /* Error in regexp compilation */
#define OSCAP_EREGEXPCOMP   8   /* Invalid string comparison in regexps */
#define OSCAP_EUNDATATYPE   9   /* Unsupported data type */  
#define OSCAP_EINVARG       10  /* Invalid argument */
#define OSCAP_EVARTYPE      11  /* Variable type is not valid */
#define OSCAP_ESEAPINIT     12  /* Failed seap initialization */
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
