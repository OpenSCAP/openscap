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
from os.path import join, exists, getmtime
import time


class getInputCVE(object):
    '''
    Class to obtain the CVE data provided by RH used to scan
    for CVEs using openscap
    '''

    hdr = {'User-agent': 'Mozilla/5.0'}
    url = "http://www.redhat.com/security/data/oval/"
    dist_cve_name = "Red_Hat_Enterprise_Linux_{0}.xml"
    dists = [5, 6, 7]

    def __init__(self, fs_dest):
        ''' Simple init declaration '''
        self.dest = fs_dest

    def _fetch_single(self, dist):
        '''
        Given a distribution number (i.e. 7), it will fetch the
        distribution specific data file
        '''
        cve_file = self.dist_cve_name.format(dist)
        dist_url = (urlparse.urljoin(self.url, cve_file))
        _url = urllib2.Request(dist_url, headers=self.hdr)
        try:
            resp = urllib2.urlopen(_url)
        except Exception as url_error:
            raise Exception("Unable to fetch CVE inputs")

        # TODO
        # When dist specific files are available in bz form,
        # will need to unbz these
        fh = open(join(self.dest, cve_file), "w")
        fh.write(resp.read())
        fh.close()

    def _is_recent_enough(self, hours, dist):
        '''
        Checks if the cve data already exists and if so whether
        it is in the given timeframe.  Returns bool
        '''
        if hours == 0:
            return False
        _fname = join(self.dest, self.dist_cve_name.format(dist))
        if not exists(_fname) or ((time.time() - getmtime(_fname))
           / (60 ** 2) > hours):
            return False
        return True

    def fetch_dist_data(self, hours_old):
        '''
        Fetches all the the distribution specific data used for
        input with openscap cve scanning
        '''
        for dist in self.dists:
            if not self._is_recent_enough(hours_old, dist):
                self._fetch_single(dist)
