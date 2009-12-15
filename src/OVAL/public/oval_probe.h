#pragma once
#ifndef OVAL_PROBE_H
#define OVAL_PROBE_H

#include <stdarg.h>
#include <stdint.h>
#include "oval_definitions.h"
#include "oval_system_characteristics.h"

typedef struct oval_pctx oval_pctx_t;

oval_pctx_t *oval_pctx_new (struct oval_syschar_model *);
void         oval_pctx_free (oval_pctx_t *);

/*
 * probe context flags
 */
#define OVAL_PCTX_FLAG_NOREPLY  0x0001 /* don't send probe result to library - just an ack */
#define OVAL_PCTX_FLAG_NORECONN 0x0002 /* don't try to reconnect on fatal errors */
#define OVAL_PCTX_FLAG_RUNALL   0x0004 /* execute all probes when executing the first */
#define OVAL_PCTX_FLAG_RUNNOW   0x0008 /* execute all probes immediately */

#define OVAL_PCTX_FLAG_MASK (0x0001|0x0002|0x0004|0x0008)

int oval_pctx_setflag (oval_pctx_t *, uint32_t);
int oval_pctx_unsetflag (oval_pctx_t *, uint32_t);

/*
 * probe context attributes
 */
#define OVAL_PCTX_ATTR_RETRY       0x0001 /* set maximum retry count (send, connect) */
#define OVAL_PCTX_ATTR_RECVTIMEOUT 0x0002 /* set receive timeout - in miliseconds */
#define OVAL_PCTX_ATTR_SENDTIMEOUT 0x0003 /* set send timeout - in miliseconds */
#define OVAL_PCTX_ATTR_SCHEME      0x0004 /* set communication scheme */
#define OVAL_PCTX_ATTR_DIR         0x0005 /* set directory where the probes are located */
#define OVAL_PCTX_ATTR_MODEL       0x0006 /* set definition model - update registered commands is necessary */ 

int oval_pctx_setattr (oval_pctx_t *, uint32_t, ...);

int oval_probe_reset (oval_pctx_t *, oval_subtype_t);
int oval_probe_close (oval_pctx_t *, oval_subtype_t);

struct oval_sysinfo *oval_probe_sysinf_eval (struct oval_syschar_model *, oval_pctx_t *);
struct oval_syschar *oval_probe_object_eval (oval_pctx_t *, struct oval_object *);

#endif /* OVAL_PROBE_H */
