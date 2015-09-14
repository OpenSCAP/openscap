/*
 * Copyright 2015 Red Hat Inc., Durham, North Carolina.
 * All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:
 *      "Jan Černý" <jcerny@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include "../common/util.h"
#include "oval_system_characteristics.h"
#include "results/oval_item_statuses.h"
#include "common/_error.h"

void oval_item_statuses_clear(struct oval_item_statuses *is)
{
	memset(is, 0, sizeof(*is));
}

void oval_item_statuses_add_status(struct oval_item_statuses *is, oval_syschar_status_t status)
{
	switch(status) {
	case SYSCHAR_STATUS_ERROR:
		is->error_cnt++;
		break;
	case SYSCHAR_STATUS_EXISTS:
		is->exists_cnt++;
		break;
	case SYSCHAR_STATUS_DOES_NOT_EXIST:
		is->does_not_exist_cnt++;
		break;
	case SYSCHAR_STATUS_NOT_COLLECTED:
		is->not_collected_cnt++;
		break;
	default:
		oscap_seterr(OSCAP_EFAMILY_OVAL, "Invalid oval status type: %s.", oval_syschar_status_get_text(status));
		break;
	}
}

oval_result_t oval_item_statuses_get_result(struct oval_item_statuses *is, oval_existence_t check_existence)
{
	oval_result_t result;
	switch (check_existence) {
	case OVAL_ALL_EXIST:
		if (is->exists_cnt >= 1 && is->does_not_exist_cnt == 0
			&& is->error_cnt == 0 && is->not_collected_cnt == 0) {
			result = OVAL_RESULT_TRUE;
		} else if (is->exists_cnt == 0 && is->does_not_exist_cnt == 0
			&& is->error_cnt == 0 && is->not_collected_cnt == 0) {
			result = OVAL_RESULT_FALSE;
		} else if (is->exists_cnt >= 0 && is->does_not_exist_cnt >= 1
			&& is->error_cnt >= 0 && is->not_collected_cnt >= 0) {
			result = OVAL_RESULT_FALSE;
		} else if (is->exists_cnt >= 0 && is->does_not_exist_cnt == 0
			&& is->error_cnt >= 1 && is->not_collected_cnt >= 0) {
			result = OVAL_RESULT_ERROR;
		} else if (is->exists_cnt >= 0 && is->does_not_exist_cnt == 0
			&& is->error_cnt == 0 && is->not_collected_cnt >= 1) {
			result = OVAL_RESULT_UNKNOWN;
		}
		break;
	case OVAL_ANY_EXIST:
		if (is->exists_cnt >= 0 && is->does_not_exist_cnt >= 0
			&& is->error_cnt == 0 && is->not_collected_cnt >= 0) {
			result = OVAL_RESULT_TRUE;
		} else if (is->exists_cnt >= 1 && is->does_not_exist_cnt >= 0
			&& is->error_cnt >=1 && is->not_collected_cnt >= 0) {
			result = OVAL_RESULT_TRUE;
		} else if (is->exists_cnt == 0 && is->does_not_exist_cnt >= 0
			&& is->error_cnt >=1 && is->not_collected_cnt >= 0) {
			result = OVAL_RESULT_ERROR;
		}
		break;
	case OVAL_AT_LEAST_ONE_EXISTS:
		if (is->exists_cnt >= 1 && is->does_not_exist_cnt >= 0
			&& is->error_cnt >= 0 && is->not_collected_cnt >= 0) {
			result = OVAL_RESULT_TRUE;
		} else if (is->exists_cnt >= 1 && is->does_not_exist_cnt == 0
			&& is->error_cnt == 0 && is->not_collected_cnt == 0) {
			result = OVAL_RESULT_FALSE;
		} else if (is->exists_cnt == 0 && is->does_not_exist_cnt >= 0
			&& is->error_cnt >= 1 && is->not_collected_cnt >= 0) {
			result = OVAL_RESULT_ERROR;
		} else if (is->exists_cnt == 0 && is->does_not_exist_cnt >= 0
			&& is->error_cnt == 0 && is->not_collected_cnt >= 1) {
			result = OVAL_RESULT_UNKNOWN;
		}
		break;
	case OVAL_NONE_EXIST:
		if (is->exists_cnt == 0 && is->does_not_exist_cnt >= 0
			&& is->error_cnt == 0 && is->not_collected_cnt == 0) {
			result = OVAL_RESULT_TRUE;
		} else if (is->exists_cnt >= 1 && is->does_not_exist_cnt >= 0
			&& is->error_cnt >= 0 && is->not_collected_cnt >= 0) {
			result = OVAL_RESULT_FALSE;
		} else if (is->exists_cnt == 0 && is->does_not_exist_cnt >= 0
			&& is->error_cnt >= 1 && is->not_collected_cnt >= 0) {
			result = OVAL_RESULT_ERROR;
		} else if (is->exists_cnt == 0 && is->does_not_exist_cnt >= 0
			&& is->error_cnt == 0 && is->not_collected_cnt >= 1) {
			result = OVAL_RESULT_UNKNOWN;
		}
		break;
	case OVAL_ONLY_ONE_EXISTS:
		if (is->exists_cnt == 1 && is->does_not_exist_cnt >= 0
			&& is->error_cnt == 0 && is->not_collected_cnt == 0) {
			result = OVAL_RESULT_TRUE;
		} else if (is->exists_cnt >= 2 && is->does_not_exist_cnt >= 0
			&& is->error_cnt >= 0 && is->not_collected_cnt >= 0) {
			result = OVAL_RESULT_FALSE;
		} else if (is->exists_cnt == 0 && is->does_not_exist_cnt >= 0
			&& is->error_cnt == 0 && is->not_collected_cnt == 1) {
			result = OVAL_RESULT_FALSE;
		} else if ((is->exists_cnt == 0 || is->exists_cnt == 1)
			&& is->does_not_exist_cnt >= 0 && is->error_cnt >= 1
			&& is->not_collected_cnt >= 0) {
			result = OVAL_RESULT_ERROR;
		} else if ((is->exists_cnt == 0 || is->exists_cnt == 1)
			&& is->does_not_exist_cnt >= 0 && is->error_cnt == 0
			&& is->not_collected_cnt >= 1) {
			result = OVAL_RESULT_ERROR;
		}
		break;
	default:
		oscap_seterr(OSCAP_EFAMILY_OVAL, "Invalid check_existence value: %s.",
				oval_existence_get_text(check_existence));
		result = OVAL_RESULT_ERROR;
		break;
	}
	return result;
}
