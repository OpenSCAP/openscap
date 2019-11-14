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

''' Utilities for oscap-docker '''

from __future__ import print_function

import os
import tempfile
import subprocess
import platform
import shutil
from oscap_docker_python.get_cve_input import getInputCVE
import sys
import docker
import collections
from oscap_docker_python.oscap_docker_util_noatomic import OscapDockerScan
from oscap_docker_python.oscap_docker_common import oscap_chroot, get_dist, \
    OscapResult, OscapError

atomic_loaded = False


class AtomicError(Exception):
    """Exception raised when an error happens in atomic import
    """
    def __init__(self, message):
        self.message = message


try:
    from Atomic.mount import DockerMount
    from Atomic.mount import MountError
    import inspect

    if "mnt_mkdir" not in inspect.getargspec(DockerMount.__init__).args:
        raise AtomicError(
            "\"Atomic.mount.DockerMount\" has been successfully imported but "
            "it doesn't support the mnt_mkdir argument. Please upgrade your "
            "Atomic installation to 1.4 or higher.\n"
        )

    # we only care about method names
    member_methods = [
        x[0] for x in
        inspect.getmembers(
            DockerMount, predicate=lambda member:
                inspect.isfunction(member) or inspect.ismethod(member)
        )
    ]

    if "_clean_temp_container_by_path" not in member_methods:
        raise AtomicError(
            "\"Atomic.mount.DockerMount\" has been successfully imported but "
            "it doesn't have the _clean_temp_container_by_path method. Please "
            "upgrade your Atomic installation to 1.4 or higher.\n"
        )

    # if all imports are ok we can use atomic
    atomic_loaded = True

except ImportError:
    sys.stderr.write(
        "Failed to import \"Atomic.mount.DockerMount\". It seems Atomic has "
        "not been installed.\n"
    )

except AtomicError as err:
    sys.stderr.write(err.message)


def isAtomicLoaded():
    return atomic_loaded


class OscapHelpers(object):
    ''' oscap class full of helpers for scanning '''
    CPE = 'oval:org.open-scap.cpe.rhel:def:'
    DISTS = ["8", "7", "6", "5"]

    def __init__(self, cve_input_dir, oscap_binary):
        self.cve_input_dir = cve_input_dir
        self.oscap_binary = oscap_binary or 'oscap'

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

    def _get_target_name_and_config(self, target):
        '''
        Determines if target is image or container. For images returns full
        image name if exists or image ID otherwise. For containers returns
        container name if exists or container ID otherwise.
        '''
        try:
            client = docker.APIClient()
        except AttributeError:
            client = docker.Client()
        try:
            image = client.inspect_image(target)
            if image["RepoTags"]:
                name = ", ".join(image["RepoTags"])
            else:
                name = image["Id"][len("sha256:"):][:10]
            return "docker-image://{}".format(name), image["Config"]
        except docker.errors.NotFound:
            try:
                container = client.inspect_container(target)
                if container["Name"]:
                    name = container["Name"].lstrip("/")
                else:
                    name = container["Id"][:10]
                return "docker-container://{}".format(name), container["Config"]
            except docker.errors.NotFound:
                return "unknown", {}

    def _scan_cve(self, chroot, target, dist, scan_args):
        '''
        Scan a chroot for cves
        '''
        cve_input = getInputCVE.dist_cve_name.format(dist)

        args = ("oval", "eval")
        for a in scan_args:
            args += (a,)
        args += (os.path.join(self.cve_input_dir, cve_input),)

        name, conf = self._get_target_name_and_config(target)

        return oscap_chroot(chroot, self.oscap_binary, args, name,
                            conf.get("Env", []) or [])

    def _scan(self, chroot, target, scan_args):
        '''
        Scan a container or image
        '''

        name, conf = self._get_target_name_and_config(target)
        return oscap_chroot(chroot, target, scan_args, name,
                            conf.get("Env", []) or [])

    def resolve_image(self, image):
        '''
        Given an image or container name, uuid, or partial, return the
        uuid or iid or False if cannot be identified
        '''
        # TODO
        pass

    def _cleanup_by_path(self, path, DM):
        '''
        Cleans up the mounted chroot by umounting it and
        removing the temporary directory
        '''
        # Sometimes when this def is called, path will have 'rootfs'
        # appended.  If it does, strip it and proceed
        _no_rootfs = path
        if os.path.basename(path) == 'rootfs':
            _no_rootfs = os.path.dirname(path)

        # umount chroot
        DM.unmount_path(_no_rootfs)

        # clean up temporary container
        DM._clean_temp_container_by_path(_no_rootfs)
        os.rmdir(_no_rootfs)


def mount_image_filesystem():
    _tmp_mnt_dir = DM.mount(image)


class OscapAtomicScan(object):
    def __init__(self, tmp_dir=tempfile.gettempdir(), mnt_dir=None,
                 hours_old=2, oscap_binary=''):
        self.tmp_dir = tmp_dir
        self.helper = OscapHelpers(tmp_dir, oscap_binary)
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

    def _find_chroot_path(self, mnt_dir):
        '''
        Remember actual mounted fs in 'rootfs' for devicemapper
        '''
        rootfs_path = os.path.join(mnt_dir, 'rootfs')
        if os.path.exists(rootfs_path):
            chroot = rootfs_path
        else:
            chroot = mnt_dir
        return chroot

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

        try:
            chroot = self._find_chroot_path(_tmp_mnt_dir)

            # Figure out which RHEL dist is in the chroot
            name, conf = self.helper._get_target_name_and_config(image)
            dist = get_dist(chroot, self.helper.oscap_binary, conf.get("Env", []) or [])

            if dist is None:
                sys.stderr.write("{0} is not based on RHEL\n".format(image))
                return None

            # Fetch the CVE input data for the dist
            fetch = getInputCVE(self.tmp_dir)
            fetch._fetch_single(dist)

            # Scan the chroot
            scan_result = self.helper._scan_cve(chroot, image, dist, scan_args)
            print(scan_result.stdout)
            print(scan_result.stderr, file=sys.stderr)

        finally:
            # Clean up
            self.helper._cleanup_by_path(_tmp_mnt_dir, DM)
            self._remove_mnt_dir(mnt_dir)

        return scan_result.returncode

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

        try:
            chroot = self._find_chroot_path(_tmp_mnt_dir)

            # Scan the chroot
            scan_result = self.helper._scan(chroot, image, scan_args)
            print(scan_result.stdout)
            print(scan_result.stderr, file=sys.stderr)

        finally:
            # Clean up
            self.helper._cleanup_by_path(_tmp_mnt_dir, DM)
            self._remove_mnt_dir(mnt_dir)

        return scan_result.returncode
