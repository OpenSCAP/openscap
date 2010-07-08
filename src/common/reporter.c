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
#include "public/error.h"
#include <errno.h>
#include <limits.h>

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

const oscap_reporter_code_t OSCAP_REPORTER_CODE_MAX = UINT_MAX;

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

struct oscap_reporter_message *oscap_reporter_message_new_arg(oscap_reporter_family_t family, oscap_reporter_code_t code, const char *fmt, va_list ap)
{
    char *string = oscap_vsprintf(fmt, ap);
    if (string == NULL) return NULL;
    struct oscap_reporter_message *msg = oscap_reporter_message_new_fill(family, code, string);
    oscap_free(string);
    return msg;
}

struct oscap_reporter_message *oscap_reporter_message_new_fmt(oscap_reporter_family_t family, oscap_reporter_code_t code, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    struct oscap_reporter_message *msg = oscap_reporter_message_new_arg(family, code, fmt, ap);
    va_end(ap);
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

#define USERRELEASE(N) do { oscap_userdata_release(&msg->user##N, msg->flags.u##N##t); } while(0)
void oscap_reporter_message_free(struct oscap_reporter_message *msg)
{
	if (msg) {
		oscap_free(msg->string);
		USERRELEASE(1); USERRELEASE(2); USERRELEASE(3);
		oscap_free(msg);
	}
}

void oscap_reporter_dispatch(oscap_reporter reporter, const struct oscap_reporter_message *msg, void *arg)
{
	assert(reporter != NULL);
	reporter(msg, arg);
}

void oscap_reporter_report(oscap_reporter reporter, struct oscap_reporter_message *msg, void *arg)
{
	if (reporter != NULL && msg != NULL)
		oscap_reporter_dispatch(reporter, msg, arg);
	oscap_reporter_message_free(msg);
}

void oscap_reporter_report_fmt(oscap_reporter reporter, void *arg, oscap_reporter_family_t family, oscap_reporter_code_t code, const char *fmt, ...)
{
	if (reporter == NULL) return;
    va_list ap;
    va_start(ap, fmt);
	oscap_reporter_report(reporter, oscap_reporter_message_new_arg(family, code, fmt, ap), arg);
    va_end(ap);
}

// ================= reporting helpers ===================

void oscap_reporter_report_xml(struct oscap_reporter_context *rctxt, xmlErrorPtr error)
{
    if (rctxt == NULL || rctxt->reporter == NULL) return;
    if (error == NULL) error = xmlGetLastError();
    if (error == NULL) return;
    struct oscap_reporter_message *msg = oscap_reporter_message_new_fill(OSCAP_REPORTER_FAMILY_XML, error->code, error->message);
    oscap_reporter_message_set_user1str(msg, error->file);
    oscap_reporter_message_set_user2num(msg, error->line);
    oscap_reporter_report(rctxt->reporter, msg, rctxt->arg);
}

void oscap_reporter_report_libc(oscap_reporter reporter, void *arg)
{
    if (reporter == NULL) return;
    oscap_reporter_report(reporter, oscap_reporter_message_new_fill(OSCAP_EFAMILY_GLIBC, errno, strerror(errno)), arg);
}

// ================== standard reporters =================

void oscap_reporter_fd(const struct oscap_reporter_message *msg, void *arg)
{
	if (arg == NULL) return;
	fprintf(arg, "%d.%d: %s\n", msg->family, msg->code, msg->string);
}

