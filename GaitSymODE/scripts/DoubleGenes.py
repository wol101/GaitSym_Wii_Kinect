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
    
    """Reads a Population file and duplicates each entry"""
    
    usageString = "Reads a Population file and duplicates each entry\nUsage:\n%s input_population output_population" % sys.argv[0]

    if len(sys.argv) < 3:
        PrintExit(usageString)
    
    input_population = sys.argv[1]
    output_population = sys.argv[2]
    
    tokens = ConvertFileToTokens(input_population)
    
    i = 0
    n_genomes = int(tokens[i])
    i = i + 1
    
    outf = open(output_population, "w")
    outf.write(str(n_genomes) + "\n")
    
    for i_genome in range(0, n_genomes):
        
        magic = tokens[i]
        i = i +1
        
        if (magic != "-1"):
            PrintExit("Error reading magic number")
            
        n_genes = int(tokens[i])
        i = i + 1
        
        data = []
        low = []
        high = []
        sd = []
        
        for i_gene in range(0, n_genes):
            data.append(tokens[i])
            i = i + 1
            low.append(tokens[i])
            i = i + 1
            high.append(tokens[i])
            i = i + 1
            sd.append(tokens[i])
            i = i + 1
        
        fitness = tokens[i]
        i = i + 1
        d0 = tokens[i]
        i = i + 1
        d1 = tokens[i]
        i = i + 1
        d2 = tokens[i]
        i = i + 1
        d3 = tokens[i]
        i = i + 1
    
        outf.write(magic + "\n")
        outf.write(str(n_genes * 2) + "\n")
        for i_gene in range(0, n_genes):
            outf.write("%s\t%s\t%s\t%s\n" % (data[i_gene], low[i_gene], high[i_gene], sd[i_gene]))
            outf.write("%s\t%s\t%s\t%s\n" % (data[i_gene], low[i_gene], high[i_gene], sd[i_gene]))
        outf.write("%s\t%s\t%s\t%s\t%s\n" % (fitness, d0, d1, d2, d3))
    
         
# program starts here

main()
   
