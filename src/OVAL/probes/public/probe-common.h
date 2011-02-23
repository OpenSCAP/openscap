#ifndef PROBE_COMMON_H
#define PROBE_COMMON_H

/**
 * SEAP command codes used by probes/library
 */
#define PROBE_CMD_STE_FETCH 1 /**< State fetch command code */
#define PROBE_CMD_OBJ_EVAL  2 /**< Object eval command code */
#define PROBE_CMD_RESET     3 /**< Reset command code */

/**
 * probe option IDs
 */
#define PROBE_VARREF_HANDLING 0
#define PROBE_RESULT_CACHING  1

/**
 * probe errors (send by probes in SEAP error packets)
 */
#define PROBE_ESUCCESS       0
#define PROBE_EINVAL         1	/**< Invalid type/value/format */
#define PROBE_ENOELM         2	/**< Missing element OBSOLETE: use ENOENT */
#define PROBE_ENOVAL         3	/**< Missing value */
#define PROBE_ENOATTR        4	/**< Missing attribute */
#define PROBE_EINIT          5	/**< Initialization failed */
#define PROBE_ENOMEM         6	/**< No memory */
#define PROBE_EOPNOTSUPP     7	/**< Not supported */
#define PROBE_ERANGE         8	/**< Out of range */
#define PROBE_EDOM           9	/**< Out of domain */
#define PROBE_EFAULT        10	/**< Memory fault/NULL value */
#define PROBE_EACCESS       11	/**< Operation not permitted */
#define PROBE_ESETEVAL      12	/**< Set evaluation failed */
#define PROBE_ENOENT        13  /**< Missing entity */
#define PROBE_ENOOBJ        14  /**< Missing object */
#define PROBE_ECONNABORTED  15  /**< Evaluation aborted */
#define PROBE_EFATAL       254	/**< Unrecoverable error */
#define PROBE_EUNKNOWN     255	/**< Unknown/Unexpected error */

#endif /* PROBE_COMMON_H */
