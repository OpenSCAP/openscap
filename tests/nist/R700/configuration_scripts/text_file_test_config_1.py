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
import shutil

def performConfig(workingDir=None):
    shutil.rmtree(r"/tmp/e", ignore_errors=True)
    os.makedirs(r"/tmp/e")
    shutil.rmtree(r"/tmp/ne", ignore_errors=True)
    os.makedirs(r"/tmp/ne")
    createFile(r"/tmp/e/1.txt", "abc")
    return
def createFile(filename, content):
    with open(filename, 'w') as file:
        file.write(content)
    return

if __name__=="__main__":
    performConfig()
