#!@OSCAP_DOCKER_PYTHON@

# Copyright (C) 2015 Brent Baude <bbaude@redhat.com>
# Copyright (C) 2015 Brent Baude <contact@d0m.tech>
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

''' oscap docker command '''

import argparse
from oscap_docker_util import OscapDockerScan
import docker
import sys
from requests import exceptions
import traceback


def ping_docker():
    ''' Simple check if the docker daemon is running '''
    # Class docker.Client was renamed to docker.APIClient in
    # python-docker-py 2.0.0.
    try:
        client = docker.APIClient()
    except AttributeError:
        client = docker.Client()
    client.ping()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='oscap docker',
                                     epilog='See `man oscap` to learn \
                                     more about OSCAP-ARGUMENTS')
    parser.add_argument('--oscap', dest='oscap_binary', default='', help='Set the oscap binary to use')
    subparser = parser.add_subparsers(help="commands")

    # Scan CVEs in image
    image_cve = subparser.add_parser('image-cve', help='Scan a docker image \
                                    for known vulnerabilities.')
    image_cve.set_defaults(func=OscapDockerScan.scan_cve, is_image=True)
    image_cve.add_argument('scan_target', help='Container or image to scan')

    # Scan an Image
    image = subparser.add_parser('image', help='Scan a docker image')
    image.add_argument('scan_target',
                       help='Container or image to scan')

    image.set_defaults(func=OscapDockerScan.scan, is_image=True)
    # Scan a container
    container = subparser.add_parser('container', help='Scan a running docker\
                                      container of given name.')
    container.add_argument('scan_target',
                           help='Container or image to scan')
    container.set_defaults(func=OscapDockerScan.scan, is_image=False)

    # Scan CVEs in container
    container_cve = subparser.add_parser('container-cve', help='Scan a \
                                         running container for known \
                                         vulnerabilities.')

    container_cve.set_defaults(func=OscapDockerScan.scan_cve, is_image=False)
    container_cve.add_argument('scan_target',
                               help='Container or image to scan')

    args, leftover_args = parser.parse_known_args()
    

    if "func" not in args:
        parser.print_help()
        sys.exit(2)

    try:
        ping_docker()
    except exceptions.ConnectionError:
        print("The docker daemon does not appear to be running")
        sys.exit(1)

    try:
        ODS = OscapDockerScan(args.scan_target, args.is_image, args.oscap_binary)
        rc = args.func(ODS, leftover_args)
    except ValueError as e:
        raise e
        sys.exit(255)
    except Exception as exc:
        traceback.print_exc(file=sys.stdout)
        sys.stderr.write("!!! WARNING !!! This software have crash, so you should \
        check that no temporary container is still running\n")
        sys.exit(255)

    sys.exit(rc)
