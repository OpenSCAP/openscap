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


class OscapError(Exception):
    ''' oscap Error'''
    pass


OscapResult = collections.namedtuple("OscapResult", ("returncode", "stdout", "stderr"))


class OscapHelpers(object):
    ''' oscap class full of helpers for scanning '''
    CPE_RHEL = 'oval:org.open-scap.cpe.rhel:def:'
    DISTS = ["7", "6", "5"]

    def __init__(self, cve_input_dir="", oscap_binary=False):
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

    #TODO replace by _get_cpe (in order to indentify any containerized system)
    def _get_dist(self, chroot, target):
        '''
        Test the chroot and determine what RHEL dist it is; returns
        an integer representing the dist
        '''
        cpe_dict = '/usr/share/openscap/cpe/openscap-cpe-oval.xml'
        if not os.path.exists(cpe_dict):
            raise OscapError()
        for dist in self.DISTS:
            result = self.oscap_chroot(chroot, target, 'oval', 'eval',
                                       '--id', self.CPE_RHEL + dist, cpe_dict,
                                       '2>&1', '>', '/dev/null')
            if "{0}{1}: true".format(self.CPE_RHEL, dist) in result.stdout:
                return dist


    def _scan_cve(self, chroot, target, dist, scan_args):
        '''
        Scan a chroot for cves
        '''
        cve_input = getInputCVE.dist_cve_name.format(dist)
        tmp_tuple = ('oval', 'eval') + tuple(scan_args) + \
            (os.path.join(self.cve_input_dir, cve_input),)
        return self.oscap_chroot(chroot, target, *tmp_tuple)

    def _scan(self, chroot, target, scan_args):
        '''
        Scan a container or image
        '''
        tmp_tuple = tuple(scan_args)
        return self.oscap_chroot(chroot, target, *tmp_tuple)

    def resolve_image(self, image):
        '''
        Given an image or container name, uuid, or partial, return the
        uuid or iid or False if cannot be identified
        '''
        # TODO
        pass



class OscapDockerScan(object):
    def __init__(self, target, is_image=False, oscap_binary='oscap'):
        
        #init docker low level api (usefull for deep details like container pid)
        try:
            self.client = docker.APIClient()
        except AttributeError:
            self.client = docker.Client()
            
        #init docker high level API (to deal with start/stop/run containers/image)
        self.client_api = docker.from_env()
        self.is_image=is_image
        self.stop_after = False #stop the container after scan if True
        self.oscap_binary = oscap_binary or 'oscap';
        
        print("ok1");
        if self.is_image:
            print("Runing given image in a temporary container ...");
            self._mount_img()
            self.name, self.config=self._get_image_name_and_config(target)
        else:
            self.name, self.config=self._get_container_name_and_config(target)
 
            if int(self.config["State"]["Pid"]) == 0:
                #TODO ask to the user if he want to run temporarily the container
                print("Container {0} is stopped, running it temporarily ..."
                    .format(self.name));
                    
                self.client_api.containers.get(self.name).start();
                self.name, self.config=self._get_container_name_and_config(target)
                if int(self.config["State"]["Pid"]) == 0:
                    sys.stderr.write(
                    "Cannot keep running container {0}, skip it.\n \
                    Consider to change the container entry point to scan it.\n"
                    .format(self.name)
                    )
                    
            self.pid=int(self.config["State"]["Pid"])
        print("ok2");
        if self._check_container_mountpoint():
            print("Docker container {0} ready to be scanned !".format(self.name));            
        else:
            sys.stderr.write(
                "Cannot mount the container {0}, skip it.\n".format(self.name))

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
            return "unknown", {}
            
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
            return "unknown", {}

    def _check_container_mountpoint(self):
        '''
        Ensure that the container fs is well mounted and return its path
        '''
        # TODO
        return True

    def _destroy_container(self, mnt_dir, force=False):
        '''
        Stop and remove the container 
        (use only on temporary container created by the tool)
        set force=True to destroy an existing container
        '''
        if self.image or force:
            #TODO
            return True
        else:
           sys.stderr.write(
            "Ignoring destroy {0} because it's not a temporary container.\n"
            .format(self.target)
            )
           return False


    def scan_cve(self, scan_args):
        '''
        Wrapper function for scanning a container or image
        '''

        # Figure out which RHEL dist is in the chroot
        dist = self.helper._get_dist(self.mountpoint)

        if dist is None:
            sys.stderr.write("{0} is not based on RHEL\n".format(image))
            return None

        # Fetch the CVE input data for the dist
        fetch = getInputCVE(self.mountpoint)
        fetch._fetch_single(dist)

        # Scan the chroot
        scan_result = self.helper._scan_cve(self.mountpoint, dist, scan_args)
        print(scan_result.stdout)
        print(scan_result.stderr, file=sys.stderr)

        return scan_result.returncode

    def scan(self, scan_args):
        '''
        Wrapper function forwarding oscap args for an offline scan
        '''
        scan_result=self.oscap_chroot("/proc/{0}/root".format(self.pid), self.oscap_binary, scan_args)
        print(scan_result.stdout)
        print(scan_result.stderr, file=sys.stderr)

        return scan_result.returncode
        
    def oscap_chroot(self, chroot_path, oscap_binary, oscap_args):
        '''
        Wrapper running oscap_chroot on an OscapDockerScan object
        '''
        os.environ["OSCAP_PROBE_ARCHITECTURE"] = platform.processor()
        os.environ["OSCAP_PROBE_ROOT"] = os.path.join(chroot_path)
        os.environ["OSCAP_PROBE_OS_NAME"] = platform.system()
        os.environ["OSCAP_PROBE_OS_VERSION"] = platform.release()
        
        os.environ["OSCAP_EVALUATION_TARGET"] = self.name
        for var in self.config["Config"].get("Env", []):
            vname, val = var.split("=", 1)
            os.environ["OSCAP_OFFLINE_"+vname] = val
        
        cmd = [oscap_binary] + [x for x in oscap_args]
        print("starting ... {0}".format(cmd));
        oscap_process = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        oscap_stdout, oscap_stderr = oscap_process.communicate()
        return OscapResult(oscap_process.returncode,
                           oscap_stdout.decode("utf-8"), oscap_stderr.decode("utf-8"))
