#!/usr/bin/env bash

# Copyright 2008 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap CVSS Module Test Suite.
#
# Authors:
#      Maros Barabas <mbarabas@redhat.com>
#      Ondrej Moris <omoris@redhat.com


. ${srcdir}/test_common.sh

# Setup.
function test_cvss_setup {
    local ret_val=0

    access_vector=(
	"AV_LOCAL"            \
	"AV_ADJACENT_NETWORK" \
	"AV_NETWORK"          )

    access_complexity=(
	"AC_HIGH"   \
	"AC_MEDIUM" \
	"AC_LOW"    )
    
    authentication=(
	"AU_NONE"              \
	"AU_SINGLE_INSTANCE"   \
	"AU_MULTIPLE_INSTANCE" )

    conf_impact=(
	"CI_NONE"     \
	"CI_PARTIAL"  \
	"CI_COMPLETE" )

    integ_impact=(
	"II_NONE"     \
	"II_PARTIAL"  \
	"II_COMPLETE" )

    avail_impact=(
	"AI_NONE"     \
	"AI_PARTIAL"  \
	"AI_COMPLETE" )

    exploitability=(
	"EX_UNPROVEN"         \
	"EX_PROOF_OF_CONCEPT" \
	"EX_FUNCTIONAL"       \
	"EX_HIGH"             \
	"EX_NOT_DEFINED"      )

    remediation_level=(
	"RL_OFFICIAL_FIX"  \
	"RL_TEMPORARY_FIX"   \
	"RL_WORKAROUND"    \
	"RL_UNAVAILABLE"   \
	"RL_NOT_DEFINED"   )

    report_confidence=(
	"RC_UNCONFIRMED"    \
	"RC_UNCORROBORATED" \
	"RC_CONFIRMED"      \
	"RC_NOT_DEFINED"    )

    collateral_damage_potential=(
	"CD_NONE"        \
	"CD_LOW"         \
	"CD_LOW_MEDIUM"  \
	"CD_MEDIUM_HIGH" \
	"CD_HIGH"        \
	"CD_NOT_DEFINED" )

    target_distribution=(
	"TD_NONE"        \
	"TD_LOW"         \
	"TD_MEDIUM"      \
	"TD_HIGH"        \
	"TD_NOT_DEFINED" )

    conf_req=(
	"CR_LOW"         \
	"CR_MEDIUM"      \
	"CR_HIGH"        \
	"CR_NOT_DEFINED" )

    integ_req=(
	"IR_LOW"         \
	"IR_MEDIUM"      \
	"IR_HIGH"        \
	"IR_NOT_DEFINED" )

    avail_req=(
	"AR_LOW"         \
	"AR_MEDIUM"      \
	"AR_HIGH"        \
	"AR_NOT_DEFINED" )
    
    return $ret_val
}

# Test Cases.

# Computes base score for all possible input values and check its correctness.
function test_cvss_base_score {
    local ret_val=0

    for A in "${access_vector[@]}"; do
	for B in "${access_complexity[@]}"; do
	    for C in "${authentication[@]}"; do
		for D in "${conf_impact[@]}"; do
		    for E in "${integ_impact[@]}"; do
			for F in "${avail_impact[@]}"; do
  			    ./test_cvss --base $A $B $C $D $E $F >/dev/null
			    ret_val=$[$ret_val+$?]
			done
		    done
		done
	    done
	done
    done

    return $ret_val
}

# Computes temporal score for possible input values and check its correctness.
function test_cvss_temporal_score {
    local ret_val=0

    for A in "${exploitability[@]}"; do
	for B in "${remediation_level[@]}"; do
	    for C in "${report_confidence[@]}"; do
		for D in `seq 0.0 0.1 10`; do
		    ./test_cvss --temporal $A $B $C "$D" >/dev/null
		    ret_val=$[$ret_val+$?]		
		done
	    done
	done
    done
    
    return $ret_val
}

# Computes environmental score for possible input values and check its correctness.
function test_cvss_environmental_score {
    local ret_val=0

    for A in ${collateral_damage_potential[0]} ${collateral_damage_potential[5]}; do
	for B in ${target_distribution[0]} ${target_distribution[4]}; do
	    for C in ${conf_req[0]} ${conf_req[3]}; do
		for D in ${integ_req[0]} ${integ_req[3]}; do
		    for E in ${avail_req[0]} ${avail_req[3]}; do
			for F in ${access_vector[0]} ${access_vector[2]}; do
			    for G in ${access_complexity[0]} ${access_complexity[2]}; do
				for H in ${authentication[0]} ${authentication[2]}; do
				    for I in ${conf_impact[0]} ${conf_impact[2]}; do
					for J in ${integ_impact[0]} ${integ_impact[2]}; do
					    for K in ${avail_impact[0]} ${avail_impact[2]}; do
						for L in ${exploitability[0]} ${exploitability[4]}; do
						    for M in ${remediation_level[0]} ${remediation_level[4]}; do
							for N in ${report_confidence[0]} ${report_confidence[3]}; do
							    ./test_cvss --enviromental $A $B $C $D $E $F $G $H $I $J $K $L $M $N >/dev/null
							    ret_val=$[$ret_val+$?]		
							done
						    done
						done
					    done
					done
				    done
				done
			    done
			done
		    done
		done
	    done
	done
    done
    
    return $ret_val
}

# Cleanup.
function test_cvss_cleanup {
    local ret_val=0
    return $ret_val
}

# TESTING.

echo ""
echo "--------------------------------------------------"

result=0
log=test_cvss.log

exec 2>$log

test_cvss_setup
ret_val=$? 
report_result "test_cvss_setup" $ret_val
result=$[$result+$ret_val]

test_cvss_base_score  
ret_val=$? 
report_result "test_cvss_base_score" $ret_val  
result=$[$result+$ret_val]   

test_cvss_temporal_score
ret_val=$? 
report_result "test_cvss_temporal_score" $ret_val  
result=$[$result+$ret_val]   

test_cvss_environmental_score
ret_val=$? 
report_result "test_cvss_environmental_score" $ret_val  
result=$[$result+$ret_val]   

test_cvss_cleanup 
ret_val=$? 
report_result "test_cvss_cleanup" $ret_val  
result=$[$result+$ret_val]

echo "--------------------------------------------------"
echo "See ${log} (in tests dir)"

exit $result
