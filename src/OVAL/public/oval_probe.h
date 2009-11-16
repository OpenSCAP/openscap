#pragma once
#ifndef OVAL_PROBE_H
#define OVAL_PROBE_H

#include <stdarg.h>
#include <stdint.h>
#include "oval_definitions.h"
#include "oval_system_characteristics.h"

typedef struct oval_pctx oval_pctx_t;

#define OVAL_PCTX_ATTR_NOREPLY  0x0001 /* don't send probe result to library - just an ack */
#define OVAL_PCTX_ATTR_NORECONN 0x0002 /* don't try to reconnect on fatal errors */
#define OVAL_PCTX_ATTR_RUNALL   0x0003 /* execute all probes when executing the first */
#define OVAL_PCTX_ATTR_RUNNOW   0x0004 /* execute all probes immediately */

#define OVAL_PCTX_PARAM_RETRY       0x0001 /* set maximum retry count (send, connect) */
#define OVAL_PCTX_PARAM_RECVTIMEOUT 0x0002 /* set receive timeout - in miliseconds */
#define OVAL_PCTX_PARAM_SENDTIMEOUT 0x0003 /* set send timeout - in miliseconds */
#define OVAL_PCTX_PARAM_SCHEME      0x0004 /* set communication scheme */
#define OVAL_PCTX_PARAM_DIR         0x0005 /* set directory where the probes are located */
#define OVAL_PCTX_PARAM_MODEL       0x0006 /* set definition model - update registered commands is necessary */ 

oval_pctx_t *oval_pctx_new (struct oval_definition_model *);
void         oval_pctx_free (oval_pctx_t *) __attribute__ ((nonnull (1)));

int oval_pctx_setattr (oval_pctx_t *, uint32_t) __attribute__ ((nonnull (1)));
int oval_pctx_unsetattr (oval_pctx_t *, uint32_t) __attribute__ ((nonnull (1)));
int oval_pctx_setparam (oval_pctx_t *, uint32_t, ...) __attribute__ ((nonnull (1)));

int oval_probe_reset (oval_pctx_t *, oval_subtype_t) __attribute__ ((nonnull (1)));
int oval_probe_close (oval_pctx_t *, oval_subtype_t) __attribute__ ((nonnull (1)));

struct oval_sysinfo *oval_probe_sysinf_eval (oval_pctx_t *) __attribute__ ((nonnull (1)));
struct oval_syschar *oval_probe_object_eval (oval_pctx_t *, struct oval_object *) __attribute__ ((nonnull (1,2)));

#endif /* OVAL_PROBE_H */
