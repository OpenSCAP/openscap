#pragma once
#ifndef OSCAP_ERROR_H
#define OSCAP_ERROR_H

#include <stdint.h>
#include <stdbool.h>

typedef uint16_t oscap_errfamily_t;
typedef uint16_t oscap_errcode_t;

#define oscap_seterr(family, code, desc) __oscap_seterr (__FILE__, __LINE__, __PRETTY_FUNCTION__, family, code, desc)

void  __oscap_seterr (const char *file, uint32_t line, const char *func,
                      oscap_errfamily_t family, oscap_errcode_t code, const char *desc);

void oscap_clearerr(void);
bool oscap_err(void);

oscap_errfamily_t oscap_err_family (void);
oscap_errcode_t   oscap_err_code (void);
const char       *oscap_err_desc (void);

#endif /* OSCAP_ERROR_H */
