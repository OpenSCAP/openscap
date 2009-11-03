/**
 * @file oval_resultTest.c
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2008 Red Hat Inc., Durham, North Carolina.
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
 *      "David Niemoller" <David.Niemoller@g2-inc.com>
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <ctype.h>
#include "oval_results_impl.h"
#include "oval_collection_impl.h"
#include "oval_string_map_impl.h"
#include "oval_errno.h"
#if !defined(__FreeBSD__)
# include <alloca.h>
#endif
#define OVAL_RESULT_TEST_DEBUG 0
static int rpmvercmp(const char * a, const char * b); // don't really feel like creating a new header file just for this

/*
 * code from http://rpm.org/api/4.4.2.2/rpmvercmp_8c-source.html
 */

/* compare alpha and numeric segments of two versions */
/* return 1: a is newer than b */
/*        0: a and b are the same version */
/*       -1: b is newer than a */
static int rpmvercmp(const char * a, const char * b)
{
    char oldch1, oldch2;
    char * str1, * str2;
    char * one, * two;
    int rc;
    int isnum;

    /* easy comparison to see if versions are identical */
    if (!strcmp(a, b)) return 0;

    str1 = alloca(strlen(a) + 1);
    str2 = alloca(strlen(b) + 1);

    strcpy(str1, a);
    strcpy(str2, b);

    one = str1;
    two = str2;

    /* loop through each version segment of str1 and str2 and compare them */
    while (*one && *two) {
        while (*one && !isalnum(*one)) one++;
        while (*two && !isalnum(*two)) two++;

        /* If we ran to the end of either, we are finished with the loop */
        if (!(*one && *two)) break;

        str1 = one;
        str2 = two;

        /* grab first completely alpha or completely numeric segment */
        /* leave one and two pointing to the start of the alpha or numeric */
        /* segment and walk str1 and str2 to end of segment */
        if (isdigit(*str1)) {
            while (*str1 && isdigit(*str1)) str1++;
            while (*str2 && isdigit(*str2)) str2++;
            isnum = 1;
        } else {
            while (*str1 && isalpha(*str1)) str1++;
            while (*str2 && isalpha(*str2)) str2++;
            isnum = 0;
        }

        /* save character at the end of the alpha or numeric segment */
        /* so that they can be restored after the comparison */
        oldch1 = *str1;
        *str1 = '\0';
        oldch2 = *str2;
        *str2 = '\0';

        /* this cannot happen, as we previously tested to make sure that */
        /* the first string has a non-null segment */
        if (one == str1) return -1;     /* arbitrary */

        /* take care of the case where the two version segments are */
        /* different types: one numeric, the other alpha (i.e. empty) */
        /* numeric segments are always newer than alpha segments */
        /* XXX See patch #60884 (and details) from bugzilla #50977. */
        if (two == str2) return (isnum ? 1 : -1);

        if (isnum) {
            /* this used to be done by converting the digit segments */
            /* to ints using atoi() - it's changed because long  */
            /* digit segments can overflow an int - this should fix that. */

            /* throw away any leading zeros - it's a number, right? */
            while (*one == '0') one++;
            while (*two == '0') two++;

            /* whichever number has more digits wins */
            if (strlen(one) > strlen(two)) return 1;
            if (strlen(two) > strlen(one)) return -1;
        }

        /* strcmp will return which one is greater - even if the two */
        /* segments are alpha or if they are numeric.  don't return  */
        /* if they are equal because there might be more segments to */
        /* compare */
        rc = strcmp(one, two);
        if (rc) return (rc < 1 ? -1 : 1);

        /* restore character that was replaced by null above */
        *str1 = oldch1;
        one = str1;
        *str2 = oldch2;
        two = str2;
    }
    /* this catches the case where all numeric and alpha segments have */
    /* compared identically but the segment sepparating characters were */
    /* different */
    if ((!*one) && (!*two)) return 0;

    /* whichever version still has characters left over wins */
    if (!*one) return -1; else return 1;
}

typedef struct oval_result_test {
	struct oval_result_system *system;
	struct oval_test *test;
	oval_result_t result;
	struct oval_message    *message;
	struct oval_collection *items;
	struct oval_collection *bindings;
	int instance;
	bool bindings_initialized;
	bool bindings_clearable;
} oval_result_test_t;

struct oval_result_test *oval_result_test_new(struct oval_result_system *sys, char* tstid)
{
	oval_result_test_t *test = (oval_result_test_t *)
		malloc(sizeof(oval_result_test_t));
	struct oval_syschar_model *syschar_model
		= oval_result_system_get_syschar_model(sys);
 	struct oval_definition_model *definition_model
 		= oval_syschar_model_get_definition_model(syschar_model);
	test->system            = sys;
 	test->test = get_oval_test_new(definition_model, tstid);
	test->message              = NULL;
	test->result               = 0;
	test->instance = 0;
	test->items                = oval_collection_new();
	test->bindings             = oval_collection_new();
	test->bindings_initialized = false;
	test->bindings_clearable   = false;
	return test;
}

struct oval_result_test *oval_result_test_clone
	(struct oval_result_test *old_test, struct oval_result_system *new_system)
{
	struct oval_test *oval_test = oval_result_test_get_test(old_test);
	char *testid = oval_test_get_id(oval_test);
	struct oval_result_test *new_test = oval_result_test_new(new_system, testid);
	struct oval_result_item_iterator *old_items = oval_result_test_get_items(old_test);
	while(oval_result_item_iterator_has_more(old_items)){
		struct oval_result_item *old_item = oval_result_item_iterator_next(old_items);
		struct oval_result_item *new_item = oval_result_item_clone(old_item, new_system);
		oval_result_test_add_item(new_test, new_item);
	}
	oval_result_item_iterator_free(old_items);

	struct oval_variable_binding_iterator *old_bindings = oval_result_test_get_bindings(old_test);
	while(oval_variable_binding_iterator_has_more(old_bindings)){
		struct oval_variable_binding *old_binding = oval_variable_binding_iterator_next(old_bindings);
		struct oval_variable_binding *new_binding = oval_variable_binding_clone(old_binding, NULL);
		oval_result_test_add_binding(new_test, new_binding);
	}
	oval_variable_binding_iterator_free(old_bindings);

	struct oval_message *old_message = oval_result_test_get_message(old_test);
	if(old_message){
		struct oval_message *new_message = oval_message_clone(old_message);
		oval_result_test_set_message(new_test, new_message);
	}

	oval_result_test_set_instance(new_test, oval_result_test_get_instance(old_test));
	oval_result_test_set_result(new_test, old_test->result);

	return new_test;
}


struct oval_result_test *make_result_test_from_oval_test
	(struct oval_result_system *sys, struct oval_test *oval_test)
{
	char *test_id = oval_test_get_id(oval_test);
	return oval_result_test_new(sys, test_id);
}

void oval_result_test_free(struct oval_result_test *test)
{
	if(test->message)free(test->message);
	if(test->bindings_clearable)oval_collection_free_items
		(test->bindings, (oscap_destruct_func)oval_variable_binding_free);
	oval_collection_free_items
		(test->items, (oscap_destruct_func)oval_result_item_free);

	test->system            = NULL;
	test->test              = NULL;
	test->message           = NULL;
	test->result            = 0;
	test->items             = NULL;
	test->bindings          = NULL;
	test->instance = 1;
	free(test);
}

int oval_result_test_iterator_has_more(struct oval_result_test_iterator
				       *oc_result_test)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_result_test);
}

struct oval_result_test *oval_result_test_iterator_next(struct
							oval_result_test_iterator
							*oc_result_test)
{
	return (struct oval_result_test *)
	    oval_collection_iterator_next((struct oval_iterator *)
					  oc_result_test);
}

void oval_result_test_iterator_free(struct
							oval_result_test_iterator
							*oc_result_test)
{
    oval_collection_iterator_free((struct oval_iterator *)
					  oc_result_test);
}

struct oval_result_system *oval_result_test_get_system(struct oval_result_test *rtest)
{
	return rtest->system;
}

struct oval_test *oval_result_test_get_test(struct oval_result_test *rtest)
{
	return ((struct oval_result_test *)rtest)->test;
}
static int istrcmp(char *st1,char *st2)
{
	int comp_idx,ret_val;

	if ((!st1)||(!st2))return(1);// if either or both is null, you stink
	for(comp_idx=0,ret_val=0;((!ret_val)&&(st1[comp_idx])&&(st2[comp_idx]));++comp_idx){
		ret_val=tolower(st2[comp_idx])-tolower(st1[comp_idx]);
	}
	return(ret_val);
}
static int strregcomp(char *pattern,char *test_str)
{
	regex_t re;
	int status;

	if ((status=regcomp(&re,pattern,REG_EXTENDED))){
if (OVAL_RESULT_TEST_DEBUG)fprintf(stderr,"%s:%d unable to compile regex pattern:%d\n",__FILE__,__LINE__,status);
			oval_errno=OVAL_INTERNAL_ERROR;
			return(OVAL_RESULT_ERROR);
	}
	if (!(status=regexec(&re,test_str,0,NULL,0))){// got a match
		return(0);
	}else if (status==REG_NOMATCH){// no match, no errror
		return(1);
	}else{
if (OVAL_RESULT_TEST_DEBUG)fprintf(stderr,"%s:%d unable to match regex pattern:%d\n",__FILE__,__LINE__,status);
			oval_errno=OVAL_INTERNAL_ERROR;
			return(OVAL_RESULT_ERROR);
	}
	regfree(&re);
	return(0);
}
// finally, we have gotten to the point of comparing system data with a state
static oval_result_t evaluate(char *sys_data,char *state_data,oval_datatype_t sys_data_type,oval_datatype_t state_data_type,oval_operation_t operation)
{
	if (state_data_type==OVAL_DATATYPE_STRING){
		if (operation==OVAL_OPERATION_EQUALS){
			return((strcmp(state_data,sys_data))?OVAL_RESULT_FALSE:OVAL_RESULT_TRUE);
		}else if (operation==OVAL_OPERATION_CASE_INSENSITIVE_EQUALS){
			return((istrcmp(state_data,sys_data))?OVAL_RESULT_FALSE:OVAL_RESULT_TRUE);
		}else if (operation==OVAL_OPERATION_NOT_EQUAL){
			return((strcmp(state_data,sys_data))?OVAL_RESULT_TRUE:OVAL_RESULT_FALSE);
		}else if (operation==OVAL_OPERATION_CASE_INSENSITIVE_NOT_EQUAL){
			return((istrcmp(state_data,sys_data))?OVAL_RESULT_TRUE:OVAL_RESULT_FALSE);
		}else if (operation==OVAL_OPERATION_PATTERN_MATCH){
			return((strregcomp(state_data,sys_data))?OVAL_RESULT_FALSE:OVAL_RESULT_TRUE);
		}else{
if (OVAL_RESULT_TEST_DEBUG)fprintf(stderr,"%s:%d invalid string comparison:%d\n",__FILE__,__LINE__,operation);
			oval_errno=OVAL_INVALID_COMPARISON;
			return(OVAL_RESULT_INVALID);
		}
	}else if (state_data_type==OVAL_DATATYPE_INTEGER){
		int state_val,syschar_val;
		state_val=atoi(state_data);
		syschar_val=atoi(sys_data);
		if (operation==OVAL_OPERATION_EQUALS){
			return((state_val==syschar_val)?OVAL_RESULT_TRUE:OVAL_RESULT_FALSE);
		}else if (operation==OVAL_OPERATION_NOT_EQUAL){
			return((state_val!=syschar_val)?OVAL_RESULT_TRUE:OVAL_RESULT_FALSE);
		}else if (operation==OVAL_OPERATION_GREATER_THAN){
			return((syschar_val>state_val)?OVAL_RESULT_TRUE:OVAL_RESULT_FALSE);
		}else if (operation==OVAL_OPERATION_GREATER_THAN_OR_EQUAL){
			return((syschar_val>=state_val)?OVAL_RESULT_TRUE:OVAL_RESULT_FALSE);
		}else if (operation==OVAL_OPERATION_LESS_THAN){
			return((syschar_val<state_val)?OVAL_RESULT_TRUE:OVAL_RESULT_FALSE);
		}else if (operation==OVAL_OPERATION_LESS_THAN_OR_EQUAL){
			return((syschar_val<=state_val)?OVAL_RESULT_TRUE:OVAL_RESULT_FALSE);
		}else{
if (OVAL_RESULT_TEST_DEBUG)fprintf(stderr,"%s:%d invalid integer comparison:%d\n",__FILE__,__LINE__,operation);
			oval_errno=OVAL_INVALID_COMPARISON;
			return(OVAL_RESULT_INVALID);
		}
	}else if (state_data_type==OVAL_DATATYPE_BOOLEAN){
		int state_int;
		int sys_int;
		state_int=(((strcmp(state_data,"true"))==0)||((strcmp(state_data,"1"))==0))?1:0;
		sys_int=(((strcmp(sys_data,"true"))==0)||((strcmp(sys_data,"1"))==0))?1:0;
		if (operation==OVAL_OPERATION_EQUALS){
			return((state_int==sys_int)?OVAL_RESULT_TRUE:OVAL_RESULT_FALSE);
		}else if (operation==OVAL_OPERATION_NOT_EQUAL){
			return((state_int!=sys_int)?OVAL_RESULT_TRUE:OVAL_RESULT_FALSE);
		}else{
if (OVAL_RESULT_TEST_DEBUG)fprintf(stderr,"%s:%d invalid boolean comparison:%d\n",__FILE__,__LINE__,operation);
			oval_errno=OVAL_INVALID_COMPARISON;
			return(OVAL_RESULT_INVALID);
		}
	}else if (state_data_type==OVAL_DATATYPE_BINARY){// I'm going to use case insensitive compare here - don't know if it's necessary
		if (operation==OVAL_OPERATION_EQUALS){
			return((istrcmp(state_data,sys_data)==0)?OVAL_RESULT_TRUE:OVAL_RESULT_FALSE);
		}else if (operation==OVAL_OPERATION_NOT_EQUAL){
			return((istrcmp(state_data,sys_data)!=0)?OVAL_RESULT_TRUE:OVAL_RESULT_FALSE);
		}else{
if (OVAL_RESULT_TEST_DEBUG)fprintf(stderr,"%s:%d invalid binary comparison:%d\n",__FILE__,__LINE__,operation);
			oval_errno=OVAL_INVALID_COMPARISON;
			return(OVAL_RESULT_INVALID);
		}
	}else if (state_data_type==OVAL_DATATYPE_EVR_STRING){
		int result;
		result=rpmvercmp(sys_data,state_data);
		if (operation==OVAL_OPERATION_EQUALS){
			return((result==0)?OVAL_RESULT_TRUE:OVAL_RESULT_FALSE);
		}else if (operation==OVAL_OPERATION_NOT_EQUAL){
			return((result!=0)?OVAL_RESULT_TRUE:OVAL_RESULT_FALSE);
		}else if (operation==OVAL_OPERATION_GREATER_THAN){
			return((result==1)?OVAL_RESULT_TRUE:OVAL_RESULT_FALSE);
		}else if (operation==OVAL_OPERATION_GREATER_THAN_OR_EQUAL){
			return((result!=-1)?OVAL_RESULT_TRUE:OVAL_RESULT_FALSE);
		}else if (operation==OVAL_OPERATION_LESS_THAN){
			return((result==-1)?OVAL_RESULT_TRUE:OVAL_RESULT_FALSE);
		}else if (operation==OVAL_OPERATION_LESS_THAN_OR_EQUAL){
			return((result!=1)?OVAL_RESULT_TRUE:OVAL_RESULT_FALSE);
		}else{
if (OVAL_RESULT_TEST_DEBUG)fprintf(stderr,"%s:%d invalid EVR comparison:%d\n",__FILE__,__LINE__,operation);
			oval_errno=OVAL_INVALID_COMPARISON;
			return(OVAL_RESULT_INVALID);
		}
	}else if (state_data_type==OVAL_DATATYPE_VERSION){
		int state_idx,sys_idx;
		int result=-1;
		int is_equal=1;
		for(state_idx=0,sys_idx=0;(((state_data[state_idx])||(sys_data[sys_idx]))&&(result==-1));){ // keep going as long as there is data in either the state or sysdata
			int tmp_state_int,tmp_sys_int;
			tmp_state_int=atoi(&state_data[state_idx]);// look at the current data field (if we're at the end, atoi should return 0)
			tmp_sys_int=atoi(&sys_data[sys_idx]);
			if (tmp_state_int!=tmp_sys_int)is_equal=0;// we might need this later (if we don't terminate early)
			if (operation==OVAL_OPERATION_EQUALS){
				if (tmp_state_int!=tmp_sys_int)return(OVAL_RESULT_FALSE);
			}else if (operation==OVAL_OPERATION_NOT_EQUAL){
				if (tmp_state_int!=tmp_sys_int)return(OVAL_RESULT_TRUE);
			}else if ((operation==OVAL_OPERATION_GREATER_THAN)||(operation==OVAL_OPERATION_GREATER_THAN_OR_EQUAL)){
				if (tmp_sys_int>tmp_state_int)return(OVAL_RESULT_TRUE);
				if (tmp_sys_int<tmp_state_int) return(OVAL_RESULT_FALSE);
			}else if ((operation==OVAL_OPERATION_LESS_THAN)||(operation==OVAL_OPERATION_LESS_THAN_OR_EQUAL)){
				if (tmp_sys_int<tmp_state_int) return(OVAL_RESULT_TRUE);
				if (tmp_sys_int>tmp_state_int) return(OVAL_RESULT_FALSE);
			}else{
if (OVAL_RESULT_TEST_DEBUG)fprintf(stderr,"%s:%d invalid version comparison:%d\n",__FILE__,__LINE__,operation);
				oval_errno=OVAL_INVALID_COMPARISON;
				return(OVAL_RESULT_INVALID);
			}
			for(state_idx=(state_data[state_idx])?state_idx++:state_idx;((state_data[state_idx])&&(isdigit(state_data[state_idx])));++state_idx);// move to the next field within the version string (if there is one)
			if ((state_data[state_idx])&&(!isdigit(state_data[state_idx])))++state_idx;
			for(sys_idx=(sys_data[sys_idx])?++sys_idx:sys_idx;((sys_data[sys_idx])&&(isdigit(sys_data[sys_idx])));++sys_idx);// move to the next field within the version string (if there is one)
			if ((sys_data[sys_idx])&&(!isdigit(sys_data[sys_idx])))++sys_idx;
		}
		// OK, we did not terminate early, and we're out of data, so we now know what to return
		if (operation==OVAL_OPERATION_EQUALS){ return(OVAL_RESULT_TRUE);
		}else if (operation==OVAL_OPERATION_NOT_EQUAL){ return(OVAL_RESULT_FALSE);
		}else if (operation==OVAL_OPERATION_GREATER_THAN){ return(OVAL_RESULT_FALSE);
		}else if (operation==OVAL_OPERATION_GREATER_THAN_OR_EQUAL){ return(OVAL_RESULT_TRUE);
		}else if (operation==OVAL_OPERATION_LESS_THAN){ return(OVAL_RESULT_FALSE);
		}else if (operation==OVAL_OPERATION_LESS_THAN_OR_EQUAL){ return(OVAL_RESULT_TRUE); }// we have already filtered out the invalid ones
	}else{
		oval_errno=OVAL_UNSUPPORTED_DATATYPE;
		return(-1);
	}
	return(OVAL_RESULT_UNKNOWN);
}

// here we compare the data within an item with the conditions in a state
static oval_result_t eval_item(struct oval_sysdata *cur_sysdata, struct oval_state *state)
{
	struct oval_state_content_iterator *state_contents;

	if (state==NULL){
		bool has_more_items = false, syschar_status_exists = false;
		struct oval_sysitem_iterator *cur_items=oval_sysdata_get_items(cur_sysdata);
		while((has_more_items=oval_sysitem_iterator_has_more(cur_items))){
			struct oval_sysitem *tmp_item=oval_sysitem_iterator_next(cur_items);
			if ((syschar_status_exists = oval_sysitem_get_status(tmp_item))==SYSCHAR_STATUS_EXISTS)
				break;//return(OVAL_RESULT_TRUE);
		}
		if(has_more_items)
			while(oval_sysitem_iterator_has_more(cur_items))
				oval_sysitem_iterator_next(cur_items);
		oval_sysitem_iterator_free(cur_items);
		return (syschar_status_exists)?OVAL_RESULT_TRUE:OVAL_RESULT_FALSE;
	}else{
		state_contents=oval_state_get_contents(state);
	}
	bool has_more_content = false, has_error = false;
	oval_result_t result = OVAL_RESULT_INVALID;
	while(!has_error && (has_more_content=oval_state_content_iterator_has_more(state_contents))){
		struct oval_state_content *content;
		struct oval_entity *state_entity;
		struct oval_value *state_value;
		char *state_entity_name;
		bool iterator_is_not_empty;
		int found_it;

		if (!has_error && (content=oval_state_content_iterator_next(state_contents))==NULL){
if(OVAL_RESULT_TEST_DEBUG)fprintf(stderr,"%s:%d found NULL state content\n",__FILE__,__LINE__);
			oval_errno=OVAL_INTERNAL_ERROR;
			has_error = true;
		}
		//state_entity=oval_state_content_entity(content);
		if (!has_error && (state_entity=oval_state_content_get_entity(content))==NULL){
if(OVAL_RESULT_TEST_DEBUG)fprintf(stderr,"%s:%d found NULL entity\n",__FILE__,__LINE__);
			oval_errno=OVAL_INTERNAL_ERROR;
			has_error = true;
		}
		//state_entity_name=oval_entity_name(state_entity);
		if (!has_error && (state_entity_name=oval_entity_get_name(state_entity))==NULL){
if(OVAL_RESULT_TEST_DEBUG)fprintf(stderr,"%s:%d found NULL entity name\n",__FILE__,__LINE__);
			oval_errno=OVAL_INTERNAL_ERROR;
			has_error = true;
		}
		if (!has_error && (state_value=oval_entity_get_value(state_entity))==NULL){
if(OVAL_RESULT_TEST_DEBUG)fprintf(stderr,"%s:%d found NULL entity value\n",__FILE__,__LINE__);
			oval_errno=OVAL_INTERNAL_ERROR;
			has_error = true;
		}
		if(!has_error){
			struct oval_sysitem_iterator *cur_items=oval_sysdata_get_items(cur_sysdata);
			for(found_it=0;((iterator_is_not_empty = oval_sysitem_iterator_has_more(cur_items))&&(!found_it));){
				char *syschar_entity_name;
				struct oval_sysitem *syschar_item;

				syschar_item=oval_sysitem_iterator_next(cur_items);
				if (syschar_item==NULL){
	if(OVAL_RESULT_TEST_DEBUG)fprintf(stderr,"%s:%d found NULL sysitem\n",__FILE__,__LINE__);
					oval_errno=OVAL_INTERNAL_ERROR;
					has_error = true;
				}
				syschar_entity_name=oval_sysitem_get_name(syschar_item);
				if (!strcmp(syschar_entity_name,state_entity_name)){
					found_it=1;
					result=evaluate(oval_sysitem_get_value(syschar_item)
	                                 ,oval_value_get_text(state_value)
	                                 ,oval_sysitem_get_datatype(syschar_item)
	                                 ,oval_value_get_datatype(state_value)
	                                 ,oval_entity_get_operation(state_entity));
					// TODO:  this does not cover all possible valid oval content
	                // come back and add support for none/all/only one/at least one....
				}
			}
			if(iterator_is_not_empty)while(oval_sysitem_iterator_has_more(cur_items))oval_sysitem_iterator_next(cur_items);
			oval_sysitem_iterator_free(cur_items);
		}
	}
	if(has_more_content)
		while(oval_state_content_iterator_has_more(state_contents))
			oval_state_content_iterator_next(state_contents);
	oval_state_content_iterator_free(state_contents);
	return (has_error)?-1:result;
}
//typedef enum {
	//OVAL_RESULT_INVALID        = 0,
	//OVAL_RESULT_TRUE           = 1,
	//OVAL_RESULT_FALSE          = 2,
	//OVAL_RESULT_UNKNOWN        = 3,
	//OVAL_RESULT_ERROR          = 4,
	//OVAL_RESULT_NOT_EVALUATED  = 5,
	//OVAL_RESULT_NOT_APPLICABLE = 6
//} oval_result_enum;
//typedef enum {
	//SYSCHAR_FLAG_UNKNOWN         = 0,
	//SYSCHAR_FLAG_ERROR           = 1,
	//SYSCHAR_FLAG_COMPLETE        = 2,
	//SYSCHAR_FLAG_INCOMPLETE      = 3,
	//SYSCHAR_FLAG_DOES_NOT_EXIST  = 4,
	//SYSCHAR_FLAG_NOT_COLLECTED   = 5,
	//SYSCHAR_FLAG_NOT_APPLICABLE  = 6
//} oval_syschar_collection_flag_t;
// here's where the actual OVAL business logic starts

#define ITEMMAP (struct oval_string_map    *)args[2]
#define TEST    (struct oval_result_test   *)args[1]
#define SYSTEM  (struct oval_result_system *)args[0]

static void _oval_test_item_consumer
	(struct oval_result_item *item, void **args)
{
	struct oval_sysdata *oval_sysdata = oval_result_item_get_sysdata(item);
	char *item_id = oval_sysdata_get_id(oval_sysdata);
	struct oval_result_item *mapped_item = oval_string_map_get_value(ITEMMAP, item_id);
	if(mapped_item==NULL){
		oval_string_map_put(ITEMMAP, item_id, item);
		oval_result_test_add_item(TEST, item);
	}
}

//int debug_flag=0;
static oval_result_t _oval_result_test_evaluate_items
	(struct oval_syschar *syschar_object, struct oval_state *state,oval_check_t test_check,oval_existence_t test_existence_check, void **args)
{
	int matches_found;
	int am_done=0;
	oval_syschar_collection_flag_t collection_flag;
	oval_result_t result;
	struct oval_sysdata_iterator *collected_items_iterator;
	bool has_more_sysdata, has_error = false;
	collection_flag=oval_syschar_get_flag(syschar_object);
	result = OVAL_RESULT_INVALID;
//if (debug_flag)fprintf(stderr,"%s:%d collection flag is:%d\n",__FILE__,__LINE__,collection_flag);
	if (collection_flag==SYSCHAR_FLAG_ERROR){// OK, the test_check and test_existence_check must simultaniously be true
		am_done=1;
		result=OVAL_RESULT_ERROR;
	}else if (collection_flag==SYSCHAR_FLAG_NOT_COLLECTED){
		am_done=1;
		result=OVAL_RESULT_UNKNOWN;
	}else if (collection_flag==SYSCHAR_FLAG_NOT_APPLICABLE){
		am_done=1;
		result=OVAL_RESULT_NOT_APPLICABLE;
	}else if (collection_flag==SYSCHAR_FLAG_DOES_NOT_EXIST){
		if ((test_check==OVAL_CHECK_NONE_EXIST)||(test_existence_check==OVAL_NONE_EXIST)){
			am_done=1;
			result=OVAL_RESULT_TRUE;
		}else if ((test_check==OVAL_CHECK_AT_LEAST_ONE)||(test_check==OVAL_CHECK_ONLY_ONE)||(test_existence_check==OVAL_ONLY_ONE_EXISTS)||(test_existence_check==OVAL_AT_LEAST_ONE_EXISTS)){
			am_done=1;
			result=OVAL_RESULT_FALSE;
		}
	}else if (collection_flag==SYSCHAR_FLAG_COMPLETE){
//if (debug_flag)fprintf(stderr,"%s:%d test_existence_check:%d none_exist:%d\n",__FILE__,__LINE__,test_existence_check,NONE_EXIST);
		if (test_existence_check==OVAL_NONE_EXIST){
//if (debug_flag)fprintf(stderr,"%s:%d setting result to false and done to true\n",__FILE__,__LINE__);
			am_done=1;
			result=OVAL_RESULT_FALSE;
		}
	}
	collected_items_iterator=oval_syschar_get_sysdata(syschar_object);
	if (collected_items_iterator==NULL){
if(OVAL_RESULT_TEST_DEBUG)fprintf(stderr,"%s:%d collected items iterator is null\n",__FILE__,__LINE__);
		oval_errno=OVAL_INVALID_ARGUMENT;
		return(-1);
	}
	for(matches_found=0;(am_done==0)&&(!has_error) && (has_more_sysdata=oval_sysdata_iterator_has_more(collected_items_iterator));){
		struct oval_sysdata *cur_sysdata;
		oval_syschar_status_t cur_sysdata_status;

		cur_sysdata=oval_sysdata_iterator_next(collected_items_iterator);
		char *cur_sysdata_id = oval_sysdata_get_id(cur_sysdata);
		struct oval_result_item *cur_item = oval_result_item_new(SYSTEM, cur_sysdata_id);
		if (cur_sysdata==NULL){
if(OVAL_RESULT_TEST_DEBUG)fprintf(stderr,"%s:%d iterator returned null\n",__FILE__,__LINE__);
			oval_errno=OVAL_INTERNAL_ERROR;
			has_error = true;
		}
		cur_sysdata_status=oval_sysdata_get_status(cur_sysdata);
		_oval_test_item_consumer(cur_item, args);
		if (cur_sysdata_status==SYSCHAR_STATUS_UNKNOWN){
			result=OVAL_RESULT_UNKNOWN;
			oval_result_item_set_result(cur_item, result);
		}else if (cur_sysdata_status==SYSCHAR_STATUS_ERROR){
			result=OVAL_RESULT_ERROR;
			oval_result_item_set_result(cur_item, result);
		}else if (cur_sysdata_status==SYSCHAR_STATUS_EXISTS){ // found one, eval this item
//if (debug_flag) fprintf(stderr,"%s:%d status is 'exists', doing eval_item with state:'%s'\n",__FILE__,__LINE__,(state!=NULL)?oval_state_get_id(state):"null");
			result=eval_item(cur_sysdata,state);
			oval_result_item_set_result(cur_item, result);
//if (debug_flag) fprintf(stderr,"%s:%d eval_item returned:%d\n",__FILE__,__LINE__,result);
			// we know one result, sometimes we can use that to make the full determination
			if (result==OVAL_RESULT_TRUE){
				if (test_check==OVAL_CHECK_AT_LEAST_ONE){
					result=OVAL_RESULT_TRUE;
					am_done=1;
				}else if ((test_check==OVAL_CHECK_ONLY_ONE)&&(matches_found>0)){
					result=OVAL_RESULT_FALSE;
					am_done=1;
				}else if (test_check==OVAL_CHECK_NONE_SATISFY){
					result=OVAL_RESULT_FALSE;
					am_done=1;
				}
				++matches_found;
			}else if (result==OVAL_RESULT_FALSE){
				if (test_check==OVAL_CHECK_ALL){
					result=OVAL_RESULT_FALSE;
					am_done=1;
				}
			}else if (result==OVAL_RESULT_INVALID){// the OVAL truth table does not include 'INVALID' so I'm ignoring it for now...
			}else if (result==OVAL_RESULT_UNKNOWN){
				result=OVAL_RESULT_UNKNOWN;
				am_done=1;
			}else if (result==OVAL_RESULT_ERROR){
				result=OVAL_RESULT_ERROR;
				am_done=1;
			}else if (result==OVAL_RESULT_NOT_EVALUATED){
				result=OVAL_RESULT_NOT_EVALUATED;
				am_done=1;
			}else if (result==OVAL_RESULT_NOT_APPLICABLE){
				result=OVAL_RESULT_NOT_APPLICABLE;
				am_done=1;
			}else{
			}
		}else if (cur_sysdata_status==SYSCHAR_STATUS_DOES_NOT_EXIST){
			result=OVAL_RESULT_FALSE;
			oval_result_item_set_result(cur_item, result);
		}else if (cur_sysdata_status==SYSCHAR_STATUS_NOT_COLLECTED){
			result=OVAL_RESULT_NOT_EVALUATED;
			oval_result_item_set_result(cur_item, result);
		}else{
if(OVAL_RESULT_TEST_DEBUG)fprintf(stderr,"%s:%d invalid sysdata status:%d\n",__FILE__,__LINE__,cur_sysdata_status);
			oval_errno=OVAL_INVALID_ARGUMENT;
			has_error = true;
		}
		oval_result_item_set_result(cur_item,result);
	}
// OK, we have looked at all our collected items and matched them against any states, with no early termination
	// we now know how to set our final result
	if (!am_done){// if we have not yet made a final determination
//if (debug_flag)fprintf(stderr,"%s:%d did not terminate early, check:%d matches:%d\n",__FILE__,__LINE__,test_check,matches_found);
		if ((test_check==OVAL_CHECK_ONLY_ONE)&&(matches_found==1)){
			result=OVAL_RESULT_TRUE;
		}else if (test_check==OVAL_CHECK_NONE_SATISFY){// since we did not terminate early, all we have are falses
			result=OVAL_RESULT_TRUE;
		}else if ((test_check==OVAL_CHECK_ALL)&&(matches_found>0)){// since we did not terminate early, all we found were true
			result=OVAL_RESULT_TRUE;
		}else{// all the 'true' conditions should be covered above, so if we get here, our result is...
			result=OVAL_RESULT_FALSE;
		}
	}
	if(has_more_sysdata)
		while(oval_sysdata_iterator_has_more(collected_items_iterator))
			oval_sysdata_iterator_next(collected_items_iterator);
	oval_sysdata_iterator_free(collected_items_iterator);
	return (has_error)?-1:result;
}

// this function will gather all the necessary ingredients and call 'evaluate_items' when it finds them
static oval_result_t _oval_result_test_result(struct oval_result_test *rtest, void **args)
{
	// NOTE:  I'm defining all my variables at the beginning of a block because some compilers (cl) have trouble
	// with variables defined anywhere else.  If that's not a concern, refactor at will.
	struct oval_object *tmp_obj;
	struct oval_state *tmp_state;
	char *test_id_string;
	struct oval_test *test2check;
	struct oval_result_system *sys;
	struct oval_syschar_model *syschar_model;

	oval_check_t test_check;
	oval_existence_t test_check_existence;
	test2check = oval_result_test_get_test(rtest);
	sys = oval_result_test_get_system(rtest);
	syschar_model = oval_result_system_get_syschar_model(sys);
	if (rtest==NULL){
		oval_errno=OVAL_INVALID_ARGUMENT;
		return(-1);
	}
	// first, let's see if we already did the test
	if (rtest->result!=OVAL_RESULT_INVALID){
if(OVAL_RESULT_TEST_DEBUG)fprintf(stderr,"%s:%d found result from previous evaluation, returning without further processing\n",__FILE__,__LINE__);
		return(rtest->result);
	}
	oval_result_t result = OVAL_RESULT_INVALID;
	// let's go looking for the stuff to test
	if (test2check==NULL){
		oval_errno=OVAL_INVALID_ARGUMENT;
		return(-1);
	}
	test_id_string=oval_test_get_id(test2check);
	if (test_id_string==NULL){
if(OVAL_RESULT_TEST_DEBUG)fprintf(stderr,"%s:%d oval test id is null\n",__FILE__,__LINE__);
		oval_errno=OVAL_INVALID_ARGUMENT;
		return(-1);
	}
//if (!strcmp("oval:org.mitre.oval:tst:99",test_id_string))debug_flag=1;
//else debug_flag=0;
	test_check=oval_test_get_check(test2check);
	test_check_existence=oval_test_get_existence(test2check);
	tmp_obj=oval_test_get_object(test2check);
	tmp_state=oval_test_get_state(test2check);
	if (tmp_obj==NULL){
if(OVAL_RESULT_TEST_DEBUG)fprintf(stderr,"%s:%d oval object is null\n",__FILE__,__LINE__);
		oval_errno=OVAL_INVALID_ARGUMENT;
		return(-1);
	} else {  // I'm doing the else here to keep some of my local variables more local
		char *definition_object_id_string;
		struct oval_syschar *syschar_object;
		definition_object_id_string=oval_object_get_id(tmp_obj);
		if (definition_object_id_string==NULL){
if(OVAL_RESULT_TEST_DEBUG)fprintf(stderr,"%s:%d oval object has null ID\n",__FILE__,__LINE__);
			oval_errno=OVAL_INVALID_ARGUMENT;
			return(-1);
		}
		// OK, we have our object ID, now use that to find selected items in the syschar_model
		syschar_object=oval_syschar_model_get_syschar(syschar_model,definition_object_id_string);
		if (syschar_object==NULL){
if(OVAL_RESULT_TEST_DEBUG)fprintf(stderr,"%s:%d system characteristics object is null\n",__FILE__,__LINE__);
			oval_errno=OVAL_INVALID_ARGUMENT;
			return(-1);
		}
		// FINALLY, we have all the pieces, now figure out the result
		result=_oval_result_test_evaluate_items(syschar_object,tmp_state,test_check,test_check_existence, args);
	}
	return result;
}

oval_result_t oval_result_test_get_result(struct oval_result_test *rtest)
{

	if(rtest->result==OVAL_RESULT_INVALID){
		void *args[] = {rtest->system, rtest, oval_string_map_new()};
		rtest->result = _oval_result_test_result(rtest, args);
		if(rtest->result==-1)rtest->result=OVAL_RESULT_UNKNOWN;
	}
	return rtest->result;
}

int oval_result_test_get_instance(struct oval_result_test *rtest)
{
	return rtest->instance;
}

struct oval_message *oval_result_test_get_message(struct oval_result_test *rtest)
{
	return ((struct oval_result_test *)rtest)->message;
}

struct oval_result_item_iterator *oval_result_test_get_items(struct oval_result_test *rtest)
{
	return (struct oval_result_item_iterator *)
	    oval_collection_iterator(rtest->items);
}

struct oval_variable_binding_iterator *oval_result_test_get_bindings(struct oval_result_test
							 *rtest)
{
	return (struct oval_variable_binding_iterator *)
	    oval_collection_iterator(rtest->bindings);
}

void oval_result_test_set_result(struct oval_result_test *test, oval_result_t result)
{
	test->result = result;
}

void oval_result_test_set_instance(struct oval_result_test *test, int instance)
{
	test->instance = instance;
}

void oval_result_test_set_message
	(struct oval_result_test *test, struct oval_message *message)
{
	if(test->message)oval_message_free(test->message);
	test->message = message;
}

void oval_result_test_add_item
	(struct oval_result_test *test, struct oval_result_item *item)
{
	oval_collection_add(test->items, item);
}

void oval_result_test_add_binding
	(struct oval_result_test *test, struct oval_variable_binding *binding)
{
	oval_collection_add(test->bindings, binding);
}

//void(*oscap_consumer_func)(void*, void*);
static void _oval_test_message_consumer
	(struct oval_message *message, struct oval_result_test *test)
{
	oval_result_test_set_message(test, message);
}

static int _oval_result_test_binding_parse
	(xmlTextReaderPtr reader, struct oval_parser_context *context, void **args)
{
	int return_code = 1;

	xmlChar *variable_id = xmlTextReaderGetAttribute(reader, BAD_CAST "variable_id");

	struct oval_syschar_model *syschar_model = oval_result_system_get_syschar_model(SYSTEM);
        struct oval_definition_model *definition_model = oval_syschar_model_get_definition_model(syschar_model);

	struct oval_variable *variable = get_oval_variable_new
 		(definition_model, variable_id, OVAL_VARIABLE_UNKNOWN);

	xmlChar *value = xmlTextReaderValue(reader);

	struct oval_variable_binding *binding = oval_variable_binding_new(variable, value);
	oval_result_test_add_binding(TEST, binding);

	xmlFree(value);
	xmlFree(variable_id);

	return return_code;
}

static int _oval_result_test_parse
	(xmlTextReaderPtr reader, struct oval_parser_context *context, void **args)
{
	int return_code = 1;
	xmlChar *localName = xmlTextReaderLocalName(reader);

	if(OVAL_RESULT_TEST_DEBUG){
		char message[200]; *message = '\0';
		sprintf(message, "_oval_result_test_parse: parsing <%s>", localName);
		oval_parser_log_debug(context, message);
	}


	if      (strcmp((const char *)localName, "message")==0){
		return_code  = oval_message_parse_tag
			(reader, context, (oscap_consumer_func)_oval_test_message_consumer, TEST);
	}else if(strcmp((const char *)localName, "tested_item")==0){
		return_code = oval_result_item_parse_tag
			(reader, context, SYSTEM,
				(oscap_consumer_func)_oval_test_item_consumer, args);
	}else if(strcmp((const char *)localName, "tested-variable")==0){
		return_code = _oval_result_test_binding_parse(reader, context, args);
	}else{
		char message[200]; *message = '\0';
		sprintf(message, "_oval_result_test_parse: TODO: <%s> not handled", localName);
		oval_parser_log_warn(context, message);
		return_code = oval_parser_skip_tag(reader, context);
	}

	free(localName);

	return return_code;
}

int oval_result_test_parse_tag
	(xmlTextReaderPtr reader, struct oval_parser_context *context,
			struct oval_result_system *sys,
			oscap_consumer_func consumer, void *client)
{
	int return_code = 1;
	if(OVAL_RESULT_TEST_DEBUG){
		oval_parser_log_debug(context, "oval_result_test_parse: BEGIN");
	}

	xmlChar *test_id = xmlTextReaderGetAttribute(reader, BAD_CAST "test_id");
	struct oval_result_test *test = oval_result_test_new(sys, test_id);
	oval_result_t result = oval_result_parse(reader, "result",0);
	oval_result_test_set_result(test, result);
	int veriable_instance = oval_parser_int_attribute(reader, "veriable_instance", 1);
	oval_result_test_set_instance(test, veriable_instance);

	struct oval_test *ovaltst = oval_result_test_get_test(test);

	oval_existence_t check_existence = oval_existence_parse
		(reader,"check_existence",OVAL_AT_LEAST_ONE_EXISTS);
	oval_existence_t tst_check_existence
		= oval_test_get_existence(ovaltst);
	if(tst_check_existence==OVAL_EXISTENCE_UNKNOWN){
		oval_test_set_existence(ovaltst, check_existence);
	}else if(tst_check_existence!=tst_check_existence){
		char message[200];*message = '\0';
		sprintf
		(
				message,
				"oval_result_test_parse: @check_existence does not match\n"
				"    test_id = %s",
				test_id
		);
		oval_parser_log_warn(context, message);
	}

	oval_check_t check = oval_check_parse
		(reader, "check",OVAL_CHECK_UNKNOWN);
	oval_check_t tst_check
		= oval_test_get_check(ovaltst);
	if(tst_check==OVAL_CHECK_UNKNOWN){
		oval_test_set_check(ovaltst, check);
	}else if(tst_check!=check){
		char message[200];*message = '\0';
		sprintf
		(
				message,
				"oval_result_test_parse: @check does not match\n"
				"    test_id = %s",
				test_id
		);
		oval_parser_log_warn(context, message);
	}

	int version = oval_parser_int_attribute(reader, "version",0);
	int tst_version = oval_test_get_version(ovaltst);
	if(tst_version==0){
		oval_test_set_version(ovaltst, version);
	}else if(tst_version!=version){
		char message[200];*message = '\0';
		sprintf
		(
				message,
				"oval_result_test_parse: @version does not match\n"
				"    test_id = %s",
				test_id
		);
		oval_parser_log_warn(context, message);
	}

	struct oval_string_map *itemmap = oval_string_map_new();
	void *args[] = {sys, test, itemmap};
	return_code = oval_parser_parse_tag
		(reader, context, (oval_xml_tag_parser)_oval_result_test_parse, args);
	test->bindings_initialized = true;
	test->bindings_clearable   = true;

	(*consumer)(test, client);
	if(OVAL_RESULT_TEST_DEBUG){
		oval_parser_log_debug(context, "oval_result_test_parse: END");
	}
	free(test_id);
	return return_code;
}

static xmlNode *_oval_result_binding_to_dom
	(struct oval_variable_binding *binding, xmlDocPtr doc, xmlNode *parent)
{
	char *value = oval_variable_binding_get_value(binding);
	xmlNs *ns_results = xmlSearchNsByHref(doc, parent, OVAL_RESULTS_NAMESPACE);
	xmlNode *binding_node = xmlNewChild(parent, ns_results, BAD_CAST "tested_variable", value);

	struct oval_variable *oval_variable = oval_variable_binding_get_variable(binding);
	char *variable_id = oval_variable_get_id(oval_variable);
	xmlNewProp(binding_node, BAD_CAST "variable_id", variable_id);

	return binding_node;
}

static void _oval_result_test_initialize_bindings(struct oval_result_test *rslt_test)
{
	struct oval_test *oval_test = oval_result_test_get_test(rslt_test);

	struct oval_object *oval_object = oval_test_get_object(oval_test);
	if(oval_object){
		char* object_id = oval_object_get_id(oval_object);
		struct oval_result_system *sys = oval_result_test_get_system(rslt_test);
		struct oval_syschar_model *syschar_model = oval_result_system_get_syschar_model(sys);
		struct oval_syschar *syschar = oval_syschar_model_get_syschar(syschar_model, object_id);
		struct oval_variable_binding_iterator *bindings = oval_syschar_get_variable_bindings(syschar);
		while(oval_variable_binding_iterator_has_more(bindings)){
			struct oval_variable_binding *binding = oval_variable_binding_iterator_next(bindings);
			oval_result_test_add_binding(rslt_test, binding);
		}
		oval_variable_binding_iterator_free(bindings);
	}
	rslt_test->bindings_initialized = true;
	rslt_test->bindings_clearable   = false;//bindings are shared from syschar model.
}

xmlNode *oval_result_test_to_dom
	(struct oval_result_test *rslt_test, xmlDocPtr doc, xmlNode *parent)
{
	xmlNs *ns_results = xmlSearchNsByHref(doc, parent, OVAL_RESULTS_NAMESPACE);
	xmlNode *test_node = xmlNewChild(parent, ns_results, BAD_CAST "test", NULL);

	struct oval_test *oval_test = oval_result_test_get_test(rslt_test);
	char *test_id = oval_test_get_id(oval_test);
	xmlNewProp(test_node, BAD_CAST "test_id", test_id);

	char version[10]; *version = '\0';
	snprintf(version, sizeof(version), "%d", oval_test_get_version(oval_test));
	xmlNewProp(test_node, BAD_CAST "version", BAD_CAST version);

	oval_existence_t existence = oval_test_get_existence(oval_test);
	if(existence!=OVAL_AT_LEAST_ONE_EXISTS){
		xmlNewProp(test_node, BAD_CAST "check_existence", oval_existence_get_text(existence));
	}

	oval_check_t check = oval_test_get_check(oval_test);
	xmlNewProp(test_node, BAD_CAST "check", oval_check_get_text(check));

	int instance_val = oval_result_test_get_instance(rslt_test);
	if(instance_val>1){
		char instance[10]; *instance = '\0';
		snprintf(instance, sizeof(instance), "%d", instance_val);
		xmlNewProp(test_node, BAD_CAST "variable_instance", BAD_CAST instance);
	}

	oval_result_t result = oval_result_test_get_result(rslt_test);
	xmlNewProp(test_node, BAD_CAST "result", oval_result_get_text(result));

	struct oval_result_item_iterator *items = oval_result_test_get_items(rslt_test);
	while(oval_result_item_iterator_has_more(items)){
		struct oval_result_item *item = oval_result_item_iterator_next(items);
		oval_result_item_to_dom(item, doc, test_node);
	}
	oval_result_item_iterator_free(items);

	if(!rslt_test->bindings_initialized){
		_oval_result_test_initialize_bindings(rslt_test);
	}
	struct oval_variable_binding_iterator *bindings = oval_result_test_get_bindings(rslt_test);
	while(oval_variable_binding_iterator_has_more(bindings)){
		struct oval_variable_binding *binding = oval_variable_binding_iterator_next(bindings);
		_oval_result_binding_to_dom(binding, doc, test_node);
	}
	oval_variable_binding_iterator_free(bindings);

	return test_node;
}
