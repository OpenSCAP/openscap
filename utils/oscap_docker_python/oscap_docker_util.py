# Copyright (C) 2015 Brent Baude <bbaude@redhat.com>
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

''' Utilities for oscap-docker '''

import os
import tempfile
import subprocess
import platform
import shutil
from oscap_docker_python.get_cve_input import getInputCVE
import sys

try:
    from Atomic.mount import DockerMount
    from Atomic.mount import MountError
    import inspect

    if "mnt_mkdir" not in inspect.getargspec(DockerMount.__init__).args:
        sys.stderr.write(
            "\"Atomic.mount.DockerMount\" has been successfully imported but "
            "it doesn't support the mnt_mkdir argument. Please upgrade your "
            "Atomic installation to 1.4 or higher.\n"
        )
        sys.exit(1)

    # we only care about method names
    member_methods = [
        x[0] for x in
        inspect.getmembers(
            DockerMount, predicate=lambda member: inspect.isfunction(member) or inspect.ismethod(member)
        )
    ]

    if "_clean_temp_container_by_path" not in member_methods:
        sys.stderr.write(
            "\"Atomic.mount.DockerMount\" has been successfully imported but "
            "it doesn't have the _clean_temp_container_by_path method. Please "
            "upgrade your Atomic installation to 1.4 or higher.\n"
        )
        sys.exit(1)

except ImportError:
    sys.stderr.write(
        "Failed to import \"Atomic.mount.DockerMount\". It seems Atomic has "
        "not been installed.\n"
    )
    sys.exit(1)


class OscapError(Exception):
    ''' oscap Error'''
    pass


class OscapHelpers(object):
    ''' oscap class full of helpers for scanning '''
    CPE = 'oval:org.open-scap.cpe.rhel:def:'
    DISTS = ["7", "6", "5"]

    def __init__(self, cve_input_dir):
        self.cve_input_dir = cve_input_dir

    @staticmethod
    def _mk_tmp_dir(tmp_dir):
        '''
        Creates a temporary directory and returns the whole
        path name
        '''
        tempfile.tempdir = tmp_dir
        return tempfile.mkdtemp()

    @staticmethod
    def _rm_tmp_dir(tmp_dir):
        '''
        Deletes the temporary directory created for the purposes
        of mount
        '''
        shutil.rmtree(tmp_dir)

    def _get_dist(self, chroot):
        '''
        Test the chroot and determine what RHEL dist it is; returns
        an integer representing the dist
        '''
        cpe_dict = '/usr/share/openscap/cpe/openscap-cpe-oval.xml'
        if not os.path.exists(cpe_dict):
            raise OscapError()
        for dist in self.DISTS:
            output = self.oscap_chroot('foo', 'bar', chroot, 'oval', 'eval',
                                       '--id', self.CPE + dist, cpe_dict,
                                       '2>&1', '>', '/dev/null')
            if "{0}{1}: true".format(self.CPE, dist) in output:
                return dist

    def oscap_chroot(self, target, image, chroot_path, *oscap_args):
        '''
        Wrapper function for executing oscap in a subprocess
        '''

        os.environ["OSCAP_PROBE_ARCHITECTURE"] = platform.processor()
        os.environ["OSCAP_PROBE_ROOT"] = os.path.join(chroot_path)
        os.environ["OSCAP_PROBE_OS_NAME"] = platform.system()
        os.environ["OSCAP_PROBE_OS_VERSION"] = platform.release()
        os.environ["OSCAP_PROBE_"
                   "PRIMARY_HOST_NAME"] = "{0}-{1}".format(target, image)
        cmd = ['oscap'] + [x for x in oscap_args]
        oscap_process = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        oscap_stdout, oscap_stderr = oscap_process.communicate()
        if oscap_process.returncode not in [0, 2]:
            sys.stderr.write("\nCommand: {0} failed!\n".format(" ".join(cmd)))
            sys.stderr.write("Command returned exit code {0}.\n".format(oscap_process.returncode))
            sys.stderr.write(oscap_stderr.decode("utf-8") + "\n")

            # Clean up
            self._cleanup_by_path(chroot_path)

            sys.exit(1)

        return oscap_stdout.decode("utf-8")

    def _scan_cve(self, chroot, dist, scan_args):
        '''
        Scan a chroot for cves
        '''
        cve_input = getInputCVE.dist_cve_name.format(dist)
        tmp_tuple = ('oval', 'eval') + tuple(scan_args) + \
            (os.path.join(self.cve_input_dir, cve_input),)
        return self.oscap_chroot("foo", "bar", chroot, *tmp_tuple)

    def _scan(self, chroot, scan_args):
        '''
        Scan a container or image
        '''
        tmp_tuple = tuple(scan_args)
        return self.oscap_chroot("foo", "bar", chroot, *tmp_tuple)

    def resolve_image(self, image):
        '''
        Given an image or container name, uuid, or partial, return the
        uuid or iid or False if cannot be identified
        '''
        # TODO
        pass

    def _cleanup_by_path(self, path):
        '''
        Cleans up the mounted chroot by umounting it and
        removing the temporary directory
        '''
        # Sometimes when this def is called, path will have 'rootfs'
        # appended.  If it does, strip it and proceed

        _no_rootfs = os.path.dirname(path) if os.path.basename(path) == \
            'rootfs' else path

        DM = DockerMount("/tmp")
        # umount chroot
        DM.unmount_path(_no_rootfs)

        # clean up temporary container
        DM._clean_temp_container_by_path(_no_rootfs)
        os.rmdir(_no_rootfs)


class OscapScan(object):
    def __init__(self, tmp_dir=tempfile.gettempdir(), mnt_dir=None,
                 hours_old=2):
        self.tmp_dir = tmp_dir
        self.helper = OscapHelpers(tmp_dir)
        self.mnt_dir = mnt_dir
        self.hours_old = hours_old

    def _ensure_mnt_dir(self):
        '''
        Ensure existing temporary directory
        '''
        if self.mnt_dir is None:
            return tempfile.mkdtemp()
        else:
            return self.mnt_dir

    def _remove_mnt_dir(self, mnt_dir):
        '''
        Remove temporary directory, but only if the directory was not
        passed through __init__
        '''
        if self.mnt_dir is None:
            os.rmdir(mnt_dir)

    def scan_cve(self, image, scan_args):
        '''
        Wrapper function for scanning a container or image
        '''

        mnt_dir = self._ensure_mnt_dir()

        # Mount the temporary image/container to the dir
        DM = DockerMount(mnt_dir, mnt_mkdir=True)
        try:
            _tmp_mnt_dir = DM.mount(image)
        except MountError as e:
            sys.stderr.write(str(e) + "\n")
            return None

        # Remeber actual mounted fs in 'rootfs'
        chroot = os.path.join(_tmp_mnt_dir, 'rootfs')


        try:
            # Figure out which RHEL dist is in the chroot
            dist = self.helper._get_dist(chroot)

            if dist is None:
                sys.stderr.write("{0} is not based on RHEL\n".format(image))
                return None

            # Fetch the CVE input data for the dist
            fetch = getInputCVE(self.tmp_dir)
            fetch._fetch_single(dist)

            # Scan the chroot
            sys.stdout.write(self.helper._scan_cve(chroot, dist, scan_args))

        finally:
            # Clean up
            self.helper._cleanup_by_path(_tmp_mnt_dir)
            self._remove_mnt_dir(mnt_dir)

    def scan(self, image, scan_args):
        '''
        Wrapper function for basic security scans using
        openscap
        '''

        mnt_dir = self._ensure_mnt_dir()

        # Mount the temporary image/container to the dir
        DM = DockerMount(mnt_dir, mnt_mkdir=True)
        try:
            _tmp_mnt_dir = DM.mount(image)
        except MountError as e:
            sys.stderr.write(str(e) + "\n")
            return None

        # Remeber actual mounted fs in 'rootfs'
        chroot = os.path.join(_tmp_mnt_dir, 'rootfs')

        # Scan the chroot
        sys.stdout.write(self.helper._scan(chroot, scan_args))

        # Clean up
        self.helper._cleanup_by_path(_tmp_mnt_dir)
        self._remove_mnt_dir(mnt_dir)
