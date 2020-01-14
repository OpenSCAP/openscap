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

from __future__ import print_function

import os
import tempfile
import shutil
from oscap_docker_python.get_cve_input import getInputCVE
import sys
import docker
import uuid
import collections
from oscap_docker_python.oscap_docker_common import oscap_chroot, get_dist, \
    OscapResult, OscapError


class OscapError(Exception):
    ''' oscap Error'''
    pass


OscapResult = collections.namedtuple("OscapResult", ("returncode", "stdout", "stderr"))


class OscapDockerScan(object):

    def __init__(self, target, is_image=False, oscap_binary='oscap'):

        # init docker low level api (usefull for deep details like container pid)
        try:
            self.client = docker.APIClient()
        except AttributeError:
            self.client = docker.Client()

        # init docker high level API (to deal with start/stop/run containers/image)
        self.client_api = docker.from_env()
        self.is_image = is_image
        self.stop_at_end = False  # stop the container after scan if True
        self.oscap_binary = oscap_binary or 'oscap'
        self.container_name = None
        self.image_name = None

        if self.is_image:
            self.image_name, self.config = self._get_image_name_and_config(target)
            if self.image_name:
                print("Runing given image in a temporary container ...")
                self.container_name = "tmp_oscap_" + str(uuid.uuid1())

                try:
                    tmp_cont = self.client.create_container(
                        self.image_name, 'bash', name=self.container_name, tty=True)
                    # tty=True is required in order to keep the container running
                    self.client.start(container=tmp_cont.get('Id'))

                    self.config = self.client.inspect_container(self.container_name)
                    if int(self.config["State"]["Pid"]) == 0:
                        sys.stderr.write("Cannot run image {0}.\n".format(self.image_name))
                    else:
                        self.pid = int(self.config["State"]["Pid"])
                except Exception as e:
                    sys.stderr.write("Cannot run image {0}.\n".format(self.image_name))
                    raise e
            else:
                raise ValueError("Image {0} not found.\n".format(target))

        else:
            self.container_name, self.config = \
                self._get_container_name_and_config(target)

            # is the container running ?
            if int(self.config["State"]["Pid"]) == 0:
                print("Container {0} is stopped, running it temporarily ..."
                      .format(self.container_name))

                self.client_api.containers.get(self.container_name).start()
                self.container_name, self.config = \
                    self._get_container_name_and_config(target)

                if int(self.config["State"]["Pid"]) == 0:
                    sys.stderr.write(
                        "Cannot keep running container {0}, skip it.\n \
                        Please start this container before scan it.\n"
                        .format(self.container_name))
                else:
                    self.stop_at_end = True

            # now we are sure that the container is running, get its PID
            self.pid = int(self.config["State"]["Pid"])

        if self._check_container_mountpoint():
            self.mountpoint = "/proc/{0}/root".format(self.pid)
            print("Docker container {0} ready to be scanned."
                  .format(self.container_name))
        else:
            self._end()
            raise RuntimeError(
                "Cannot access mountpoint of container {0}, "
                "please RUN WITH ROOT privileges.\n"
                .format(self.container_name))

    def _end(self):
        if self.is_image:
            # stop and remove the temporary container
            self.client.stop(self.container_name)
            self.client.remove_container(self.container_name)
            print("Temporary container {0} cleaned".format(self.container_name))
        else:
            if self.stop_at_end:
                # just stop the container if the tool have started it.
                self.client.stop(self.container_name)

    def _get_image_name_and_config(self, target):
        '''
        Ensure that target is an image.
        Returns full image name if exists or image ID otherwise.
        For containers returns
        container name if exists or container ID otherwise.
        '''

        try:
            image = self.client.inspect_image(target)
            if image["RepoTags"]:
                name = ", ".join(image["RepoTags"])
            else:
                name = image["Id"][len("sha256:"):][:10]
            return name, image
        except docker.errors.NotFound:
            return None, {}

    def _get_container_name_and_config(self, target):
        '''
        Ensure that target is a container.
        Returns container name if exists or container ID otherwise.
        '''
        try:
            container = self.client.inspect_container(target)
            if container["Name"]:
                name = container["Name"].lstrip("/")
            else:
                name = container["Id"][:10]
            return name, container
        except docker.errors.NotFound:
            return None, {}

    def _check_container_mountpoint(self):
        '''
        Ensure that the container fs is well mounted and return its path
        '''
        return os.access("/proc/{0}/root".format(self.pid), os.R_OK)

    def scan_cve(self, scan_args):
        '''
        Wrapper function for scanning cve of a mounted container
        '''

        tmp_dir = tempfile.mkdtemp()

        # Figure out which RHEL dist is in the chroot
        dist = get_dist(self.mountpoint, self.oscap_binary,
                        self.config["Config"].get("Env", []) or [])

        if dist is None:
            sys.stderr.write("{0} is not based on RHEL\n"
                             .format(self.image_name or self.container_name))
            return None

        # Fetch the CVE input data for the dist
        fetch = getInputCVE(tmp_dir)
        cve_file = fetch._fetch_single(dist)

        print("CVEs downloaded in " + cve_file)

        args = ("oval", "eval")
        for a in scan_args:
            args += (a,)
        args += (cve_file,)

        scan_result = oscap_chroot(
            self.mountpoint, self.oscap_binary, args,
            self.image_name or self.container_name,
            self.config["Config"].get("Env", []) or []  # because Env can exists but be None
        )

        print(scan_result.stdout)
        print(scan_result.stderr, file=sys.stderr)

        # cleanup

        print("Cleaning temporary files ...")
        shutil.rmtree(tmp_dir)
        self._end()

        return scan_result.returncode

    def scan(self, scan_args):
        '''
        Wrapper function forwarding oscap args for an offline scan
        '''
        scan_result = oscap_chroot(
            "/proc/{0}/root".format(self.pid),
            self.oscap_binary, scan_args,
            self.image_name or self.container_name,
            self.config["Config"].get("Env", []) or []  # because Env can exists but be None
        )

        print(scan_result.stdout)
        print(scan_result.stderr, file=sys.stderr)

        self._end()

        return scan_result.returncode
