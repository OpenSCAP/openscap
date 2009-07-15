#pragma once
#ifndef STRTO_H
#define STRTO_H

#include <stddef.h>
#include <stdint.h>

int8_t strto_int8 (const char *str, size_t len, char **endptr, int base);
int8_t strto_int8_bin (const char *str, size_t len, char **endptr);
int8_t strto_int8_oct (const char *str, size_t len, char **endptr);
int8_t strto_int8_dec (const char *str, size_t len, char **endptr);
int8_t strto_int8_hex (const char *str, size_t len, char **endptr);

uint8_t strto_uint8 (const char *str, size_t len, char **endptr, int base);
uint8_t strto_uint8_bin (const char *str, size_t len, char **endptr);
uint8_t strto_uint8_oct (const char *str, size_t len, char **endptr);
uint8_t strto_uint8_dec (const char *str, size_t len, char **endptr);
uint8_t strto_uint8_hex (const char *str, size_t len, char **endptr);

int16_t strto_int16 (const char *str, size_t len, char **endptr, int base);
int16_t strto_int16_bin (const char *str, size_t len, char **endptr);
int16_t strto_int16_oct (const char *str, size_t len, char **endptr);
int16_t strto_int16_dec (const char *str, size_t len, char **endptr);
int16_t strto_int16_hex (const char *str, size_t len, char **endptr);

uint16_t strto_uint16 (const char *str, size_t len, char **endptr, int base);
uint16_t strto_uint16_bin (const char *str, size_t len, char **endptr);
uint16_t strto_uint16_oct (const char *str, size_t len, char **endptr);
uint16_t strto_uint16_dec (const char *str, size_t len, char **endptr);
uint16_t strto_uint16_hex (const char *str, size_t len, char **endptr);

int32_t strto_int32 (const char *str, size_t len, char **endptr, int base);
int32_t strto_int32_bin (const char *str, size_t len, char **endptr);
int32_t strto_int32_oct (const char *str, size_t len, char **endptr);
int32_t strto_int32_dec (const char *str, size_t len, char **endptr);
int32_t strto_int32_hex (const char *str, size_t len, char **endptr);

uint32_t strto_uint32 (const char *str, size_t len, char **endptr, int base);
uint32_t strto_uint32_bin (const char *str, size_t len, char **endptr);
uint32_t strto_uint32_oct (const char *str, size_t len, char **endptr);
uint32_t strto_uint32_dec (const char *str, size_t len, char **endptr);
uint32_t strto_uint32_hex (const char *str, size_t len, char **endptr);

int64_t strto_int64 (const char *str, size_t len, char **endptr, int base);
int64_t strto_int64_bin (const char *str, size_t len, char **endptr);
int64_t strto_int64_oct (const char *str, size_t len, char **endptr);
int64_t strto_int64_dec (const char *str, size_t len, char **endptr);
int64_t strto_int64_hex (const char *str, size_t len, char **endptr);

uint64_t strto_uint64 (const char *str, size_t len, char **endptr, int base);
uint64_t strto_uint64_bin (const char *str, size_t len, char **endptr);
uint64_t strto_uint64_oct (const char *str, size_t len, char **endptr);
uint64_t strto_uint64_dec (const char *str, size_t len, char **endptr);
uint64_t strto_uint64_hex (const char *str, size_t len, char **endptr);

intmax_t strto_intmax (const char *str, size_t len, char **endptr, int base);
intmax_t strto_intmax_bin (const char *str, size_t len, char **endptr);
intmax_t strto_intmax_oct (const char *str, size_t len, char **endptr);
intmax_t strto_intmax_dec (const char *str, size_t len, char **endptr);
intmax_t strto_intmax_hex (const char *str, size_t len, char **endptr);

double strto_double (const char *str, size_t len, char **endptr);

#endif /* STRTO_H */
