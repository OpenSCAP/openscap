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

try:
    # Python2 imports
    import urlparse
    import urllib2 as urllib
except ImportError:
    #Python3 imports
    import urllib.parse as urlparse
    import urllib.request as urllib
from os.path import join, exists
from os import stat, utime
from sys import stderr
import datetime


class getInputCVE(object):
    '''
    Class to obtain the CVE data provided by RH used to scan
    for CVEs using openscap
    '''

    hdr = {'User-agent': 'Mozilla/5.0'}
    hdr2 = [('User-agent', 'Mozilla/5.0')]
    url = "https://www.redhat.com/security/data/oval/"
    dist_cve_name = "com.redhat.rhsa-RHEL{0}.xml.bz2"
    dists = [5, 6, 7]
    remote_pattern = '%a, %d %b %Y %H:%M:%S %Z'

    def __init__(self, fs_dest, DEBUG=False):
        ''' Simple init declaration '''
        self.dest = fs_dest
        self.DEBUG = DEBUG

    def _fetch_single(self, dist):
        '''
        Given a distribution number (i.e. 7), it will fetch the
        distribution specific data file if upstream has a newer
        input file.  Returns the path of file.
        '''
        cve_file = self.dist_cve_name.format(dist)
        dest_file = join(self.dest, cve_file)
        dist_url = (urlparse.urljoin(self.url, cve_file))
        if self._is_cache_same(dest_file, dist_url):
            return dest_file

        _url = urllib.Request(dist_url, headers=self.hdr)
        # TODO
        # When dist specific files are available in bz form, some
        # of this logic may need to change
        try:
            resp = urllib.urlopen(_url)

        except Exception as url_error:
            raise Exception("Unable to fetch CVE inputs due to {0}"
                            .format(url_error))

        fh = open(dest_file, "wb")
        fh.write(resp.read())
        fh.close()

        # Correct Last-Modified timestamp
        headers = self._parse_http_headers(resp.info())
        resp.close()
        try:
            remote_ts = headers['last-modified']
            epoch = datetime.datetime.utcfromtimestamp(0)
            remote_dt = datetime.datetime.strptime(remote_ts, self.remote_pattern)
            seconds_epoch = (remote_dt - epoch).total_seconds()
            utime(dest_file, (seconds_epoch, seconds_epoch))
        except KeyError:
            self._print_no_last_modified_warning(dist_url)

        return dest_file

    def _print_no_last_modified_warning(self, url):
        if self.DEBUG:
            stderr.write("Warning: Response header of HTTP doesn't contain " \
                         "\"last-modified\" field. Cannot determine version" \
                         " of remote file \"{0}\"\n".format(url))

    def _parse_http_headers(self, http_headers):
        '''
        Returns dictionary containing HTTP headers with lowercase keys
        '''

        headers_dict = dict(http_headers)
        return dict( (key.lower(), value) for key, value in headers_dict.items() )

    def _is_cache_same(self, dest_file, dist_url):
        '''
        Checks if the local cache version and the upstream
        version is the same or not.  If they are the same,
        returns True; else False.
        '''

        if not exists(dest_file):
            if self.DEBUG:
                stderr.write("No file in cache, fetching {0}\n".format(dest_file))
            return False

        opener = urllib.build_opener()
        # Add the header
        opener.addheaders = self.hdr2
        # Grab the header
        try:
            res = opener.open(HeadRequest(dist_url))
            headers = self._parse_http_headers(res.info())
            res.close()
            remote_ts = headers['last-modified']

        except urllib.HTTPError as http_error:
            if self.DEBUG:
                stderr.write("Cannot send HTTP HEAD request to get \"last-modified\"" \
                             " attribute of remote content file.\n{0} - {1}\n"
                             .format(http_error.code, http_error.reason))
            return False

        except KeyError:
            self._print_no_last_modified_warning(dist_url)
            return False

        # The remote's datetime
        remote_dt = datetime.datetime.strptime(remote_ts, self.remote_pattern)
        # Get the locals datetime from the file's mtime, converted to UTC
        local_dt = datetime.datetime.utcfromtimestamp((stat(dest_file))
                                                      .st_mtime)

        # Giving a two second comfort zone
        # Else we declare they are different
        if (remote_dt - local_dt).seconds > 2:
            if self.DEBUG:
                stderr.write("Had a local file {0} " \
                      "but it wasn't new enough\n".format(dest_file))
            return False
        if self.DEBUG:
            stderr.write("File {0} is same as upstream\n".format(dest_file))

        return True

    def fetch_dist_data(self):
        '''
        Fetches all the the distribution specific data used for
        input with openscap cve scanning and returns a list
        of those files.
        '''
        cve_files = []
        for dist in self.dists:
            cve_files.append(self._fetch_single(dist))
        return cve_files


class HeadRequest(urllib.Request):
    def get_method(self):
        return 'HEAD'
