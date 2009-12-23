/*
 * oval_errno.h
 *
 *  Created on: Sep 8, 2009
 *      Author: tom.greaves
 */

#ifndef OVAL_ERRNO_H_
#define OVAL_ERRNO_H_

#include "../common/util.h"

OSCAP_HIDDEN_START;

extern int oval_errno;
#define OVAL_INVALID_ARGUMENT     1
#define OVAL_INTERNAL_ERROR       2
#define OVAL_UNSUPPORTED_DATATYPE 3
#define OVAL_INVALID_COMPARISON   4
#define OVAL_NO_RESULT_CALCULATED 5

OSCAP_HIDDEN_END;

#endif				/* OVAL_ERRNO_H_ */
