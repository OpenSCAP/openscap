''' oscap docker command '''
import argparse
from oscap_docker_python.oscap_docker_util import oscapScan
import docker
import sys
from requests import exceptions


class oscapDocker(object):
    ''' Generic class to call the scans '''
    def __init__(self):
        pass

    def set_args(self, args, unknown):
        '''
        Sets arguments for argparse into the oscapDocker class
        '''
        self.args = args
        self.unknown_args = unknown

    def cve_scan(self):
        ''' Wrapper function for container/image scanning '''
        OS = oscapScan()
        print OS.scan_cve(self.args.scan_target, self.unknown_args)

    def scan(self):
        ''' Wrapper functiopn to scan with openscap'''
        OS = oscapScan()
        print OS.scan(self.args.scan_target, self.unknown_args)


def ping_docker():
    ''' Simple check if the docker daemon is running '''
    client = docker.Client()
    client.ping()

if __name__ == '__main__':

    OD = oscapDocker()

    parser = argparse.ArgumentParser(description='oscap docker')
    subparser = parser.add_subparsers(help="commands")

    # Scan CVEs in image
    image_cve = subparser.add_parser('image-cve', help='Scan a docker image \
                                    for known vulnerabilities.')
    image_cve.set_defaults(func=OD.cve_scan)
    image_cve.add_argument('scan_target', help='Container or image to scan')

    # Scan an Image
    image = subparser.add_parser('image', help='Scan a docker image')
    image.add_argument('scan_target',
                       help='Container or image to scan')

    image.set_defaults(func=OD.scan)
    # Scan a container
    container = subparser.add_parser('container', help='Scan a running docker\
                                      container of given name.')
    container.add_argument('scan_target',
                           help='Container or image to scan')
    container.set_defaults(func=OD.scan)

    # Scan CVEs in container
    container_cve = subparser.add_parser('container-cve', help='Scan a \
                                         running container for known \
                                         vulnerabilities.')

    container_cve.set_defaults(func=OD.cve_scan)
    container_cve.add_argument('scan_target',
                               help='Container or image to scan')

    args, unknown = parser.parse_known_args()

    try:
        ping_docker()

    except exceptions.ConnectionError:
        print "The docker daemon does not appear to be running"
        sys.exit(1)

    OD.set_args(args, unknown)
    args.func()
