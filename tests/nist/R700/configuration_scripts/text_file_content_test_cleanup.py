#!/usr/bin/env python

__author__ = 'matt.kerr'
__copyright__ = "Copyright 2011, G2, Inc."
__credits__ = ["Matt Kerr"]
__license__ = "TODO"
__version__ = "TODO"
__maintainer__ = 'matt.kerr'
__email__ = "TODO"
__status__ = "Alpha"

import shutil

def performConfig(workingDir=None):
    shutil.rmtree(r"/tmp/e")
    shutil.rmtree(r"/tmp/ne")

if __name__ == "__main__":
    performConfig()
