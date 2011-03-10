#!/usr/bin/env bash

# Copyright 2009 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap CVSS Module Test Suite.
#
# Authors:
#      Maros Barabas <mbarabas@redhat.com>
#      Ondrej Moris <omoris@redhat.com



. $srcdir/../../test_common.sh

# Test cases.

# Setup.
access_vector=( "AV_LOCAL" "AV_ADJACENT_NETWORK" "AV_NETWORK" )
access_complexity=( "AC_HIGH" "AC_MEDIUM" "AC_LOW" )  
authentication=( "AU_NONE" "AU_SINGLE_INSTANCE" "AU_MULTIPLE_INSTANCE" )
conf_impact=( "CI_NONE"	"CI_PARTIAL" "CI_COMPLETE" )
integ_impact=( "II_NONE" "II_PARTIAL" "II_COMPLETE" )
avail_impact=( "AI_NONE" "AI_PARTIAL" "AI_COMPLETE" )
exploitability=("EX_UNPROVEN" "EX_PROOF_OF_CONCEPT" "EX_FUNCTIONAL" "EX_HIGH" "EX_NOT_DEFINED" )
remediation_level=( "RL_OFFICIAL_FIX" "RL_TEMPORARY_FIX" "RL_WORKAROUND" "RL_UNAVAILABLE" "RL_NOT_DEFINED" )
report_confidence=( "RC_UNCONFIRMED" "RC_UNCORROBORATED" "RC_CONFIRMED" "RC_NOT_DEFINED" )
collateral_damage_potential=( "CD_NONE" "CD_LOW" "CD_LOW_MEDIUM" "CD_MEDIUM_HIGH" "CD_HIGH" "CD_NOT_DEFINED" )
target_distribution=( "TD_NONE" "TD_LOW" "TD_MEDIUM" "TD_HIGH" "TD_NOT_DEFINED" )
conf_req=( "CR_LOW" "CR_MEDIUM" "CR_HIGH" "CR_NOT_DEFINED" )
integ_req=( "IR_LOW" "IR_MEDIUM" "IR_HIGH" "IR_NOT_DEFINED" )
avail_req=( "AR_LOW" "AR_MEDIUM" "AR_HIGH" "AR_NOT_DEFINED" )

# Computes base score for all possible input values and check its correctness.
function test_api_cvss_base_score {
    local ret_val=0

    for A in "${access_vector[@]}"; do
	for B in "${access_complexity[@]}"; do
	    for C in "${authentication[@]}"; do
		for D in "${conf_impact[@]}"; do
		    for E in "${integ_impact[@]}"; do
			for F in "${avail_impact[@]}"; do
  			    ./test_api_cvss --base $A $B $C $D $E $F >/dev/null
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
function test_api_cvss_temporal_score {
    local ret_val=0

    for A in "${exploitability[@]}"; do
	for B in "${remediation_level[@]}"; do
	    for C in "${report_confidence[@]}"; do
		for D in `seq 0.0 1.0 10`; do
		    ./test_api_cvss --temporal $A $B $C "$D" >/dev/null
		    ret_val=$[$ret_val+$?]		
		done
	    done
	done
    done
    
    return $ret_val
}

# Computes environmental score for a few possible input values and check its correctness.
function test_api_cvss_environmental_score {
    local ret_val=0

    ./test_api_cvss --enviromental \
	${collateral_damage_potential[0]} \
	${target_distribution[0]} \
	${conf_req[0]} \
	${integ_req[0]} \
	${avail_req[0]} \
	${access_vector[0]} \
	${access_complexity[0]} \
	${authentication[0]} \
	${conf_impact[0]} \
	${integ_impact[0]} \
	${avail_impact[0]} \
	${exploitability[0]} \
	${remediation_level[0]} \
	${report_confidence[0]} >/dev/null
    
    ret_val=$[$ret_val+$?]		

    ./test_api_cvss --enviromental \
	${collateral_damage_potential[5]} \
	${target_distribution[4]} \
	${conf_req[3]} \
	${integ_req[3]} \
	${avail_req[3]} \
	${access_vector[2]} \
	${access_complexity[2]} \
	${authentication[2]} \
	${conf_impact[2]} \
	${integ_impact[2]} \
	${avail_impact[2]} \
	${exploitability[4]} \
	${remediation_level[4]} \
	${report_confidence[3]} >/dev/null

    ret_val=$[$ret_val+$?]		
    
    return $ret_val
}

# Testing.

test_init "test_api_cvss.log"

test_run "test_api_cvss_base_score" test_api_cvss_base_score  
test_run "test_api_cvss_temporal_score" test_api_cvss_temporal_score  
test_run "test_api_cvss_environmental_score" test_api_cvss_environmental_score  

test_exit 

