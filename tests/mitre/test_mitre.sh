#!/usr/bin/env bash

# Author:
#      Peter Vrabec, <pvrabec@redhat.com>


. ${srcdir}/../test_common.sh

# Test Cases.

function test_mitre {

    if [ ! -d /tmp/ValidationSupportFiles ]; then
        eval "which unzip > /dev/null 2>&1"    
        if [ ! $? -eq 0 ]; then	
            echo -e "No unzip found in $PATH!\n" 
	    return 255; # Test is not applicable.
        fi
        /usr/bin/unzip -u ValidationSupportFiles.zip -d /tmp
    fi

    local ret_val=0;
    local DEFFILE=${srcdir}/$1
    local RESFILE="$DEFFILE".results

    [ -f $RESFILE ] && rm -f $RESFILE
    ../../utils/.libs/oscap oval eval --results $RESFILE $DEFFILE
    ret_val=$?
    
    if [ $ret_val -eq 0 ]; then
        return 0;
    elif [ $ret_val -eq 2 ]; then
	return 1;
    elif [ $ret_val -eq 1 ]; then
        return 255;
    else
	return "$ret_val"
    fi
}

# Testing.
test_init "test_mitre.log"

# INDEPENDET
test_run "ind-def_unknown_test.xml" test_mitre ind-def_unknown_test.xml
test_run "ind-def_variable_test.xml" test_mitre ind-def_variable_test.xml
test_run "ind-def_environmentvariable_test.xml" test_mitre ind-def_environmentvariable_test.xml

test_run "ind-def_family_test.xml" test_mitre ind-def_family_test.xml

# [E:oval_test.c:368:oval_test_parse_tag] Unknown test oval:org.mitre.oval.test:tst:305.
# [E:oval_defModel.c:262:oval_definition_model_import] Failed to merge the definition model from: ../ind-def_filehash58_test.xml.
#test_run "ind-def_filehash58_test.xml" test_mitre ind-def_filehash58_test.xml

#  their bug in: oval:org.mitre.oval.test:tst:102
#  unsupport recursion up: oval:org.mitre.oval.test:obj:562
#< filehash_test >
#
#Deprecated As Of Version: 5.8
#Reason: Replaced by the filehash58_test.
#Comment: This object has been deprecated and may be removed in a future version of the language.
#test_run "ind-def_filehash_test.xml" test_mitre ind-def_filehash_test.xml

test_run "ind-def_ldap_test.xml" test_mitre ind-def_ldap_test.xml
test_run "ind-def_sql_test.xml" test_mitre ind-def_sql_test.xml

#  unsupported recursion up: oval:org.mitre.oval.test:obj:577
#test_run "ind-def_textfilecontent54_test.xml" test_mitre ind-def_textfilecontent54_test.xml
#  unsupported recursion up: oval:org.mitre.oval.test:obj:800
#test_run "ind-def_textfilecontent_test.xml" test_mitre ind-def_textfilecontent_test.xml

test_run "ind-def_unknown_test.xml" test_mitre ind-def_unknown_test.xml
test_run "ind-def_variable_test.xml" test_mitre ind-def_variable_test.xml

#  unsupported recursion up: oval:org.mitre.oval.test:obj:701
#test_run "ind-def_xmlfilecontent_test.xml" test_mitre ind-def_xmlfilecontent_test.xml

# LINUX
test_run "linux-def_dpkginfo_test.xml" test_mitre linux-def_dpkginfo_test.xml
#  do not pass content validation
#test_run "linux-def_inetlisteningservers_test.xml" test_mitre linux-def_inetlisteningservers_test.xml

# failed
#test_run "linux-def_partition_test.xml" test_mitre linux-def_partition_test.xml
test_run "linux-def_rpminfo_test.xml" test_mitre linux-def_rpminfo_test.xml

test_run "oval_binary_datatype.xml" test_mitre oval_binary_datatype.xml
test_run "linux-def_rpmverify_test.xml" test_mitre linux-def_rpmverify_test.xml
test_run "linux-def_selinuxboolean_test.xml" test_mitre linux-def_selinuxboolean_test.xml
test_run "linux-def_slackwarepkginfo_test.xml" test_mitre linux-def_slackwarepkginfo_test.xml
test_run "oval_binary_datatype.xml" test_mitre oval_binary_datatype.xml
test_run "oval_boolean_datatype.xml" test_mitre oval_boolean_datatype.xml
# failed
#test_run "oval_check_enumeration_entity.xml" test_mitre oval_check_enumeration_entity.xml
test_run "oval_check_enumeration_object_state.xml" test_mitre oval_check_enumeration_object_state.xml
test_run "oval-def_arithmetic_function.xml" test_mitre oval-def_arithmetic_function.xml
test_run "oval-def_begin_function.xml" test_mitre oval-def_begin_function.xml
test_run "oval-def_concat_function.xml" test_mitre oval-def_concat_function.xml
test_run "oval-def_constant_variable.xml" test_mitre oval-def_constant_variable.xml
test_run "oval-def_criteria.xml" test_mitre oval-def_criteria.xml
test_run "oval-def_criterion.xml" test_mitre oval-def_criterion.xml
# failed
#test_run "oval-def_end_function.xml" test_mitre oval-def_end_function.xml
test_run "oval-def_escape_regex_function.xml" test_mitre oval-def_escape_regex_function.xml
# failed
#test_run "oval-def_extend_definition.xml" test_mitre oval-def_extend_definition.xml
test_run "oval-def_external_variable.xml" test_mitre oval-def_external_variable.xml
test_run "oval-def_literal_component.xml" test_mitre oval-def_literal_component.xml
test_run "oval-def_local_variable.xml" test_mitre oval-def_local_variable.xml
test_run "oval-def_object_component.xml" test_mitre oval-def_object_component.xml
test_run "oval-def_regex_capture_function.xml" test_mitre oval-def_regex_capture_function.xml
# failed
test_run "oval-def_set.xml" test_mitre oval-def_set.xml

# failed
#test_run "oval-def_split_function.xml" test_mitre oval-def_split_function.xml
test_run "oval-def_substring_function.xml" test_mitre oval-def_substring_function.xml
# failed
#test_run "oval-def_time_difference_function.xml" test_mitre oval-def_time_difference_function.xml
test_run "oval-def_variable_component.xml" test_mitre oval-def_variable_component.xml
test_run "oval_evr_string_datatype.xml" test_mitre oval_evr_string_datatype.xml
test_run "oval_existence_enumeration.xml" test_mitre oval_existence_enumeration.xml
test_run "oval_float_datatype.xml" test_mitre oval_float_datatype.xml
test_run "oval_int_datatype.xml" test_mitre oval_int_datatype.xml
test_run "oval_string_datatype.xml" test_mitre oval_string_datatype.xml
test_run "oval_version_datatype.xml" test_mitre oval_version_datatype.xml
# failed
#test_run "unix-def_file_test.xml" test_mitre unix-def_file_test.xml
test_run "unix-def_inetd_test.xml" test_mitre unix-def_inetd_test.xml
# failed
#test_run "unix-def_interface_test.xml" test_mitre unix-def_interface_test.xml
test_run "unix-def_password_test.xml" test_mitre unix-def_password_test.xml
test_run "unix-def_process58_test.xml" test_mitre unix-def_process58_test.xml
test_run "unix-def_process_test.xml" test_mitre unix-def_process_test.xml
test_run "unix-def_runlevel_test.xml" test_mitre unix-def_runlevel_test.xml
# skipped
#test_run "unix-def_shadow_test.xml" test_mitre unix-def_shadow_test.xml
test_run "unix-def_uname_test.xml" test_mitre unix-def_uname_test.xml
test_run "unix-def_xinetd_test.xml" test_mitre unix-def_xinetd_test.xml

test_exit
