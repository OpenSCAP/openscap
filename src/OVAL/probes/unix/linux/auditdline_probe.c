// TODO License SOPRA STERIA

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// System libraries
#include <libaudit.h>

// OpenSCAP libraries
#include <probe/probe.h>
#include <debug_priv.h>
#include <sexp-manip.h>
#include <list.h>

// probe include
#include "auditdline_probe.h"

static struct oscap_list* get_all_audit_rules(probe_ctx *ctx) {

	struct oscap_list *audit_list = oscap_list_new();

	int audit_fd = audit_open();

	if (audit_fd > -1) {

		// Request the kernel to provide the audit rules list
		if (audit_request_rules_list_data(audit_fd) > 0) {

			dD("Successfull request to get the kernel audit rules.");

		} else {
			int errnum = errno;
			dE("Failed to request the kernel for audit rules : %s", strerror(errnum));
			SEXP_t *item = probe_item_create(
					OVAL_DGAMI_AUDITDLINE, NULL,
					"filter_key", OVAL_DATATYPE_STRING, "",
					NULL
			);

			probe_item_setstatus(item, SYSCHAR_STATUS_ERROR);
			probe_item_add_msg(item, OVAL_MESSAGE_LEVEL_ERROR,
					   "Failed to request the kernel for audit rules : %s.", strerror (errnum));
			probe_item_collect(ctx, item);
		 }


	} else {
		//TODO gestion d'erreur
	}

	audit_close(audit_fd);

	return audit_list;
}


int auditdline_probe_offline_mode_supported()
{
	return PROBE_OFFLINE_CHROOT;
}

int auditdline_probe_main(probe_ctx *ctx, void *arg) {

	SEXP_t *probe_in = probe_ctx_getobject(ctx);
	SEXP_t *key_filter = probe_obj_getent(probe_in, "filter_key", 1);
	SEXP_t *key_filter_value = probe_ent_getval(key_filter);
	char *key_filter_str = SEXP_string_cstr(key_filter_value);

	get_all_audit_rules(ctx);

	// Cleanup of the ressources
	free(key_filter_str);
	SEXP_free(key_filter_value);
	SEXP_free(key_filter);

	return 0;
}