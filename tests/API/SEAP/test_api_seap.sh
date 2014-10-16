#!/usr/bin/env bash

# Copyright 2009 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap SEAP Test Suite.
#
# Created on: Nov 30, 2009
#
# Authors:
#      David Niemoller
#      Ondrej Moris <omoris@redhat.com>

. ../../test_common.sh

# Test cases.

# Parsing incorrect expression.
function test_api_seap_incorrect_expression {
    local ret_val=0;

    ARGS=( 
	"18446744073709551616" \
	"111111111111111111111" \
	"1e" \
	"11e" \
	"111e" \
	"1111e" \
	"11111e" \
	"111111e" \
	"1111111e" \
	"11111111e" \
	"111111111e" \
	"11111111111111111111111111111111111111111e" \
	"-1e" \
	"-11e" \
	"-111e" \
	"-1111e" \
	"-11111e" \
	"-111111e" \
	"-1111111e" \
	"-11111111e" \
	"+1e" \
	"+11e" \
	"+111e" \
	"+1111ee" \
	"+11111e" \
	"+111111e" \
	"+1111111e" \
	"+11111111e" \
	"+111111111e" \
	".12e1.2" \
	"3.-12" \
	"+12.1-1" \
	"0.12.3" \
	"+0.1.23" \
	"-0.12.3" \
	".123." \
	"+.123." \
	"-.123." \
	".0." \
	"1.." \
	"-1.e" \
	"+1.e" \
	"12.4e" \
	"-12.4e" \
	"+12.3e" \
	"1e2.4" \
	"1e-2.4" \
	"1e+2.4" \
	"12e1.4" \
	"12e-1.4" \
	"12e+1.4" \
	"1e12.4" \
	"1e-12.4" \
	"1e+12.4" \
	"1E2.4" \
	"1E-2.4" \
	"1E+2.4" \
	"12E1.4" \
	"12E-1.4" \
	"12E+1.4" \
	"1E12.4" \
	"1E-12.4" \
	"1E+12.4" \
	"123.3e3.4" \
	"-12.3e3.4" \
	"+12.3e3.4" \
	"123.3e-3.4" \
	"123.3e+3.4" \
	"-12.3e-3.4" \
	"+12.3e+3.4" \
	"(a" \
	"(aa" \
	"(aaa" \
	"(aaaa" \
	"(aaaaa" \
	"(aaaaaa" \
	"(aaaaaaa" \
	"(aaaaaaaa" \
	"\"(a b c)" \
	"\"(aa b cc)" \
	"aaa b c)" \
	"aa bb b)" \
	"a 1 2)" \
	"1 2 (a" \
	"'simple" \
	"'\"test\"" \
	"[INT8123" \
	"[UINT8]123)" \
	"[url]http://example.com\"" \
	"[simple string])hello" \
	"|TWFu|)" \
	"([base64]))|TWFu|" \
	"|TW9ua2V5Cg==|))" \
	"|TW9ua2V5Cg|(" \
	"|YmFzZTY0KE1vbmtleSkgPSB8VFc5dWEyVjVDZ3wK|)" \
	"(msg :id 123 (test \"abcd\") 123 [url]\"http://www.example.com\"))" \
	"(msg (:id 123 :hash [md5]|PNeg23b/ncpIl54kw5tAjA==| (test 123 \"asdf\" [wtf]\"dlskflskdf\"))" \
	"(msg :id 123(test 123 \"asdf \"bla\"))" \
	"(msg :id 123(test 123\"asdf\"\"bla))" 
	)

    for I in "${ARGS[@]}"; do
	./test_api_seap_parser "$I"
	ret_val=$[$ret_val+$?]
    done 

    return $([ $ret_val -eq ${#ARGS[@]} ])
}

# Parsing correct expression.
function test_api_seap_correct_expression {
    local ret_val=0;

    ARGS=(
	"0" \
	"1" \
	"11" \
	"111" \
	"1111" \
	"11111" \
	"111111" \
	"1111111" \
	"11111111" \
	"111111111" \
	"1111111111" \
	"99999999999" \
	"18446744073709551615" \
	"9223372036854775807" \
	"9223372036854775808" \
	"-9223372036854775807" \
	"-9223372036854775808" \
	"4294967295" \
	"4294967296" \
	"2147483647" \
	"2147483648" \
	"-2147483647" \
	"-2147483648" \
	"65535" \
	"65536" \
	"131071" \
	"131072" \
	"-131071" \
	"-131072" \
	"255" \
	"256" \
	"127" \
	"128" \
	"-127" \
	"-128" \
	"-1" \
	"-11" \
	"-111" \
	"-1111" \
	"-11111" \
	"-111111" \
	"-1111111" \
	"-11111111" \
	"+1" \
	"+11" \
	"+111" \
	"+1111" \
	"+11111" \
	"+111111" \
	"+1111111" \
	"+11111111" \
	"+111111111" \
	"12" \
	"-12" \
	"+12" \
	"0.123" \
	"+0.123" \
	"-0.123" \
	".123" \
	"+.123" \
	"-.123" \
	".0" \
	"1." \
	"-1." \
	"+1." \
	"12." \
	"-12." \
	"+12." \
	"1e2" \
	"1e-2" \
	"1e+2" \
	"12e1" \
	"12e-1" \
	"12e+1" \
	"1e12" \
	"1e-12" \
	"1e+12" \
	"1E2" \
	"1E-2" \
	"1E+2" \
	"12E1" \
	"12E-1" \
	"12E+1" \
	"1E12" \
	"1E-12" \
	"1E+12" \
	"123.3e3" \
	"-12.3e3" \
	"+12.3e3" \
	"123.3e-3" \
	"123.3e+3" \
	"-12.3e-3" \
	"+12.3e+3" \
	"a" \
	"aa" \
	"aaa" \
	"aaaa" \
	"aaaaa" \
	"aaaaaa" \
	"aaaaaaa" \
	"aaaaaaaa" \
	"\"(a b c)\"" \
	"\"(aa b cc)\"" \
	"aaa b c" \
	"aa bb b" \
	"a 1 2" \
	"1 a 2" \
	"1 2 a" \
	"'simple'" \ 
	"'\"test\"'" \
	"[INT8]123" \
	"[UINT8]123" \
	"[url]\"http://example.com\"" \
	"[simple string]hello" \
	"|TWFu|" \
	"[base64]|TWFu|" \
	"|TW9ua2V5Cg==|" \
	"|TW9ua2V5Cg|" \
	"|YmFzZTY0KE1vbmtleSkgPSB8VFc5dWEyVjVDZ3wK|" \
	"(msg :id 123 (test \"abcd\" 123 [url]\"http://www.example.com\"))" \
	"(msg :id 123 :hash [md5]|PNeg23b/ncpIl54kw5tAjA==| (test 123 \"asdf\" [wtf]\"dlskflskdf\"))" \
	"(msg :id 123(test 123 \"asdf\" \"bla\"))" \
	"(msg :id 123(test 123\"asdf\"\"bla\"))" \
	"(msg)" \
	"(\"\" \"a\" \"aa\" \"aaa\" \"aaaa\" \"aaaaa\")" \
	"(\"\"(\"\")\"\")" \
	"(\"asdf\"(\"asdf\")\"asdf\")" \
	"(m(m)m)" \
	"(a(a a)a)" \
	"(a())" \
	"((((((()))))))" \
	"()()()()()()()" \
	"()()()()()()" \
	"()()()()()" \
	"()()()()" \
	"()()" \
	"()" \
	"()(())((()))()" \
	"(1(2(3(4(5(6(7)))))))" \
	"[a]b" \
	"[a]b[c]d" \
	"[asdf](1 2 3 4)" \
	"[num]([int]1 [int]2 [char]'c' [string]\"asdf\")" \
	"1:a" \
	"3:abc" \
	"4:abc\"" \
	"4:\"abc" \
	"7:a c d e" \
	"1:a1:b1:c" \
	"2:ab2:cd" \
	"(1:a2:bc)" \
	"4|TWFu|" \
	"(4|TWFu|4|TWFu|)" \
	"4[type]4:abcd" \
	"(4[type]3:123)"
    )

    for I in "${ARGS[@]}"; do
	./test_api_seap_parser "$I"
	ret_val=$[$ret_val+$?]
	./test_api_seap_parser "($I)"
	ret_val=$[$ret_val+$?]
	./test_api_seap_parser "( $I)"
	ret_val=$[$ret_val+$?]
	./test_api_seap_parser "($I )"
	ret_val=$[$ret_val+$?]
	./test_api_seap_parser "( $I )"
	ret_val=$[$ret_val+$?]
    done 

    return $ret_val
}

function test_api_seap_split {
    local ret_val=0;

    echo '-- mark00 ---'
    ./test_api_seap_parser '(fs' 'ck)'
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '(f' 's' 'c' 'k' ')'
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '(fs' 'c' 'k)'
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '(f' 's' 'c' 'k)' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '(' 'fs' 'ck)' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '(f' 'sck' ')' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '(fs' 'ck' ')' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '(fs' 'c' 'k' ')' 
    ret_val=$[$ret_val+$?]

    echo '-- mark01 ---' 
    ./test_api_seap_parser '[test]' 'test' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '4[test]' 'test' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '[test]' '4:test' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '4[test]' '4:test' 
    ret_val=$[$ret_val+$?]

    echo '-- mark02 ---' 
    ./test_api_seap_parser '[te' 'st]' 'test' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '4[te' 'st]' 'test' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '[te' 'st]' '4:te' 'st' 
    ret_val=$[$ret_val+$?]

    echo '-- mark03 ---' 
    ./test_api_seap_parser '([test]' 'test)' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '([te' 'st]' 'test)' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '([test]' 'te' 'st)' 
    ret_val=$[$ret_val+$?]

    echo '-- mark04 ---' 
    ./test_api_seap_parser '([' 'test]' 'test)' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '([' 'te' 'st]' 'test)' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '([' 'test]' 'te' 'st)' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '(' '[' 't' 'e' 's' 't' ']' 't' 'e' 's' 't' ')' 
    ret_val=$[$ret_val+$?]

    echo '-- mark05 ---' 
    ./test_api_seap_parser '([test]' '(te' 'st))' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '([test]' '(te' 'st))' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '([test]' '(te' 'st))' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '([test]' '(te' 'st))' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '([test]' '(test' '))' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '([te' 'st]' '(test))' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '([te' 'st]' '(t' 'est))' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '([te' 'st]' '(' 'test))' 
    ret_val=$[$ret_val+$?]

    echo '-- mark06 ---' 
    ./test_api_seap_parser '(testing (sparse (buffer (functionality' ' in) the) S-exp) parser)' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '(testing (sparse (buffer' '(functionality in) the) S-exp) parser)' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '(testing (spa' 'rse (buffer (functionality in) the) S-exp) parser)' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '(testing (sparse (buffer (functionality in) the) S-exp) parser)' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '(tes' 'ting (sparse (buffer (functionality in) the) S-exp) parser)' 
    ret_val=$[$ret_val+$?]

    echo '-- mark07 ---' 
    ./test_api_seap_parser '(testing (sparse (buffer (functionality' ' in) the) S-exp' ') parser)' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '(testing (sparse (buffer' '(functionality in) the) S-exp) parser)' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '(testing (spa' 'rse (buffer (functionality in) the) S-' 'exp) parser)' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '(testing (sparse (buffer (functionality in) the) S' '-exp) parser)' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '(tes' 'ting (sparse (buffer (functionality in) the) ' 'S-exp) parser)' 
    ret_val=$[$ret_val+$?]

    echo '-- mark08 ---' 
    ./test_api_seap_parser '(testing (sparse (buffer (functionality' ' in) the) S-exp' ') ' 'parser)' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '(testing (sparse (buffer' '(functi' 'onality in) the) S-exp) parser)' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '(testing (spa' 'rse (buffer (functionality in) the) S-' 'exp) parser' ')' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '(testing (sparse (' 'buffer (functionality in) the) S' '-exp) parser)' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '(tes' 'ting (sp' 'arse (buffer (' 'functionality in) the) ' 'S-exp) par' 'ser)' 
    ret_val=$[$ret_val+$?]

    echo '-- mark09 ---' 
    ./test_api_seap_parser '(testing (sparse (buffer (functionality' ' in) the) S-exp' ') ' 'parser)' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '(testing (sparse (buffer' '(functi' 'onality in) the) S-exp) pars' 'er)' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '(testing (spa' 'rse (buffer (functionality in) the) S-' 'exp) parser' ')' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '(testing (sparse (' 'buffer (functionality in) the) S' '-exp) p' 'arse' 'r)' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '(tes' 'ting (sp' 'arse (buffer (' 'functionality in) the) ' 'S-exp) par' 'se' 'r' ')' 
    ret_val=$[$ret_val+$?]
    
    echo '-- mark10 ---' 
    ./test_api_seap_parser '(123' '456)' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '(123' '456.34)' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '(123' '.34)' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '(123.' '34)' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '(123.' '34e3)' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '(123.' '34' 'e3)' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '(123.' '34' 'e' '3)' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '(123.' '3' '4' 'e' '3)' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '(123' '.' '3' '4' 'e' '3)' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '(' '1' '2' '3' '.' '3' '4' 'e' '3)' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '(' '-' '1' '2' '3' '.' '3' '4' 'e' '3)' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '(' '+' '1' '2' '3' '.' '3' '4' 'e' '3)' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '(' '+' '1' '2' '3' '.' '3' '4' 'e' '3)' 
    ret_val=$[$ret_val+$?]
    ./test_api_seap_parser '(' '-' '.' '3' '4' 'e' '3)' 
    ret_val=$[$ret_val+$?]

    return $ret_val
}

function test_api_seap_concurency {
    local ret_val=0;

    export SEXP_VALIDATE_DISABLE="1"
    export SEAP_DEBUGLOG_DISABLE="1"
    ./test_api_seap_concurency
    ret_val=$?
    unset SEXP_VALIDATE_DISABLE
    unset SEAP_DEBUGLOG_DISABLE

    return $ret_val
}

function test_api_strto {
    ./test_api_strto
}

# Testing.

test_init "test_api_seap.log"

test_run "test_api_seap_incorrect_expression"   test_api_seap_incorrect_expression
test_run "test_api_seap_correct_expression"     test_api_seap_correct_expression
test_run "test_api_seap_split"                  test_api_seap_split
test_run "test_api_seap_concurency"             test_api_seap_concurency
test_run "test_api_seap_spb"                  ./test_api_seap_spb
test_run "test_api_seap_list"                 ./test_api_seap_list
test_run "test_api_seap_number_expression"    ./test_api_seap_number
test_run "test_api_seap_string_expression"    ./test_api_seap_string
test_run "test_api_SEXP_deepcmp"              ./test_api_SEXP_deepcmp
test_run "test_api_strto"                     ./test_api_strto

test_exit
