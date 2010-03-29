#pragma once
#ifndef _SEXP_ATOMIC_H
#define _SEXP_ATOMIC_H

#include <stdint.h>
#include <stdbool.h>

uint16_t SEXP_atomic_dec_u16 (volatile uint16_t *ptr);
uint16_t SEXP_atomic_inc_u16 (volatile uint16_t *ptr);
bool     SEXP_atomic_cas_u16 (volatile uint16_t *ptr, uint16_t old, uint16_t new);

uint32_t SEXP_atomic_dec_u32 (volatile uint32_t *ptr);
uint32_t SEXP_atomic_inc_u32 (volatile uint32_t *ptr);
bool     SEXP_atomic_cas_u32 (volatile uint32_t *ptr, uint32_t old, uint32_t new);

#endif /* _SEXP_ATOMIC_H */
