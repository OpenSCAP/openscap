/*
 * Copyright 2010 Red Hat Inc., Durham, North Carolina.
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
 *       Lukas Kuklinek <lkuklinek@redhat.com>
 */

#include "reporter_priv.h"
#include "list.h"
#include <stdio.h>
#include <string.h>


struct oscap_reporter_type {
	oscap_reporter_init_func    init;
	oscap_reporter_report_func  report;
	oscap_reporter_destroy_func destroy;
};

struct oscap_reporter {
	const struct oscap_reporter_type *type;
	void *userdata;
};

union oscap_reporter_userdata {
	int   num;
	char *str;
	void *ptr;
};

typedef enum oscap_reporeter_userdata {
	OSCAP_REPORTER_USERDATA_NONE,
	OSCAP_REPORTER_USERDATA_NUM,
	OSCAP_REPORTER_USERDATA_STR,
	OSCAP_REPORTER_USERDATA_PTR
} oscap_reporeter_userdata_t;

struct oscap_reporter_message {
	oscap_reporter_family_t family;
	oscap_reporter_code_t   code;

	struct {
		oscap_reporeter_userdata_t u1t : 2;
		oscap_reporeter_userdata_t u2t : 2;
		oscap_reporeter_userdata_t u3t : 2;
	} flags;

	char *string;

	union oscap_reporter_userdata user1;
	union oscap_reporter_userdata user2;
	union oscap_reporter_userdata user3;
};

struct oscap_reporter_message *oscap_reporter_message_new(void)
{
	return oscap_calloc(1, sizeof(struct oscap_reporter_message));
}

struct oscap_reporter_message *oscap_reporter_message_new_fill(oscap_reporter_family_t family, oscap_reporter_code_t code, const char *string)
{
	struct oscap_reporter_message *msg = oscap_reporter_message_new();
	msg->family = family;
	msg->code   = code;
	msg->string = oscap_trim(oscap_strdup(string));
	return msg;
}

#define USERCOPY(N) do { \
	if (msg->flags.u##N##t == OSCAP_REPORTER_USERDATA_STR) { \
		oscap_free(newmsg->user##N.str); \
		newmsg->user##N.str = oscap_strdup(msg->user##N.str); \
	} } while(0)
struct oscap_reporter_message *oscap_reporter_message_clone(const struct oscap_reporter_message *msg)
{
	struct oscap_reporter_message *newmsg = oscap_calloc(1, sizeof(struct oscap_reporter_message));
	memcpy(newmsg, msg, sizeof(*newmsg));
	newmsg->string = oscap_strdup(msg->string);
	USERCOPY(1); USERCOPY(2); USERCOPY(3);
	return newmsg;
}
#undef USERCOPY

static inline void oscap_userdata_release(union oscap_reporter_userdata *data, oscap_reporeter_userdata_t type)
{
	assert(data);
	switch (type) {
		case OSCAP_REPORTER_USERDATA_STR: oscap_free(data->str);
		default: break;
	}
}

static inline void oscap_userdata_set_str(union oscap_reporter_userdata *data, const char *newstr)
{
	assert(data);
	data->str = oscap_strdup(newstr);
}
static inline void oscap_userdata_set_num(union oscap_reporter_userdata *data, int newnum)
{
	assert(data);
	data->num = newnum;
}
static inline void oscap_userdata_set_ptr(union oscap_reporter_userdata *data, void *newptr)
{
	assert(data);
	data->ptr = newptr;
}

#define OSCAP_USERDATA_CHECK(RTYPE, CNAME, N) \
		do { assert(msg != NULL); if (msg->flags.u##N##t != OSCAP_REPORTER_USERDATA_##CNAME) return ((RTYPE)0); } while(0)
#define OSCAP_USERDATA_GETTER(RTYPE, RTNAME, CNAME, N) \
		RTYPE oscap_reporter_message_get_user##N##RTNAME(const struct oscap_reporter_message* msg) \
		{ OSCAP_USERDATA_CHECK(RTYPE, CNAME, N); return msg->user##N.RTNAME; }
#define OSCAP_USERDATA_SETTER(RTYPE, RTNAME, CNAME, N) \
		bool oscap_reporter_message_set_user##N##RTNAME(struct oscap_reporter_message* msg, RTYPE newval) \
		{ assert(msg != NULL); oscap_userdata_release(&msg->user##N, msg->flags.u##N##t); \
		oscap_userdata_set_##RTNAME(&msg->user##N, newval); msg->flags.u##N##t = OSCAP_REPORTER_USERDATA_##CNAME; return true; }
#define OSCAP_USERDATA_ACCESSOR_ONE(RTYPE, RTNAME, CNAME, N) \
		OSCAP_USERDATA_GETTER(RTYPE, RTNAME, CNAME, N) OSCAP_USERDATA_SETTER(RTYPE, RTNAME, CNAME, N)
#define OSCAP_USERDATA_ACCESSOR(N) OSCAP_USERDATA_ACCESSOR_ONE(const char*, str, STR, N) \
		OSCAP_USERDATA_ACCESSOR_ONE(int, num, NUM, N) OSCAP_USERDATA_ACCESSOR_ONE(void*, ptr, PTR, N)

OSCAP_USERDATA_ACCESSOR(1)
OSCAP_USERDATA_ACCESSOR(2)
OSCAP_USERDATA_ACCESSOR(3)
OSCAP_ACCESSOR_SIMPLE(oscap_reporter_family_t, oscap_reporter_message, family)
OSCAP_ACCESSOR_SIMPLE(oscap_reporter_code_t, oscap_reporter_message, code)
OSCAP_ACCESSOR_STRING(oscap_reporter_message, string)
OSCAP_ACCESSOR_STRING(oscap_reporter, userdata)

#define USERRELEASE(N) do { oscap_userdata_release(&msg->user##N, msg->flags.u##N##t); } while(0)
void oscap_reporter_message_free(struct oscap_reporter_message *msg)
{
	if (msg) {
		oscap_free(msg->string);
		USERRELEASE(1); USERRELEASE(2); USERRELEASE(3);
		oscap_free(msg);
	}
}
#undef USERRELEASE

static bool oscap_reporter_empty_init(void** user) { return true; }
static void oscap_reporter_empty_report(const struct oscap_reporter_message *msg, void* user) {}
static void oscap_reporter_empty_destroy(void* user) {}

struct oscap_reporter_type *oscap_reporter_type_new_full(
						oscap_reporter_init_func    init,
						oscap_reporter_report_func  report,
						oscap_reporter_destroy_func destroy)
{
	struct oscap_reporter_type *reptype = oscap_calloc(1, sizeof(struct oscap_reporter_type));
	reptype->init    = (init    ? init    : oscap_reporter_empty_init   );
	reptype->report  = (report  ? report  : oscap_reporter_empty_report );
	reptype->destroy = (destroy ? destroy : oscap_reporter_empty_destroy);
	return reptype;
}

void oscap_reporter_type_free(struct oscap_reporter_type *reptype)
{
	oscap_free(reptype);
}


struct oscap_reporter *oscap_reporter_new(const struct oscap_reporter_type *type, void *user)
{
	assert(type != NULL);

	struct oscap_reporter *reporter = oscap_calloc(1, sizeof(struct oscap_reporter));
	reporter->userdata = user;
	reporter->type = type;

	if (!reporter->type->init(&reporter->userdata)) {
		oscap_free(reporter);
		return NULL;
	}

	return reporter;
}

void oscap_reporter_free(struct oscap_reporter *reporter)
{
	if (reporter) {
		reporter->type->destroy(reporter->userdata);
		oscap_free(reporter);
	}
}

void oscap_reporter_dispatch(struct oscap_reporter *reporter, const struct oscap_reporter_message *msg)
{
	assert(reporter != NULL);
	reporter->type->report(msg, reporter->userdata);
}

void oscap_reporter_report(struct oscap_reporter *reporter, struct oscap_reporter_message *msg)
{
	if (reporter != NULL && msg != NULL)
		oscap_reporter_dispatch(reporter, msg);
	oscap_reporter_message_free(msg);
}

// =============== stdout reporter =====================

static void oscap_reporter_stdout_report(const struct oscap_reporter_message *msg, void *user)
{
	printf("%d.%d %s: %s\n", msg->family, msg->code, (user ? (const char *) user : ""), msg->string);
}

const struct oscap_reporter_type OSCAP_REPORTER_STDOUT = {
	.init    = oscap_reporter_empty_init,
	.report  = oscap_reporter_stdout_report,
	.destroy = oscap_reporter_empty_destroy
};

// ===================== multireporter ===================

static bool oscap_reporter_multi_init(void** user)
{
    if (*user != NULL) return false;
    *user = oscap_list_new();
    return true;
}

static void oscap_reporter_multi_report(const struct oscap_reporter_message *msg, void* user)
{
    assert(msg != NULL);
    assert(user != NULL);

    struct oscap_reporter *reporter;
    struct oscap_iterator *it = oscap_iterator_new(user);

    while (oscap_iterator_has_more(it)) {
        reporter = oscap_iterator_next(it);
        oscap_reporter_dispatch(reporter, msg);
    }

    oscap_iterator_free(it);
}

static void oscap_reporter_multi_destroy(void* user)
{
    assert(user != NULL);
    oscap_list_free(user, (oscap_destruct_func) oscap_reporter_free);
}

void oscap_reporter_multi_add_reporter(struct oscap_reporter *multi, struct oscap_reporter *reporter)
{
    assert(multi != NULL);
    if (reporter != NULL)
        oscap_list_add(multi->userdata, reporter);
}

const struct oscap_reporter_type OSCAP_REPORTER_MULTI = {
	.init    = oscap_reporter_multi_init,
	.report  = oscap_reporter_multi_report,
	.destroy = oscap_reporter_multi_destroy
};

