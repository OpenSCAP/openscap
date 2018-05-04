#!/bin/bash

. "$builddir/tests/test_common.sh"

function test_eval_no_verify {

	$OSCAP xccdf eval "${srcdir}/$1"
}

function test_verify {
	# just for reference and not used for now
	xmlsec1 verify --pubkey-pem "$2" "$1"
}

test_init test_signed.log

test_run "signed-sds-no-verification" test_eval_no_verify sds-signed.xml
test_run "signed-sds-fake-x509-no-verification" test_eval_no_verify sds-signed-fake-x509.xml

#if [ -z ${CUSTOM_OSCAP+x} ] ; then
    # test_run "signed-sds-verify" test_verify sds-signed.xml sds-signed-key.pub  
#fi

test_exit
