#!/usr/bin/env python

__author__ = 'matt.kerr'
__copyright__ = "Copyright 2011, G2, Inc."
__credits__ = ["Matt Kerr"]
__license__ = "TODO"
__version__ = "TODO"
__maintainer__ = 'matt.kerr'
__email__ = "TODO"
__status__ = "Alpha"

import os

def performConfig(workingDir=None):
    os.makedirs(r"/tmp/scapVal/ind_tfc/e", exist_ok=True)
    os.makedirs(r"/tmp/scapVal/ind_tfc/ne", exist_ok=True)
    createFile(r"/tmp/scapVal/ind_tfc/e/1.txt", "abc")
    return
def createFile(filename, content):
    with open(filename, 'w') as file:
        file.write(content)
    return

if __name__=="__main__":
    performConfig()