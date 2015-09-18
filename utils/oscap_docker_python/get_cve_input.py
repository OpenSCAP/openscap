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

import urllib2
import urlparse
from os.path import join, exists
from os import stat, utime
import datetime


class getInputCVE(object):
    '''
    Class to obtain the CVE data provided by RH used to scan
    for CVEs using openscap
    '''

    hdr = {'User-agent': 'Mozilla/5.0'}
    hdr2 = [('User-agent', 'Mozilla/5.0')]
    url = "http://www.redhat.com/security/data/oval/"
    dist_cve_name = "Red_Hat_Enterprise_Linux_{0}.xml"
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

        _url = urllib2.Request(dist_url, headers=self.hdr)
        # TODO
        # When dist specific files are available in bz form, some
        # of this logic may need to change
        try:
            resp = urllib2.urlopen(_url)

        except Exception as url_error:
            raise Exception("Unable to fetch CVE inputs due to"
                            .format(url_error))

        fh = open(dest_file, "w")
        fh.write(resp.read())
        fh.close()

        # Correct Last-Modified timestamp
        remote_ts = dict(resp.info())['last-modified']
        epoch = datetime.datetime.utcfromtimestamp(0)
        remote_dt = datetime.datetime.strptime(remote_ts, self.remote_pattern)
        seconds_epoch = (remote_dt - epoch).total_seconds()
        utime(dest_file, (seconds_epoch, seconds_epoch))

        return self.dest

    def _is_cache_same(self, dest_file, dist_url):
        '''
        Checks if the local cache version and the upstream
        version is the same or not.  If they are the same,
        returns True; else False.
        '''

        if not exists(dest_file):
            if self.DEBUG:
                print "No file in cache, fetching {0}".format(dest_file)
            return False
        opener = urllib2.OpenerDirector()
        opener.add_handler(urllib2.HTTPHandler())
        opener.add_handler(urllib2.HTTPDefaultErrorHandler())
        # Extra for handling redirects
        opener.add_handler(urllib2.HTTPErrorProcessor())
        opener.add_handler(urllib2.HTTPRedirectHandler())
        # Add the header
        opener.addheaders = self.hdr2
        # Grab the header
        res = opener.open(HeadRequest(dist_url))
        remote_ts = dict(res.info())['last-modified']
        # The remote's datetime
        remote_dt = datetime.datetime.strptime(remote_ts, self.remote_pattern)
        # Get the locals datetime from the file's mtime, converted to UTC
        local_dt = datetime.datetime.utcfromtimestamp((stat(dest_file))
                                                      .st_mtime)
        res.close()

        # Giving a two second comfort zone
        # Else we declare they are different
        if (remote_dt - local_dt).seconds > 2:
            if self.DEBUG:
                print "Had a local file {0} " \
                      "but it wasn't new enough".format(dest_file)
            return False
        if self.DEBUG:
            print "File {0} is same as upstream".format(dest_file)

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


class HeadRequest(urllib2.Request):
    def get_method(self):
        return 'HEAD'
