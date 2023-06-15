#!/usr/bin/python -u

import sys
import string
import array
import os
import re
import time
import math

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
    
def ConvertToListOfTokensIgnoreQuotes(theBuffer):
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
        while (byte in string.whitespace) == 0:
            word = word + byte
            i = i + 1
            if i >= len(charList): break
            byte = charList[i]
        
        if len(word) > 0: tokenList.append(word)
        i = i + 1
            
    return tokenList        

def ConvertFileToTokensIgnoreQuotes(filename):
    """reads in a file and converts to a list of whitespace delimited tokens"""
    
    input = open(filename, 'r')
    theBuffer = input.read()
    input.close()
    
    return ConvertToListOfTokensIgnoreQuotes(theBuffer)  

def ConvertFileToLines(filename):
    """reads in a file and converts to a list of lines"""
    
    input = open(filename, 'r')
    theLines = input.readlines()
    input.close()
    
    return theLines 
    
    
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

def IsANumber(theString):
    """checks to see whether a string is a valid number"""
    if re.match('([+-]?)(?=\d|\.\d)\d*(\.\d*)?([Ee]([+-]?\d+))?', theString) == None: return 0
    return 1


def main():
    usageString = "Usage:\n%s maxReproductions improvementReproductions improvementThreshold logfile1 logfile2 ..." % sys.argv[0]
    
    if len(sys.argv) < 5: PrintExit(usageString)
    
    maxReproductions = int(sys.argv[1])
    improvementReproductions  = int(sys.argv[2])
    improvementThreshold = float(sys.argv[3])

    fileList = sys.argv[4:]

    for i in range(1, len(fileList)):
        lines = ConvertFileToLines(fileList[i])
        
        # find first data line
        count = 0
        while count < len(lines):
            tokens = ConvertToListOfTokens(lines[count])
            count = count + 1
            if IsANumber(tokens[0]):
                break
        
        if count == len(lines):
            PrintExit("Error: Could not find start line")
        
        # now do emulation
        currentBestScore = float(tokens[11])
        lastReproductionBestScore = currentBestScore
        lastReproduction = int(tokens[0])
        
        while count < len(lines):
            tokens = ConvertToListOfTokens(lines[count])
            count = count + 1
            if len(tokens) != 12:
                print("Error: terminated before maxReproductions")
                break
            reproduction = int(tokens[0])
            currentBestScore = float(tokens[11])
            if reproduction >= maxReproductions:
                break
            if reproduction >= lastReproduction + improvementReproductions:
                if currentBestScore < lastReproductionBestScore + improvementThreshold:
                    break
                lastReproductionBestScore = currentBestScore
                lastReproduction = int(tokens[0])

        
        print "%s %d %f" % (fileList[i], reproduction, currentBestScore)
    
# program starts here

main()
   

