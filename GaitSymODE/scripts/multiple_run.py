#!/usr/bin/python -u

import sys
import string
import array
import os
import re
import time
import math

def PrintExit(value):
    """exits with error message"""
    sys.stderr.write(str(value) + "\n");
    sys.exit(1)

usageString = "Usage:\n%s command list_of_files ..." % sys.argv[0]

if len(sys.argv) < 3: PrintExit(usageString)

command = sys.argv[1]
listOfFiles = sys.argv[2:]

for i in listOfFiles:
    fullCommand = command + ' "' + i + '"'
    print fullCommand
    os.system(fullCommand)
    
