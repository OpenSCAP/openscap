#pragma once
#ifndef OVAL_PROBE_H
#define OVAL_PROBE_H

#include <stdarg.h>
#include <stdint.h>
#include "oval_definitions.h"
#include "oval_system_characteristics.h"

typedef struct oval_pctx oval_pctx_t;

/**
 * Creaste new probe context.
 * @param model system characteristics model
 */
oval_pctx_t *oval_pctx_new(struct oval_syschar_model *model);

/**
 * Free probe context
 * @param pctx probe context
 */
void oval_pctx_free(oval_pctx_t *pctx) __attribute__ ((nonnull(1)));

/*
 * probe context flags
 */
#define OVAL_PCTX_FLAG_NOREPLY  0x0001	/* don't send probe result to library - just an ack */
#define OVAL_PCTX_FLAG_NORECONN 0x0002	/* don't try to reconnect on fatal errors */
#define OVAL_PCTX_FLAG_RUNALL   0x0004	/* execute all probes when executing the first */
#define OVAL_PCTX_FLAG_RUNNOW   0x0008	/* execute all probes immediately */

#define OVAL_PCTX_FLAG_MASK (0x0001|0x0002|0x0004|0x0008)

/**
 * Set probe context flag
 * @param pctx probe context
 * @param flags set of flags to set (combined with the bitwise or operator)
 */
int oval_pctx_setflag(oval_pctx_t *pctx, uint32_t flags) __attribute__ ((nonnull(1)));

/**
 * Unset probe context flag
 * @param pctx probe context
 * @param flags set of flags to unset (combined with the bitwise or operator)
 */
int oval_pctx_unsetflag(oval_pctx_t *pctx, uint32_t flags) __attribute__ ((nonnull(1)));

/*
 * probe context attributes
 */
#define OVAL_PCTX_ATTR_RETRY       0x0001	/* set maximum retry count (send, connect) */
#define OVAL_PCTX_ATTR_RECVTIMEOUT 0x0002	/* set receive timeout - in miliseconds */
#define OVAL_PCTX_ATTR_SENDTIMEOUT 0x0003	/* set send timeout - in miliseconds */
#define OVAL_PCTX_ATTR_SCHEME      0x0004	/* set communication scheme */
#define OVAL_PCTX_ATTR_DIR         0x0005	/* set directory where the probes are located */
#define OVAL_PCTX_ATTR_MODEL       0x0006	/* set definition model - update registered commands is necessary */

/**
 * Set probe context attribute
 * @param pctx probe context
 * @param attr the attribute to set
 * @param ... attribute dependend arguments
 */
int oval_pctx_setattr(oval_pctx_t *pctx, uint32_t attr, ...) __attribute__ ((nonnull(1)));

/**
 * Reset probe state
 * @param pctx probe context
 * @param subtype subtype of the probe which will be reset
 */
int oval_probe_reset(oval_pctx_t *pctx, oval_subtype_t subtype) __attribute__ ((nonnull(1)));

/**
 * Close connection and shutdown a probe
 * @param pctx probe context
 * @param subtype subtype of the probe which will be closed
 */
int oval_probe_close(oval_pctx_t *pctx, oval_subtype_t subtype) __attribute__ ((nonnull(1)));

/**
 * Evaluate system info probe
 * @param pctx probe context
 */
struct oval_sysinfo *oval_probe_sysinf_eval(oval_pctx_t *pctx) __attribute__ ((nonnull(1)));

/**
 * Evaluate an object
 * @param pctx probe context
 * @param object the object to evaluate
 */
struct oval_syschar *oval_probe_object_eval(oval_pctx_t *pctx, struct oval_object *object) __attribute__ ((nonnull(1, 2)));

#endif				/* OVAL_PROBE_H */
