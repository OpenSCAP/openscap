/*
 * xccdf_extensions.c
 *
 *  Created on: Jan 7, 2010
 *      Author: david.niemoller
 */
#include "public/xccdf_extensions.h"
#include "../XCCDF/item.h"

#define XCCDF_ABSTRACT_STRING_SETTER(TNAME,MNAME,MEMBER) \
        void xccdf_##TNAME##_set_##MNAME(const struct xccdf_##TNAME* item, const char *MNAME) { XITEM(item)->MEMBER = strdup(MNAME);}

#define XCCDF_ABSTRACT_TYPE_SETTER(TNAME,TYPE,MNAME,MEMBER) \
        void xccdf_##TNAME##_set_##MNAME(const struct xccdf_##TNAME* item, TYPE MNAME) { XITEM(item)->MEMBER = MNAME;}

#define XCCDF_ABSTRACT_APPENDER(TNAME, MTYPE, MNAME, MEMBER) \
        void xccdf_##TNAME##_append_##MNAME(const struct xccdf_##TNAME* item, MTYPE MNAME) {oscap_list_add(XITEM(item)->MEMBER, MNAME);}


#define XCCDF_ITEM_STRING_SETTER(MNAME) \
		XCCDF_ABSTRACT_STRING_SETTER(item,MNAME,item.MNAME) \
		XCCDF_ABSTRACT_STRING_SETTER(benchmark, MNAME, item.MNAME) \
		XCCDF_ABSTRACT_STRING_SETTER(profile, MNAME, item.MNAME) \
		XCCDF_ABSTRACT_STRING_SETTER(rule, MNAME, item.MNAME) \
		XCCDF_ABSTRACT_STRING_SETTER(value, MNAME, item.MNAME) \
		XCCDF_ABSTRACT_STRING_SETTER(group, MNAME, item.MNAME)

#define XCCDF_STATIC_ITEM_STRING_SETTER(MNAME) \
		static XCCDF_ABSTRACT_STRING_SETTER(item,MNAME,item.MNAME)

#define XCCDF_ITEM_TYPE_SETTER(MTYPE,MNAME) \
		XCCDF_ABSTRACT_TYPE_SETTER(item,MTYPE, MNAME,item.MNAME) \
		XCCDF_ABSTRACT_TYPE_SETTER(benchmark, MTYPE, MNAME, item.MNAME) \
		XCCDF_ABSTRACT_TYPE_SETTER(profile, MTYPE, MNAME, item.MNAME) \
		XCCDF_ABSTRACT_TYPE_SETTER(rule, MTYPE, MNAME, item.MNAME) \
		XCCDF_ABSTRACT_TYPE_SETTER(value, MTYPE, MNAME, item.MNAME) \
		XCCDF_ABSTRACT_TYPE_SETTER(group, MTYPE, MNAME, item.MNAME)

#define XCCDF_STATIC_ITEM_TYPE_SETTER(MTYPE,MNAME) \
		static XCCDF_ABSTRACT_TYPE_SETTER(item,MTYPE, MNAME,item.MNAME)


#define XCCDF_ITEM_APPENDER(MTYPE, MNAME, MEMBER) \
		XCCDF_ABSTRACT_APPENDER(item, MTYPE, MNAME, item.MEMBER) \
		XCCDF_ABSTRACT_APPENDER(benchmark, MTYPE, MNAME, item.MEMBER) \
		XCCDF_ABSTRACT_APPENDER(profile, MTYPE, MNAME, item.MEMBER) \
		XCCDF_ABSTRACT_APPENDER(rule, MTYPE, MNAME, item.MEMBER) \
		XCCDF_ABSTRACT_APPENDER(value, MTYPE, MNAME, item.MEMBER) \
		XCCDF_ABSTRACT_APPENDER(group, MTYPE, MNAME, item.MEMBER)

#define XCCDF_STATIC_ITEM_APPENDER(MTYPE, MNAME, MEMBER) \
		static XCCDF_ABSTRACT_APPENDER(item, MTYPE, MNAME, item.MEMBER)

/*--------------------*\
|    Item methods      |
\*--------------------*/

XCCDF_STATIC_ITEM_STRING_SETTER(id)

XCCDF_ITEM_STRING_SETTER(title)
XCCDF_ITEM_STRING_SETTER(description)
XCCDF_ITEM_STRING_SETTER(question)
XCCDF_ITEM_STRING_SETTER(rationale)
XCCDF_ITEM_STRING_SETTER(cluster_id)
XCCDF_ITEM_STRING_SETTER(version)
XCCDF_ITEM_STRING_SETTER(version_update)

XCCDF_ITEM_TYPE_SETTER(float, weight)
XCCDF_ITEM_TYPE_SETTER(time_t, version_time)
XCCDF_ITEM_TYPE_SETTER(struct xccdf_item *, extends)
XCCDF_ITEM_TYPE_SETTER(struct xccdf_item *, parent)

XCCDF_STATIC_ITEM_APPENDER(struct xccdf_status *, status, statuses)
XCCDF_STATIC_ITEM_APPENDER(struct xccdf_reference *, reference, references)
XCCDF_STATIC_ITEM_APPENDER(char *, platform, platforms)

XCCDF_STATIC_ITEM_TYPE_SETTER(struct xccdf_flags, flags)
XCCDF_STATIC_ITEM_TYPE_SETTER(struct xccdf_item *, benchmark)


static struct xccdf_item* _xccdf_benchmark_new(const char *id)
{
	struct xccdf_item *item_new = xccdf_benchmark_new();
	xccdf_item_set_id(item_new, id);
	return item_new;
}

static struct xccdf_item *_xccdf_value_new(const char *id, xccdf_value_type_t type, struct xccdf_item *parent)
{
	struct xccdf_item *item_new = xccdf_value_new(parent, type);
	xccdf_item_set_id(item_new, id);
	return item_new;
}

#define XCCDF_STATIC_ABSTRACT_NEW(TNAME) \
        static struct xccdf_item* _xccdf_##TNAME##_new \
        (const char *id, struct xccdf_item *parent) \
        {\
			struct xccdf_item *item_new = xccdf_##TNAME##_new(parent); \
			xccdf_item_set_id(item_new, id); \
			return item_new; \
        }

XCCDF_STATIC_ABSTRACT_NEW(profile)
XCCDF_STATIC_ABSTRACT_NEW(rule)
XCCDF_STATIC_ABSTRACT_NEW(group)

/*--------------------*\
|   Benchmark methods  |
\*--------------------*/

#define XCCDF_BENCHMARK_STRING_SETTER(MNAME) \
		XCCDF_ABSTRACT_STRING_SETTER(benchmark, MNAME, sub.bench.MNAME)

#define XCCDF_STATIC_BENCHMARK_APPENDER(MTYPE, MNAME, MEMBER) \
		static XCCDF_ABSTRACT_APPENDER(benchmark, MTYPE, MNAME, sub.bench.MEMBER)

struct xccdf_benchmark *xccdf_benchmark_create(const char *id)
{
	return XBENCHMARK(_xccdf_benchmark_new(id));
}

struct xccdf_group *xccdf_benchmark_append_new_group(const struct xccdf_benchmark *benchmark, const char *id)
{
	struct xccdf_item *group = xccdf_benchmark_get_item(benchmark, id);
	if(group==NULL){
		group = _xccdf_group_new(id, XITEM(benchmark));
		oscap_htable_add(XITEM(benchmark)->sub.bench.dict, id, group);
		oscap_list_add(XITEM(benchmark)->sub.bench.content, group);
	}else{
		group = NULL;
	}
	return XGROUP(group);
}

struct xccdf_value *xccdf_benchmark_append_new_value(const struct xccdf_benchmark *benchmark, const char *id, xccdf_value_type_t type)
{
	struct xccdf_item *value = xccdf_benchmark_get_item(benchmark, id);
	if(value==NULL){
		value = _xccdf_value_new(id, type, XITEM(benchmark));
		oscap_htable_add(XITEM(benchmark)->sub.bench.dict, id, value);
		oscap_list_add(XITEM(benchmark)->sub.bench.content, value);
	}else{
		value = NULL;
	}
	return XVALUE(value);
}

struct xccdf_rule *xccdf_benchmark_append_new_rule(const struct xccdf_benchmark *benchmark, const char *id)
{
	struct xccdf_item *rule = xccdf_benchmark_get_item(benchmark, id);
	if(rule==NULL){
		rule = _xccdf_rule_new(id, XITEM(benchmark));
		oscap_htable_add(XITEM(benchmark)->sub.bench.dict, id, rule);
		oscap_list_add(XITEM(benchmark)->sub.bench.content, rule);
	}else{
		rule = NULL;
	}
	return XRULE(rule);
}

struct xccdf_profile *xccdf_benchmark_append_new_profile(const struct xccdf_benchmark *benchmark, const char *id)
{
	struct xccdf_item *profile = xccdf_benchmark_get_item(benchmark, id);
	if(profile==NULL){
		profile = _xccdf_profile_new(id, XITEM(benchmark));
		oscap_htable_add(XITEM(benchmark)->sub.bench.dict, id, profile);
		oscap_list_add(XITEM(benchmark)->sub.bench.content, profile);
	}else{
		profile = NULL;
	}
	return XPROFILE(profile);
}

XCCDF_BENCHMARK_STRING_SETTER(style)
XCCDF_BENCHMARK_STRING_SETTER(style_href)
XCCDF_BENCHMARK_STRING_SETTER(front_matter)
XCCDF_BENCHMARK_STRING_SETTER(rear_matter)

XCCDF_BENCHMARK_STRING_SETTER(metadata)

