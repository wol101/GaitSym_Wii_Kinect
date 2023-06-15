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

def main():
    usageString = "Usage:\n%s -p parameterFile.txt -o outputFolder" % sys.argv[0]

    parameterFile = ""
    outputFolder = ""
    
    i = 1;
    while i < len(sys.argv):
        if sys.argv[i] == "-p":
            i = i + 1
            if i >= len(sys.argv): PrintExit(usageString)
            parameterFile = sys.argv[i] 
            i = i + 1
            continue 

        if sys.argv[i] == "-o":
            i = i + 1
            if i >= len(sys.argv): PrintExit(usageString)
            outputFolder = sys.argv[i] 
            i = i + 1
            continue 

        PrintExit(usageString)

    f =  '#PBS -N GaitSym\n' 
    f += '#PBS -l mppwidth=WIDTHSUB\n' 
    f += '#PBS -l mppnppn=NSUB\n' 
    f += '#PBS -l walltime=12:00:00\n' 
    f += '#PBS -j oe\n' 
    f += '#PBS -A e72-rob\n' 
    f += '\n' 
    f += 'cd WDSUB\n' 
    f += '\n' 
    f += 'aprun -n WIDTHSUB -N NSUB /work/e72/e72/wsellers/UnifiedMPI --serverArgs "-p PARAMETERFILESUB -o OUTPUTFOLDERSUB" --clientArgs "-q" \n'

    jobSubmitFile = f
    jobSubmitFile = jobSubmitFile.replace("WIDTHSUB", "512")
    jobSubmitFile = jobSubmitFile.replace("NSUB", "2")
    jobSubmitFile = jobSubmitFile.replace("WDSUB", os.getcwd())
    jobSubmitFile = jobSubmitFile.replace("PARAMETERFILESUB", parameterFile)
    jobSubmitFile = jobSubmitFile.replace("OUTPUTFOLDERSUB", outputFolder)
    jobSubmitFile = jobSubmitFile.replace("WIDTHSUB", "512")

    out = open("submit.pbs", "w")
    out.write(jobSubmitFile);
    out.close()

    command = "qsub submit.pbs"
    print command

    jobnum = os.popen(command).read()
    print jobnum
    
    finishFile = "GaitSym.o" + jobnum[0:-5]
    
    print "Looking for: " + finishFile
    while 1:
        if os.path.exists(finishFile):
            break
        time.sleep(5)

    
# program starts here

main()
   


