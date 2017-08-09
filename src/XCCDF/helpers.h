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
 *      Lukas Kuklinek <lkuklinek@redhat.com>
 */

#ifndef XCCDF_HELPERS_H_
#define XCCDF_HELPERS_H_

#include <assert.h>

#define MACRO_BLOCK(code) do { code } while(false)
#define ASSERT_TYPE(item,t) assert((item)->type & t)
#define ASSERT_BENCHMARK(item) ASSERT_TYPE(item, XCCDF_BENCHMARK)
#define XBENCHMARK(item) ((struct xccdf_benchmark*)item)
#define XPROFILE(item) ((struct xccdf_profile*)item)
#define XGROUP(item) ((struct xccdf_group*)item)
#define XRULE(item) ((struct xccdf_rule*)item)
#define XITEM(item) ((struct xccdf_item*)item)
#define XVALUE(item) ((struct xccdf_value*)item)
#define XRESULT(item) ((struct xccdf_result*)item)

#define XCCDF_STATUS_CURRENT(TYPE) \
		struct xccdf_status * xccdf_##TYPE##_get_status_current(const struct xccdf_##TYPE* item) {\
			return xccdf_item_get_current_status(XITEM(item)); }

#define XCCDF_TEXT_IGETTER(SNAME,MNAME,MEXP) \
        struct oscap_text_iterator* xccdf_##SNAME##_get_##MNAME(const struct xccdf_##SNAME* item) \
        { return oscap_iterator_new(XITEM(item)->MEXP); }
#define XCCDF_GENERIC_GETTER(RTYPE,TNAME,MEMBER) \
        RTYPE xccdf_##TNAME##_get_##MEMBER(const struct xccdf_##TNAME* item) { return (RTYPE)((item)->MEMBER); }
#define XCCDF_GENERIC_IGETTER(ITYPE,TNAME,MNAME) \
        struct xccdf_##ITYPE##_iterator* xccdf_##TNAME##_get_##MNAME(const struct xccdf_##TNAME* item) \
        { return oscap_iterator_new(item->MNAME); }
#define XCCDF_ABSTRACT_GETTER(RTYPE,TNAME,MNAME,MEMBER) \
        RTYPE xccdf_##TNAME##_get_##MNAME(const struct xccdf_##TNAME* item) { return (RTYPE)(XITEM(item)->MEMBER); }
#define XCCDF_ITERATOR_GETTER(ITYPE,TNAME,MNAME,MEMBER) \
        struct xccdf_##ITYPE##_iterator* xccdf_##TNAME##_get_##MNAME(const struct xccdf_##TNAME* item) \
        { return oscap_iterator_new(XITEM(item)->MEMBER); }
#define XCCDF_SITERATOR_GETTER(TNAME,MNAME,MEMBER) \
        struct oscap_string_iterator* xccdf_##TNAME##_get_##MNAME(const struct xccdf_##TNAME* item) \
        { return oscap_iterator_new(XITEM(item)->MEMBER); }
#define XCCDF_TITERATOR_GETTER(TNAME,MNAME,MEMBER) \
        struct oscap_text_iterator* xccdf_##TNAME##_get_##MNAME(const struct xccdf_##TNAME* item) \
        { return oscap_iterator_new(XITEM(item)->MEMBER); }
#define XCCDF_HTABLE_GETTER(RTYPE,TNAME,MNAME,MEMBER) \
		RTYPE xccdf_##TNAME##_get_##MNAME(const struct xccdf_##TNAME* item, const char* key) \
		{ return (RTYPE)oscap_htable_get(XITEM(item)->MEMBER, key); }
#define XCCDF_SIGETTER(TNAME,MNAME) \
        struct oscap_string_iterator* xccdf_##TNAME##_get_##MNAME(const struct xccdf_##TNAME* item) \
        { return oscap_iterator_new(XITEM(item)->sub.TNAME.MNAME); }

#define XCCDF_BENCHMARK_GETTER_A(RTYPE,MNAME,MEMBER) XCCDF_ABSTRACT_GETTER(RTYPE,benchmark,MNAME,MEMBER)
#define XCCDF_BENCHMARK_GETTER_I(RTYPE,MNAME) XCCDF_BENCHMARK_GETTER_A(RTYPE,MNAME,item.MNAME)
#define XCCDF_BENCHMARK_IGETTER_I(ITYPE,MNAME) XCCDF_ITERATOR_GETTER(ITYPE,benchmark,MNAME,item.MNAME)
#define XCCDF_BENCHMARK_GETTER(RTYPE,MNAME) XCCDF_BENCHMARK_GETTER_A(RTYPE,MNAME,sub.benchmark.MNAME)
#define XCCDF_BENCHMARK_IGETTER(ITYPE,MNAME) XCCDF_ITERATOR_GETTER(ITYPE,benchmark,MNAME,sub.benchmark.MNAME)

#define XCCDF_PROFILE_GETTER_A(RTYPE,MNAME,MEMBER) XCCDF_ABSTRACT_GETTER(RTYPE,profile,MNAME,MEMBER)
#define XCCDF_PROFILE_GETTER_I(RTYPE,MNAME) XCCDF_PROFILE_GETTER_A(RTYPE,MNAME,item.MNAME)
#define XCCDF_PROFILE_IGETTER_I(ITYPE,MNAME) XCCDF_ITERATOR_GETTER(ITYPE,profile,MNAME,item.MNAME)
#define XCCDF_PROFILE_GETTER(RTYPE,MNAME) XCCDF_PROFILE_GETTER_A(RTYPE,MNAME,sub.profile.MNAME)
#define XCCDF_PROFILE_IGETTER(ITYPE,MNAME) XCCDF_ITERATOR_GETTER(ITYPE,profile,MNAME,sub.profile.MNAME)

#define XCCDF_RULE_GETTER_A(RTYPE,MNAME,MEMBER) XCCDF_ABSTRACT_GETTER(RTYPE,rule,MNAME,MEMBER)
#define XCCDF_RULE_GETTER_I(RTYPE,MNAME) XCCDF_RULE_GETTER_A(RTYPE,MNAME,item.MNAME)
#define XCCDF_RULE_IGETTER_I(ITYPE,MNAME) XCCDF_ITERATOR_GETTER(ITYPE,rule,MNAME,item.MNAME)
#define XCCDF_RULE_GETTER(RTYPE,MNAME) XCCDF_RULE_GETTER_A(RTYPE,MNAME,sub.rule.MNAME)
#define XCCDF_RULE_IGETTER(ITYPE,MNAME) XCCDF_ITERATOR_GETTER(ITYPE,rule,MNAME,sub.rule.MNAME)

#define XCCDF_GROUP_GETTER_A(RTYPE,MNAME,MEMBER) XCCDF_ABSTRACT_GETTER(RTYPE,group,MNAME,MEMBER)
#define XCCDF_GROUP_GETTER_I(RTYPE,MNAME) XCCDF_GROUP_GETTER_A(RTYPE,MNAME,item.MNAME)
#define XCCDF_GROUP_IGETTER_I(ITYPE,MNAME) XCCDF_ITERATOR_GETTER(ITYPE,group,MNAME,item.MNAME)
#define XCCDF_GROUP_GETTER(RTYPE,MNAME) XCCDF_GROUP_GETTER_A(RTYPE,MNAME,sub.group.MNAME)
#define XCCDF_GROUP_IGETTER(ITYPE,MNAME) XCCDF_ITERATOR_GETTER(ITYPE,group,MNAME,sub.group.MNAME)

#define XCCDF_VALUE_GETTER_A(RTYPE,MNAME,MEMBER) XCCDF_ABSTRACT_GETTER(RTYPE,value,MNAME,MEMBER)
#define XCCDF_VALUE_GETTER_I(RTYPE,MNAME) XCCDF_VALUE_GETTER_A(RTYPE,MNAME,item.MNAME)
#define XCCDF_VALUE_IGETTER_I(ITYPE,MNAME) XCCDF_ITERATOR_GETTER(ITYPE,value,MNAME,item.MNAME)
#define XCCDF_VALUE_GETTER(RTYPE,MNAME) XCCDF_VALUE_GETTER_A(RTYPE,MNAME,sub.value.MNAME)
#define XCCDF_VALUE_IGETTER(ITYPE,MNAME) XCCDF_ITERATOR_GETTER(ITYPE,value,MNAME,sub.value.MNAME)

#define XCCDF_RESULT_GETTER_A(RTYPE,MNAME,MEMBER) XCCDF_ABSTRACT_GETTER(RTYPE,result,MNAME,MEMBER)
#define XCCDF_RESULT_GETTER_I(RTYPE,MNAME) XCCDF_RESULT_GETTER_A(RTYPE,MNAME,item.MNAME)
#define XCCDF_RESULT_IGETTER_I(ITYPE,MNAME) XCCDF_ITERATOR_GETTER(ITYPE,result,MNAME,item.MNAME)
#define XCCDF_RESULT_GETTER(RTYPE,MNAME) XCCDF_RESULT_GETTER_A(RTYPE,MNAME,sub.result.MNAME)
#define XCCDF_RESULT_IGETTER(ITYPE,MNAME) XCCDF_ITERATOR_GETTER(ITYPE,result,MNAME,sub.result.MNAME)

#define XCCDF_SETTER_GENERIC(STYPE,MNAME,MTYPE,FREE,DUP) \
		bool xccdf_##STYPE##_set_##MNAME(struct xccdf_##STYPE* item, MTYPE newval) \
		{ FREE(XITEM(item)->sub.STYPE.MNAME); XITEM(item)->sub.STYPE.MNAME = DUP(newval); return true; }
#define XCCDF_SETTER_SIMPLE(STYPE,MNAME,MTYPE) \
		bool xccdf_##STYPE##_set_##MNAME(struct xccdf_##STYPE* item, MTYPE newval) \
		{ XITEM(item)->sub.STYPE.MNAME = (newval); return true; }
#define XCCDF_SETTER_STRING(STYPE,MNAME) \
		XCCDF_SETTER_GENERIC(STYPE, MNAME, const char*, free, oscap_strdup)
#define XCCDF_ACCESSOR_STRING(STYPE,MNAME) \
		XCCDF_SETTER_STRING(STYPE, MNAME) XCCDF_ABSTRACT_GETTER(const char*, STYPE, MNAME, sub.STYPE.MNAME)
#define XCCDF_ACCESSOR_SIMPLE(STYPE,MTYPE,MNAME) \
		XCCDF_SETTER_SIMPLE(STYPE,MNAME,MTYPE) XCCDF_ABSTRACT_GETTER(MTYPE,STYPE,MNAME,sub.STYPE.MNAME)
#define XCCDF_IGETTER(TNAME,ITYPE,MNAME) \
		XCCDF_ITERATOR_GETTER(ITYPE, TNAME, MNAME, sub.TNAME.MNAME)
#define XCCDF_ADDER(STYPE,MNAME,SINGNAME,MTYPE,DUP) \
		bool xccdf_##STYPE##_add_##SINGNAME(struct xccdf_##STYPE* item, MTYPE newval) \
		{ return oscap_list_add(XITEM(item)->sub.STYPE.MNAME, DUP(newval)); }
#define XCCDF_LISTMANIP(TNAME,ITYPE,MNAME) \
		XCCDF_IGETTER(TNAME,ITYPE,MNAME) XCCDF_ADDER(TNAME,MNAME,ITYPE,struct xccdf_##ITYPE*,)
#define XCCDF_LISTMANIP_STRING(TNAME,SNAME,MNAME) \
		struct oscap_string_iterator* xccdf_##TNAME##_get_##MNAME(const struct xccdf_##TNAME* item) \
		{ return oscap_iterator_new(XITEM(item)->sub.TNAME.MNAME); } \
		XCCDF_ADDER(TNAME,MNAME,SNAME,const char *,oscap_strdup)
#define XCCDF_LISTMANIP_TEXT(TNAME,SNAME,MNAME) \
		struct oscap_text_iterator* xccdf_##TNAME##_get_##MNAME(const struct xccdf_##TNAME* item) \
		{ return oscap_iterator_new(XITEM(item)->sub.TNAME.MNAME); } \
		XCCDF_ADDER(TNAME,MNAME,SNAME,struct oscap_text *,)

#define XCCDF_ITEM_GETTER(RTYPE,MNAME) \
		XCCDF_ABSTRACT_GETTER(RTYPE,item,MNAME,item.MNAME) \
        XCCDF_BENCHMARK_GETTER_A(RTYPE,MNAME,item.MNAME) \
        XCCDF_PROFILE_GETTER_A(RTYPE,MNAME,item.MNAME) \
        XCCDF_RULE_GETTER_A(RTYPE,MNAME,item.MNAME) \
        XCCDF_VALUE_GETTER_A(RTYPE,MNAME,item.MNAME) \
        XCCDF_GROUP_GETTER_A(RTYPE,MNAME,item.MNAME) \
        XCCDF_RESULT_GETTER_A(RTYPE,MNAME,item.MNAME)
#define XCCDF_ITEM_IGETTER(RTYPE,MNAME) \
        XCCDF_ITERATOR_GETTER(RTYPE,item,MNAME,item.MNAME) \
        XCCDF_ITERATOR_GETTER(RTYPE,benchmark,MNAME,item.MNAME) \
        XCCDF_ITERATOR_GETTER(RTYPE,profile,MNAME,item.MNAME) \
        XCCDF_ITERATOR_GETTER(RTYPE,rule,MNAME,item.MNAME) \
        XCCDF_ITERATOR_GETTER(RTYPE,value,MNAME,item.MNAME) \
        XCCDF_ITERATOR_GETTER(RTYPE,group,MNAME,item.MNAME) \
        XCCDF_ITERATOR_GETTER(RTYPE,result,MNAME,item.MNAME)
#define XCCDF_ITEM_SIGETTER(MNAME) \
        XCCDF_SITERATOR_GETTER(item,MNAME,item.MNAME) \
        XCCDF_SITERATOR_GETTER(benchmark,MNAME,item.MNAME) \
        XCCDF_SITERATOR_GETTER(profile,MNAME,item.MNAME) \
        XCCDF_SITERATOR_GETTER(rule,MNAME,item.MNAME) \
        XCCDF_SITERATOR_GETTER(value,MNAME,item.MNAME) \
        XCCDF_SITERATOR_GETTER(group,MNAME,item.MNAME) \
        XCCDF_SITERATOR_GETTER(result,MNAME,item.MNAME)
#define XCCDF_ITEM_TIGETTER(MNAME) \
        XCCDF_TITERATOR_GETTER(item,MNAME,item.MNAME) \
        XCCDF_TITERATOR_GETTER(benchmark,MNAME,item.MNAME) \
        XCCDF_TITERATOR_GETTER(profile,MNAME,item.MNAME) \
        XCCDF_TITERATOR_GETTER(rule,MNAME,item.MNAME) \
        XCCDF_TITERATOR_GETTER(value,MNAME,item.MNAME) \
        XCCDF_TITERATOR_GETTER(group,MNAME,item.MNAME) \
        XCCDF_TITERATOR_GETTER(result,MNAME,item.MNAME)
#define XCCDF_FLAG_SETTER(SNAME, FNAME) \
        bool xccdf_##SNAME##_set_##FNAME(struct xccdf_##SNAME *item, bool newval) \
        { assert(item != NULL); XITEM(item)->item.flags.FNAME = newval; return true; }
#define XCCDF_FLAG_GETTER(MNAME) \
		XCCDF_ABSTRACT_GETTER(bool,item,MNAME,item.flags.MNAME) XCCDF_FLAG_SETTER(item,MNAME) \
        XCCDF_BENCHMARK_GETTER_A(bool,MNAME,item.flags.MNAME)   XCCDF_FLAG_SETTER(benchmark,MNAME) \
        XCCDF_PROFILE_GETTER_A(bool,MNAME,item.flags.MNAME)     XCCDF_FLAG_SETTER(profile,MNAME) \
        XCCDF_RULE_GETTER_A(bool,MNAME,item.flags.MNAME)        XCCDF_FLAG_SETTER(rule,MNAME) \
        XCCDF_GROUP_GETTER_A(bool,MNAME,item.flags.MNAME)       XCCDF_FLAG_SETTER(group,MNAME) \
        XCCDF_VALUE_GETTER_A(bool,MNAME,item.flags.MNAME)       XCCDF_FLAG_SETTER(value,MNAME) \
        XCCDF_RESULT_GETTER_A(bool,MNAME,item.flags.MNAME)      XCCDF_FLAG_SETTER(result,MNAME)

#define XCCDF_ITEM_SETTER_ONE(STYPE,MNAME,MTYPE,DUP) \
		bool xccdf_##STYPE##_set_##MNAME(struct xccdf_##STYPE* item, MTYPE newval) \
		{ XITEM(item)->item.MNAME = DUP(newval); return true; }
#define XCCDF_ITEM_SETTER_ONEF(STYPE,MNAME,MTYPE,FREE,DUP) \
		bool xccdf_##STYPE##_set_##MNAME(struct xccdf_##STYPE* item, MTYPE newval) \
		{ FREE(XITEM(item)->item.MNAME); XITEM(item)->item.MNAME = DUP(newval); return true; }
#define XCCDF_ITEM_SETTER_ONES(STYPE,MNAME) XCCDF_ITEM_SETTER_ONEF(STYPE,MNAME,const char*,free,oscap_strdup)
#define XCCDF_ITEM_SETTER_SIMPLE(MTYPE,MNAME) XCCDF_ITEM_SETTER_ONE(item,MNAME,MTYPE,) \
		XCCDF_ITEM_SETTER_ONE(benchmark,MNAME,MTYPE,) XCCDF_ITEM_SETTER_ONE(profile,MNAME,MTYPE,) \
		XCCDF_ITEM_SETTER_ONE(rule,MNAME,MTYPE,) XCCDF_ITEM_SETTER_ONE(value,MNAME,MTYPE,) \
		XCCDF_ITEM_SETTER_ONE(group,MNAME,MTYPE,) XCCDF_ITEM_SETTER_ONE(result,MNAME,MTYPE,)
#define XCCDF_ITEM_SETTER_STRING(MNAME) XCCDF_ITEM_SETTER_ONES(item,MNAME) \
		XCCDF_ITEM_SETTER_ONES(benchmark,MNAME) XCCDF_ITEM_SETTER_ONES(profile,MNAME) \
		XCCDF_ITEM_SETTER_ONES(rule,MNAME) XCCDF_ITEM_SETTER_ONES(value,MNAME) \
		XCCDF_ITEM_SETTER_ONES(group,MNAME) XCCDF_ITEM_SETTER_ONES(result,MNAME)

#define XCCDF_ITEM_ADDER_ONE(STYPE,MNAME,SINGNAME,MTYPE,DUP) \
		bool xccdf_##STYPE##_add_##SINGNAME(struct xccdf_##STYPE* item, MTYPE newval) \
		{ return oscap_list_add(XITEM(item)->item.MNAME, DUP(newval)); }
#define XCCDF_ITEM_ADDER_ONES(STYPE,MNAME,SINGNAME) XCCDF_ITEM_ADDER_ONE(STYPE,MNAME,SINGNAME,const char*,oscap_strdup)
#define XCCDF_ITEM_ADDER_STRING(MNAMES,MNAME) XCCDF_ITEM_ADDER_ONES(item,MNAME,MNAMES) \
        XCCDF_ITEM_ADDER_ONES(benchmark,MNAME,MNAMES) XCCDF_ITEM_ADDER_ONES(profile,MNAME,MNAMES) \
        XCCDF_ITEM_ADDER_ONES(group,MNAME,MNAMES) XCCDF_ITEM_ADDER_ONES(rule,MNAME,MNAMES) \
        XCCDF_ITEM_ADDER_ONES(value,MNAME,MNAMES) XCCDF_ITEM_ADDER_ONES(result,MNAME,MNAMES)
#define XCCDF_ITEM_ADDER(MTYPE,MNAMES,MNAME) XCCDF_ITEM_ADDER_ONE(item,MNAME,MNAMES,MTYPE,) \
        XCCDF_ITEM_ADDER_ONE(benchmark,MNAME,MNAMES,MTYPE,) XCCDF_ITEM_ADDER_ONE(profile,MNAME,MNAMES,MTYPE,) \
        XCCDF_ITEM_ADDER_ONE(group,MNAME,MNAMES,MTYPE,) XCCDF_ITEM_ADDER_ONE(rule,MNAME,MNAMES,MTYPE,) \
        XCCDF_ITEM_ADDER_ONE(value,MNAME,MNAMES,MTYPE,) XCCDF_ITEM_ADDER_ONE(result,MNAME,MNAMES,MTYPE,)

#define XCCDF_ITEM_ADDER_REG(STYPE, MTYPE, MNAME) \
	bool xccdf_##STYPE##_add_##MTYPE(struct xccdf_##STYPE *STYPE, struct xccdf_##MTYPE *item) \
	{ return xccdf_add_item(XITEM(STYPE)->sub.STYPE.MNAME, XITEM(STYPE), XITEM(item), #MTYPE "-"); }

#define XITERATOR(x) ((struct oscap_iterator*)(x))
#define XCCDF_ITERATOR(n) struct xccdf_##n##_iterator*
#define XCCDF_ITERATOR_FWD(n) struct xccdf_##n##_iterator;
#define XCCDF_ITERATOR_HAS_MORE(n) bool xccdf_##n##_iterator_has_more(XCCDF_ITERATOR(n) it) { return oscap_iterator_has_more(XITERATOR(it)); }
#define XCCDF_ITERATOR_NEXT(t,n) t xccdf_##n##_iterator_next(XCCDF_ITERATOR(n) it) { return oscap_iterator_next(XITERATOR(it)); }
#define XCCDF_ITERATOR_FREE(n) void xccdf_##n##_iterator_free(XCCDF_ITERATOR(n) it) { oscap_iterator_free(XITERATOR(it)); }
#define XCCDF_ITERATOR_RESET(n) void xccdf_##n##_iterator_reset(XCCDF_ITERATOR(n) it) { oscap_iterator_reset(XITERATOR(it)); }
#define XCCDF_ITERATOR_GEN_T(t,n) XCCDF_ITERATOR_FWD(n) XCCDF_ITERATOR_HAS_MORE(n) XCCDF_ITERATOR_RESET(n) XCCDF_ITERATOR_NEXT(t,n) OSCAP_ITERATOR_REMOVE_F(xccdf_##n) XCCDF_ITERATOR_FREE(n)
#define XCCDF_ITERATOR_GEN_S(n) XCCDF_ITERATOR_GEN_T(struct xccdf_##n*,n)
#define XCCDF_ITERATOR_GEN_TEXT(n) XCCDF_ITERATOR_GEN_T(struct oscap_text *,n)

#define XCCDF_FREE_GEN(TYPE) void xccdf_##TYPE##_free(struct xccdf_##TYPE* item) { xccdf_##TYPE##_free_impl((struct xccdf_item *)item); }

#endif
