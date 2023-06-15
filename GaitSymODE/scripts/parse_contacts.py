#!/usr/bin/python -u

import sys
import string
import array
import os
import re
import time

def PrintExit(value):
    """exits with error message"""
    sys.stderr.write(str(value) + "\n");
    sys.exit(1)

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
   
def GetValueFromTokenList(tokenList, match):
    """returns value after the matching token in the tokenList (match + 1)
       raises NoMatch on error"""
    for i in range(0, len(tokenList) - 1):
        if tokenList[i] == match: return tokenList[i + 1]
        
    raise "NoMatch", match

def IsANumber(theString):
    """checks to see whether a string is a valid number"""
    if re.match('([+-]?)(?=\d|\.\d)\d*(\.\d*)?([Ee]([+-]?\d+))?', theString) == None: return 0
    return 1
    
def ReturnNumber(theString):
    """checks to see whether a string is a valid number"""
    matchObj = re.search('([+-]?)(?=\d|\.\d)\d*(\.\d*)?([Ee]([+-]?\d+))?', theString)
    if matchObj == None: return 0
    return float(matchObj.group())
    
def main():
    usageString = "Usage:\n%s [-i file]" % sys.argv[0]

    inputFile = "ContactDebug.txt"

    i = 1
    while i < len(sys.argv):
        if sys.argv[i] == "-i":
            i = i + 1
            if i >= len(sys.argv): PrintExit(usageString)
            inputFile = sys.argv[i] 
            i = i + 1
            continue 

        PrintExit(usageString)
    
    # read the input file line by line        
    theInput = open(inputFile, 'r')
    theLines = theInput.readlines()
    theInput.close()
    
    # complex looping to collect together times
    print "Time LeftFootX LeftFootY LeftFootZ RightFootX RightFootY RightFootZ"
    lastTime = -1
    LeftFootX = LeftFootY = LeftFootZ = 0
    RightFootX = RightFootY = RightFootZ = 0
    for line in theLines:
        tokens = ConvertToListOfTokens(line)
        if (len(tokens) < 14):
            break 
        time = float(tokens[1])
        if (time != lastTime):
            if (lastTime != -1):
                print "%f %f %f %f %f %f %f" % (lastTime, LeftFootX, LeftFootY, LeftFootZ, RightFootX, RightFootY, RightFootZ)
            lastTime = time
            LeftFootX = LeftFootY = LeftFootZ = 0
            RightFootX = RightFootY = RightFootZ = 0
        if (tokens[2] == "LeftFoot"):
            LeftFootX += float(tokens[11])
            LeftFootY += float(tokens[12])
            LeftFootZ += float(tokens[13])
        if (tokens[2] == "RightFoot"):
            RightFootX += float(tokens[11])
            RightFootY += float(tokens[12])
            RightFootZ += float(tokens[13])
        

# program starts here

main()
   

