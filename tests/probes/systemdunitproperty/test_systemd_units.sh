#!/usr/bin/env bash

. $builddir/tests/test_common.sh

set -e -o pipefail

function test_systemd_units {
    probecheck "systemdunitproperty" || return 255
    probecheck "systemdunitdependency" || return 255
    require_python_module "dbusmock" || return 255
    require "dbus-daemon" || return 255

    # The Python fixture starts a private bus. It never talks to host systemd.
    python3 "${srcdir}/test_systemd_units.py"
}

test_run "Systemd probes enumerate installed units and real instances" test_systemd_units
