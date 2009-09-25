#ifndef PROBE_ENTCMP_H
#define PROBE_ENTCMP_H

#include <seap.h>
#include <stdarg.h>
#include "api/oval_definitions.h"
#include "api/oval_results.h"

oval_result_t SEXP_OVALent_result_bychk(SEXP_t *res_lst, oval_check_t check);
oval_result_t SEXP_OVALent_result_byopr(SEXP_t *res_lst, oval_operator_t operator);

oval_result_t SEXP_OVALentobj_cmp(SEXP_t *ent_obj, SEXP_t *val);
oval_result_t SEXP_OVALentste_cmp(SEXP_t *ent_ste, SEXP_t *ent_itm);
oval_result_t SEXP_OVALent_cmp_binary(SEXP_t *val1, SEXP_t *val2, oval_operation_t op);
oval_result_t SEXP_OVALent_cmp_binary(SEXP_t *val1, SEXP_t *val2, oval_operation_t op);
oval_result_t SEXP_OVALent_cmp_bool(SEXP_t *val1, SEXP_t *val2, oval_operation_t op);
oval_result_t SEXP_OVALent_cmp_evr(SEXP_t *val1, SEXP_t *val2, oval_operation_t op);
oval_result_t SEXP_OVALent_cmp_filesetrev(SEXP_t *val1, SEXP_t *val2, oval_operation_t op);
oval_result_t SEXP_OVALent_cmp_float(SEXP_t *val1, SEXP_t *val2, oval_operation_t op);
oval_result_t SEXP_OVALent_cmp_int(SEXP_t *val1, SEXP_t *val2, oval_operation_t op);
oval_result_t SEXP_OVALent_cmp_ios(SEXP_t *val1, SEXP_t *val2, oval_operation_t op);
oval_result_t SEXP_OVALent_cmp_version(SEXP_t *val1, SEXP_t *val2, oval_operation_t op);
oval_result_t SEXP_OVALent_cmp_string(SEXP_t *val1, SEXP_t *val2, oval_operation_t op);

#endif
