// TODO License SOPRA STERIA

#ifndef OPENSCAP_AUDITDLINE_PROBE_H
#define OPENSCAP_AUDITDLINE_PROBE_H

#include "probe-api.h"

struct audit_line {
	struct oscap_string* audit_line;
	struct oscap_stringlist* filter_key;
	int line_number;
};

struct audit_line *audit_line_new();

void auditd_line_free(struct audit_line* audit_line);

void audit_line_push(struct audit_line * audit_line, const char * audit_line_part);

bool audit_line_add_filter_key(struct audit_line * audit_line, const char * filter_key);

int auditdline_probe_offline_mode_supported(void);
int auditdline_probe_main(probe_ctx *ctx, void *arg);

 #endif /* OPENSCAP_AUDITDLINE_PROBE_H */