#!/usr/bin/env bash

# Copyright 2015 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap Probes Test Suite.

RPMBASE="${builddir}/tests/probes/rpm"
RPMTEST="${RPMBASE}/root"
RPMBUILD="${RPMBASE}/build"

# Since Fedora 36 RPM database location changed, see
# https://fedoraproject.org/wiki/Changes/RelocateRPMToUsr
RPMDB_PATH="/usr/lib/sysimage/rpm/"

function rpm_build {
    require "rpmbuild" || return 255
    if [ ! -f ${RPMBUILD}/RPMS/noarch/foo-1.0-1.noarch.rpm ]; then
        rpmbuild --define "_buildhost testhost" --define "_tmppath ${RPMTEST}/tmp" --define "_topdir ${RPMBUILD}" -ba ${srcdir}/../foo.spec
    fi
    if [ ! -f ${RPMBUILD}/RPMS/noarch/foobar-1.0-1.noarch.rpm ]; then
        rpmbuild --define "_buildhost testhost" --define "_tmppath ${RPMTEST}/tmp" --define "_topdir ${RPMBUILD}" -ba ${srcdir}/../foobar.spec
    fi
}

function rpm_prepare_offline {
    set_chroot_offline_test_mode "$RPMTEST"
    require "rpm" || return 255
    rm -rf ${RPMTEST}
    mkdir -p ${RPMTEST}/usr/lib/rpm
    cp /usr/lib/rpm/rpmrc ${RPMTEST}/usr/lib/rpm/rpmrc
    cp /usr/lib/rpm/macros ${RPMTEST}/usr/lib/rpm/macros
    rpm_build
    rpm -i ${RPMBUILD}/RPMS/noarch/foobar-1.0-1.noarch.rpm --badreloc --relocate="/etc=${RPMTEST}/etc/" --dbpath="${RPMTEST}${RPMDB_PATH}"
    rpm -i ${RPMBUILD}/RPMS/noarch/foo-1.0-1.noarch.rpm --badreloc --relocate="/etc=${RPMTEST}/etc/" --dbpath="${RPMTEST}${RPMDB_PATH}"
}

function rpm_cleanup_offline {
    rm -rf ${RPMTEST}
    unset_chroot_offline_test_mode
}

function rpm_query {
    require "rpm" || return 255
    if [ -d "$OSCAP_PROBE_ROOT" ]; then
        DB="--dbpath=${OSCAP_PROBE_ROOT}${RPMDB_PATH}"
    fi
    rpm $DB --qf "%{$2}\n" -q $1 | head -
}
