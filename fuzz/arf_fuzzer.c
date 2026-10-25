/*
 * libFuzzer harness for ARF / Result Data Stream (RDS) parsing
 * (src/DS/rds.c, src/DS/rds_index.c, src/DS/ds_rds_session.c).
 *
 * ARF (Asset Reporting Format) result files are the output side of a scan and
 * are routinely passed around and re-ingested, so their parser is a real attack
 * surface. The base scap_parse_fuzzer only builds the RDS session; this harness
 * goes further and walks the result-data-stream index (reports, assets,
 * report-requests) and extracts the embedded reports, which is what drives the
 * bulk of the RDS parsing code.
 *
 * Pipeline:
 *   ds_rds_session_new_from_source()      open the ARF
 *   ds_rds_session_get_rds_idx()          build & return the RDS index
 *   walk reports / assets / report-requests via the index iterators
 *   ds_rds_session_select_report(NULL)    extract+parse the first report
 *   ds_rds_session_select_report_request(NULL)
 */

#include <stddef.h>
#include <stdint.h>

#include "fuzz_common.h"
#include "oscap_source.h"
#include "scap_ds.h"
#include "ds_rds_session.h"

static void walk_index(struct rds_index *idx)
{
	if (idx == NULL) {
		return;
	}

	struct rds_report_index_iterator *rit = rds_index_get_reports(idx);
	while (rds_report_index_iterator_has_more(rit)) {
		struct rds_report_index *r = rds_report_index_iterator_next(rit);
		rds_report_index_get_id(r);
	}
	rds_report_index_iterator_free(rit);

	struct rds_report_request_index_iterator *qit = rds_index_get_report_requests(idx);
	while (rds_report_request_index_iterator_has_more(qit)) {
		struct rds_report_request_index *q = rds_report_request_index_iterator_next(qit);
		rds_report_request_index_get_id(q);
	}
	rds_report_request_index_iterator_free(qit);

	struct rds_asset_index_iterator *ait = rds_index_get_assets(idx);
	while (rds_asset_index_iterator_has_more(ait)) {
		struct rds_asset_index *a = rds_asset_index_iterator_next(ait);
		struct rds_report_index_iterator *arit = rds_asset_index_get_reports(a);
		while (rds_report_index_iterator_has_more(arit)) {
			rds_report_index_iterator_next(arit);
		}
		rds_report_index_iterator_free(arit);
	}
	rds_asset_index_iterator_free(ait);
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
	FUZZ_INIT();

	struct oscap_source *source =
		oscap_source_new_from_memory((const char *)data, size, "fuzz-arf.xml");
	if (source == NULL) {
		return 0;
	}

	struct ds_rds_session *session = ds_rds_session_new_from_source(source);
	if (session != NULL) {
		walk_index(ds_rds_session_get_rds_idx(session));
		// Returned sources are owned by the session; do not free them.
		ds_rds_session_select_report(session, NULL);
		ds_rds_session_select_report_request(session, NULL);
		ds_rds_session_free(session);
	}

	oscap_source_free(source);
	return 0;
}
