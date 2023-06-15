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
    usageString = "Usage:\n%s -i file -n interval -s skip" % sys.argv[0]

    inputFile = ""
    interval = -1
    skip = 1
    
    i = 1
    while i < len(sys.argv):
        if sys.argv[i] == "-i":
            i = i + 1
            if i >= len(sys.argv): PrintExit(usageString)
            inputFile = sys.argv[i] 
            i = i + 1
            continue 
            
        if sys.argv[i] == "-n":
            i = i + 1
            if i >= len(sys.argv): PrintExit(usageString)
            interval = int(sys.argv[i])
            i = i + 1
            continue 

        if sys.argv[i] == "-s":
            i = i + 1
            if i >= len(sys.argv): PrintExit(usageString)
            skip = int(sys.argv[i])
            i = i + 1
            continue 

        PrintExit(usageString)

    if (len(inputFile) == 0):
        PrintExit(usageString)
    if (interval < 1):
        PrintExit(usageString)
    
    # read the input file line by line        
    theInput = open(inputFile, 'r')
    theLines = theInput.readlines()
    theInput.close()
    
    print theLines[0],
    tokens = ConvertToListOfTokens(theLines[1])
    w = len(tokens)
    s = -1
    for d in range(1, len(theLines)):
        s += 1
        if (s % skip) == 0:
            c = d
            sums = []
            for j in range(0, w):
                sums.append(0)
            for i in range(0, interval):
                tokens = ConvertToListOfTokens(theLines[c])
                c += 1
                if c >= len(theLines): break
                for j in range(0, w):
                    sums[j] += float(tokens[j])
            line = ""
            for j in range(0, w):
                sums[j] /= interval
                if (j < w - 1):
                    line += "%f " % (sums[j])
                else:
                    line += "%f" % (sums[j])
            print line
            

# program starts here

main()
   

