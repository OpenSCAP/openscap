// TODO License SOPRA STERIA

#ifndef OPENSCAP_AUDITDLINE_PROBE_H
#define OPENSCAP_AUDITDLINE_PROBE_H

#include "probe-api.h"

int auditdline_probe_offline_mode_supported(void);
int auditdline_probe_main(probe_ctx *ctx, void *arg);

 #endif /* OPENSCAP_AUDITDLINE_PROBE_H */