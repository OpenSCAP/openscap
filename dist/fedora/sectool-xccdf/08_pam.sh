#!/bin/bash

# Copyright (C) 2008 Red Hat, Inc.
# Written by Tomas Heinrich <theinric@redhat.com>
# Adapted for SCE by Martin Preisler <mpreisle@redhat.com>

SYSTEM_AUTH_CFG="/etc/pam.d/system-auth"
PARANOID=0

if [[ ! -f ${SYSTEM_AUTH_CFG} ]]; then
    echo "Configuration file \"${SYSTEM_AUTH_CFG}\" doesn't exist."
    exit $XCCDF_RESULT_FAIL
fi

RET=$XCCDF_RESULT_PASS

sed -n -e ':a; /\\$/N; s/\\\n//; ta; s/^[ \t]*password[^#]*pam_cracklib.so.*//; T; q 1' ${SYSTEM_AUTH_CFG}
if [[ "$?" -eq "0" ]]; then
	echo "New passwords for system services aren't checked with cracklib."
	echo "Add a call to pam_cracklib to '${SYSTEM_AUTH_CFG}'. Please see 'man pam_cracklib' for details."
	RET=$XCCDF_RESULT_INFORMATIONAL
fi

if (( ${PARANOID} == 1 )); then
    sed -n -e ':a; /\\$/N; s/\\\n//; ta; s/^[ \t]*auth[^#]*pam_tally\.so.*//p; T; q 1' ${SYSTEM_AUTH_CFG}
    if [[ "$?" -eq "0" ]]; then
	echo "Account lockout after repetitive failed login attempts is not configured."
	echo "Add a call to pam_tally to '${SYSTEM_AUTH_CFG}'. Please see 'man pam_tally' for details."
	RET=$XCCDF_RESULT_INFORMATIONAL
    fi
fi

exit $RET

