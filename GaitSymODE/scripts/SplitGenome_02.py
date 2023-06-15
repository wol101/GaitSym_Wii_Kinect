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

def IsANumber(theString):
    """checks to see whether a string is a valid number"""
    if re.match('([+-]?)(?=\d|\.\d)\d*(\.\d*)?([Ee]([+-]?\d+))?', theString) == None: return 0
    return 1
     

def main():
    """custom written to convert (approximately) 5 phase horse to 8 phase horse"""
    usageString = "Usage:\n%s offset file" % sys.argv[0]

    if len(sys.argv) < 3:
        PrintExit(usageString)
    
    offset = int(sys.argv[1])
    fileName = sys.argv[2]
        
    populationTokenList = ConvertFileToTokens(fileName)
    outpop = []
    c = 0
    populationLength = int(populationTokenList[c])
    outpop.append(populationLength)
    c = c + 1
    for i in range(0, populationLength):
        magic = int(populationTokenList[c])
        c = c + 1
        if magic != -1:
            PrintExit("Error parsing magic number")
        genomeLength = int(populationTokenList[c])
        c = c + 1
        if genomeLength != 62:
            PrintExit("Genome length of 62 required")
        genome = range(0, genomeLength)
        genomeLow = range(0, genomeLength)
        genomeHigh = range(0, genomeLength)
        genomeSD = range(0, genomeLength)
        for j in range(0, genomeLength):
            genome[j] = float(populationTokenList[c])
            c = c + 1
            genomeLow[j] = float(populationTokenList[c])
            c = c + 1
            genomeHigh[j] = float(populationTokenList[c])
            c = c + 1
            genomeSD[j] = float(populationTokenList[c])
            c = c + 1
        extra = range(0, 5)
        for j in range(0, 5):
            extra[j] = populationTokenList[c]
            c = c + 1
        
        outpop.append(-1)
        outpop.append(12 * 8 + 2) # genome length
        
        outpop.append(genome[0]) # duration
        outpop.append(genomeLow[0]) # duration
        outpop.append(genomeHigh[0]) # duration
        outpop.append(genomeSD[0]) # duration
        outpop.append(0) # phase
        outpop.append(0) # phase
        outpop.append(0) # phase
        outpop.append(0) # phase
        blocks = 5
        for b in range(0, blocks):
            rep = 1
            if (b == 0 or b == 1 or b == 3):
                    rep = 2
            for irep in range(0, rep):
                o = b * 12
                for j in range(0, 3):
                    ind = o + j + 2
                    outpop.append(genome[ind])
                    outpop.append(genomeLow[ind])
                    outpop.append(genomeHigh[ind])
                    outpop.append(genomeSD[ind])
                for j in range(0, 3):
                    ind = o + j + 2 + 12 * offset
                    if ind >= 62:
                        ind = ind - 60
                    if ind < 2:
                        ind = ind + 60
                    outpop.append(genome[ind])
                    outpop.append(genomeLow[ind])
                    outpop.append(genomeHigh[ind])
                    outpop.append(genomeSD[ind])

        for j in range(0, 5):
            outpop.append(extra[j])

    WriteTokenList(fileName + "_split", outpop)
    
# program starts here

main()
   
