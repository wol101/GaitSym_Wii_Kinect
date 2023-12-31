#!/usr/bin/python -u

import sys
import string
import array
import os
import re
import time

def CopyFile(theInputFileName, theOutputFileName):
    """Copies the contents of a file"""
    theInput = open(theInputFileName, 'r')
    theData = theInput.read()
    theInput.close()
    theOutput = open(theOutputFileName, 'w')
    theOutput.write(theData)
    theOutput.close()

def StartsWith(theString, thePrefix):
    if theString[0: len(thePrefix)] == thePrefix:
        return 1
    return 0

def EndsWith(theString, theSuffix):
    if theString[len(theString) - len(theSuffix):] == theSuffix:
        return 1
    return 0

def PrintExit(value):
    """exits with error message"""
    sys.stderr.write(str(value) + "\n");
    sys.exit(1)

def WriteTokenList(filename, tokenList):
    theOutput = open(filename, 'w')
    for i in range(0, len(tokenList)):
        theOutput.write(str(tokenList[i]) + "\n")
    theOutput.close()
    

def ConvertToListOfTokens(theBuffer):
    """converts a string into a list of tokens delimited by whitespace"""
    
    charList = list(theBuffer)
    i = 0
    tokenList = []
    while i < len(charList):
        byte = charList[i]
        if byte in string.whitespace: 
            i = i + 1
            continue
        word = ""
        if byte == '"':
            i = i + 1
            byte = charList[i]
            while byte != '"':
                word = word + byte
                i = i + 1
                if i >= len(charList): PrintExit("Unmatched \" error")
                byte = charList[i]
        else:
            while (byte in string.whitespace) == 0:
                word = word + byte
                i = i + 1
                if i >= len(charList): break
                byte = charList[i]
        
        if len(word) > 0: tokenList.append(word)
        i = i + 1
            
    return tokenList        

def ConvertFileToTokens(filename):
    """reads in a file and converts to a list of whitespace delimited tokens"""
    
    input = open(filename, 'r')
    theBuffer = input.read()
    input.close()
    
    return ConvertToListOfTokens(theBuffer)
   
def GetIndexFromTokenList(tokenList, match):
    """returns the index pointing to the value in the tokenList (match + 1)
       raises NoMatch on error"""
    for i in range(0, len(tokenList) - 1):
        if tokenList[i] == match: return i + 1
        
    raise "NoMatch", match

def GetValueFromTokenList(tokenList, match):
    """returns value after the matching token in the tokenList (match + 1)
       raises NoMatch on error"""
    for i in range(0, len(tokenList) - 1):
        if tokenList[i] == match: return tokenList[i + 1]
        
    raise "NoMatch", match
    
def GetValueFromTokenListRemovingEquals(tokenList, match):
    """returns value after the matching token in the tokenList (match + 1)
       removing any equals sign if necessary
       raises NoMatch on error"""
    for i in range(0, len(tokenList) - 1):
        if tokenList[i] == match: 
            if tokenList[i + 1] == "=":
                return tokenList[i + 2]
            string = tokenList[i + 1]
            if string[0: 1] == "=":
                return string[1:]
            return string
        
    raise "NoMatch", match

def IsANumber(theString):
    """checks to see whether a string is a valid number"""
    if re.match('([+-]?)(?=\d|\.\d)\d*(\.\d*)?([Ee]([+-]?\d+))?', theString) == None: return 0
    return 1
     
def main():
    usageString = "Usage:\n%s cameraStart cameraEnd listOfFiles" % sys.argv[0]

    if len(sys.argv) < 4:
        PrintExit(usageString)
    
    # get the files in the right places
    cameraStartFile = sys.argv[1]
    cameraEndFile = sys.argv[2]
    fileList = []
    for i in range(3, len(sys.argv)):
        fileList.append(sys.argv[i])
    # I think the file list should be sorted...
    fileList.sort()
    
    tokenList = ConvertFileToTokens(cameraStartFile)
    cameraStartPosX = float(GetValueFromTokenListRemovingEquals(tokenList, "gCameraX"))
    cameraStartPosY = float(GetValueFromTokenListRemovingEquals(tokenList, "gCameraY"))
    cameraStartPosZ = float(GetValueFromTokenListRemovingEquals(tokenList, "gCameraZ"))
    cameraStartCOIX = float(GetValueFromTokenListRemovingEquals(tokenList, "gCOIx"))
    cameraStartCOIY = float(GetValueFromTokenListRemovingEquals(tokenList, "gCOIy"))
    cameraStartCOIZ = float(GetValueFromTokenListRemovingEquals(tokenList, "gCOIz"))
    
    tokenList = ConvertFileToTokens(cameraEndFile)
    cameraEndPosX = float(GetValueFromTokenListRemovingEquals(tokenList, "gCameraX"))
    cameraEndPosY = float(GetValueFromTokenListRemovingEquals(tokenList, "gCameraY"))
    cameraEndPosZ = float(GetValueFromTokenListRemovingEquals(tokenList, "gCameraZ"))
    cameraEndCOIX = float(GetValueFromTokenListRemovingEquals(tokenList, "gCOIx"))
    cameraEndCOIY = float(GetValueFromTokenListRemovingEquals(tokenList, "gCOIy"))
    cameraEndCOIZ = float(GetValueFromTokenListRemovingEquals(tokenList, "gCOIz"))
    
    for i in range(0, len(fileList)):
    
        # calculate camera
        fraction = float(i) / float(len(fileList) - 1)
        cameraPosX = cameraStartPosX + fraction * (cameraEndPosX - cameraStartPosX)
        cameraPosY = cameraStartPosY + fraction * (cameraEndPosY - cameraStartPosY)
        cameraPosZ = cameraStartPosZ + fraction * (cameraEndPosZ - cameraStartPosZ)
        cameraCOIX = cameraStartCOIX + fraction * (cameraEndCOIX - cameraStartCOIX)
        cameraCOIY = cameraStartCOIY + fraction * (cameraEndCOIY - cameraStartCOIY)
        cameraCOIZ = cameraStartCOIZ + fraction * (cameraEndCOIZ - cameraStartCOIZ)
    
        # read file
        input = open(fileList[i], 'r')
        theLines = input.readlines()
        input.close()

        # comment out #declare statements
        for j in range(0, len(theLines)):
            if StartsWith(theLines[j], "#declare"):
                theLines[j] = "// " + theLines[j]
    
        # write out file with new values
        output = open("Smooth_" + fileList[i], 'w')
        output.write("#declare gCameraX = %f;\n" % (cameraPosX))
        output.write("#declare gCameraY = %f;\n" % (cameraPosY))
        output.write("#declare gCameraZ = %f;\n" % (cameraPosZ))
        output.write("#declare gCOIx = %f;\n" % (cameraCOIX))
        output.write("#declare gCOIy = %f;\n" % (cameraCOIY))
        output.write("#declare gCOIz = %f;\n" % (cameraCOIZ))
        output.write("\n")
        for j in range(0, len(theLines)):
            output.write(theLines[j])
        output.close()
    
    
# program starts here

main()
   
