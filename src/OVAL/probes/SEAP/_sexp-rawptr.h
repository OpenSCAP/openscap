#ifndef SEXP_RAWPTR
#define SEXP_RAWPTR

#include <stdint.h>

static inline void *SEXP_rawptr_mask(void *ptr, uintptr_t mask)
{
	return (void *)((uintptr_t)(ptr) & mask);
}

#define SEXP_rawptr_maskT(T, ptr, mask) ((T *)SEXP_rawptr_mask((void *)(ptr), mask))

static inline void *SEXP_rawptr_merge(void *ptr, uintptr_t bits)
{
	return (void *)(((uintptr_t)(ptr) | bits));
}

#define SEXP_rawptr_mergeT(T, ptr, bits) ((T *)SEXP_rawptr_merge((void *)(ptr), bits))

static inline void *SEXP_rawptr_safemerge(void *ptr, uintptr_t bits, uintptr_t mask)
{
	return (void *)(((uintptr_t)SEXP_rawptr_mask(ptr, mask))|(~mask & bits));
}

#define SEXP_rawptr_safemergeT(T, ptr, bits, mask) ((T *)SEXP_rawptr_safemerge((void *)(ptr), bits, mask))

static inline bool SEXP_rawptr_bit(void *ptr, int n)
{
	return (bool)(((uintptr_t)(ptr)) & (1 << n));
}

static inline uintptr_t SEXP_rawptr_bitmask(void *ptr, uintptr_t mask)
{
	return ((uintptr_t)(ptr)) & mask;
}

#endif /* SEXP_RAWPTR */
