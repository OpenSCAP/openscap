''' Utilities for oscap-docker '''
import os
import tempfile
import subprocess
import platform
from Atomic.mount import DockerMount
import shutil
from oscap_docker_python.get_cve_input import getInputCVE
import sys


class oscapError(Exception):
    ''' oscap Error'''
    pass


class oscapHelpers:
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
            raise oscapError
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
        try:
            run = subprocess.check_output(cmd)
        except Exception as error:
            print "\nCommand: {0} failed!\n".format(" ".join(cmd))
            print "Error was:\n"
            print error

            # Clean up
            self._cleanup_by_path(chroot_path)

            sys.exit(1)

        return run

    def _scan_cve(self, chroot, dist, scan_args):
        '''
        Scan a chroot for cves
        '''
        cve_input = "Red_Hat_Enterprise_Linux_{0}.xml".format(dist)
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


class oscapScan(oscapHelpers):

    def __init__(self, tmp_dir=tempfile.gettempdir(), mnt_dir=os.getcwd(),
                 hours_old=2):
        self.tmp_dir = tmp_dir
        self.helper = oscapHelpers(tmp_dir)
        self.mnt_dir = mnt_dir
        self.hours_old = hours_old

    def scan_cve(self, image, scan_args):
        '''
        Wrapper function for scanning a container or image
        '''
        # Mount the temporary image/container to the dir
        DM = DockerMount(self.mnt_dir, mnt_mkdir=True)
        _tmp_mnt_dir = DM.mount(image)

        # Remeber actual mounted fs in 'rootfs'
        chroot = os.path.join(_tmp_mnt_dir, 'rootfs')

        # Figure out which RHEL dist is in the chroot
        dist = self.helper._get_dist(chroot)

        # Fetch the CVE input data for the dist
        fetch = getInputCVE(self.tmp_dir)
        if not fetch._is_recent_enough(self.hours_old, dist):
            # TODO
            # This should probably be in a try/except
            fetch._fetch_single(dist)

        # Scan the chroot
        print self.helper._scan_cve(chroot, dist, scan_args)

        # Clean up
        self.helper._cleanup_by_path(_tmp_mnt_dir)

    def scan(self, image, scan_args):
        '''
        Wrapper function for basic security scans using
        openscap
        '''
        # Mount the temporary image/container to the dir
        DM = DockerMount(self.mnt_dir, mnt_mkdir=True)
        _tmp_mnt_dir = DM.mount(image)

        # Remeber actual mounted fs in 'rootfs'
        chroot = os.path.join(_tmp_mnt_dir, 'rootfs')

        # Scan the chroot
        print self.helper._scan(chroot, scan_args)

        # Clean up
        self.helper._cleanup_by_path(_tmp_mnt_dir)
