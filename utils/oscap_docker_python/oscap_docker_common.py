# Copyright (C) 2015 Brent Baude <bbaude@redhat.com>
# Copyright (C) 2019 Dominique Blaze <contact@d0m.tech>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the
# Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.

import subprocess
import platform
import os
import collections


class OscapError(Exception):
    ''' oscap Error'''
    pass

OscapResult = collections.namedtuple("OscapResult", ("returncode", "stdout", "stderr"))


def oscap_chroot(chroot_path, oscap_binary, oscap_args, target_name, local_env=[]):
        '''
        Wrapper running oscap_chroot on an OscapDockerScan OscapAtomicScan object
        '''
        os.environ["OSCAP_PROBE_ROOT"] = os.path.join(chroot_path)
        os.environ["OSCAP_EVALUATION_TARGET"] = target_name

        for var in local_env:
            vname, val = var.split("=", 1)
            os.environ["OSCAP_OFFLINE_" + vname] = val

        cmd = [oscap_binary] + [x for x in oscap_args]

        oscap_process = subprocess.Popen(cmd, stdout=subprocess.PIPE,
                                         stderr=subprocess.PIPE)
        oscap_stdout, oscap_stderr = oscap_process.communicate()
        return OscapResult(oscap_process.returncode,
                           oscap_stdout.decode("utf-8"),
                           oscap_stderr.decode("utf-8"))

# TODO replace by _get_cpe (in order to indentify any containerized system)


def get_dist(mountpoint, oscap_binary, local_env):
    CPE_RHEL = 'oval:org.open-scap.cpe.rhel:def:'
    DISTS = ["8", "7", "6", "5"]

    '''
    Test the chroot and determine what RHEL dist it is; returns
    an integer representing the dist
    '''

    cpe_dict = '/usr/share/openscap/cpe/openscap-cpe-oval.xml'
    if not os.path.exists(cpe_dict):
        # sometime it's installed into /usr/local/share instead of /usr/local
        cpe_dict = '/usr/local/share/openscap/cpe/openscap-cpe-oval.xml'
        if not os.path.exists(cpe_dict):
            raise OscapError()

    for dist in DISTS:
        result = oscap_chroot(
            mountpoint, oscap_binary,
            ("oval", "eval", "--id", CPE_RHEL + dist, cpe_dict,
             mountpoint, "2>&1", ">", "/dev/null"),
            '*',
            local_env
        )

        if "{0}{1}: true".format(CPE_RHEL, dist) in result.stdout:
            print("This system seems based on RHEL{0}.".format(dist))
            return dist
