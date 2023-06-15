#!/usr/bin/python -u

import sys
import string
import array
import os
import re
import time
import xml.etree.ElementTree as etree

def ConvertToListOfTokens(charList):
    """converts a string into a list of tokens delimited by whitespace"""
    
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

def OptimiseWhitespace(in_str):
    """removes surplus whitespace characters whitespace"""

    tokens = ConvertToListOfTokens(in_str)
    out_str = tokens[0]
    for i in range(1, len(tokens)):
        out_str = out_str + " " + tokens[i]

    return out_str        

def GetIndexFromTokenList(tokenList, match):
    """returns the index pointing to the value in the tokenList 
       return -1 if not found"""
    for i in range(0, len(tokenList)):
        if tokenList[i] == match: return i

    return -1

def ExtractGeneNumber(charList):

    # find the g
    i = 0
    while i < len(charList):
        if charList[i] == 'g': 
            i = i + 1
            break
        i = i + 1
    
    # expecting '(' next
    if charList[i] != '(':
        return -1
        
    i = i + 1
    nstr = ""
    
    while i < len(charList):
        if charList[i] == ')':
            break
        nstr = nstr + charList[i]
        i = i + 1
    
    gene = int(nstr)
    return gene

def main():
    
    """Reads a config file with drivers and halves the duration and doubles the number of genes"""
    
    usageString = "Reads a config file with drivers and halves the duration and doubles the number of genes\nUsage:\n%s inputFile outputFile" % sys.argv[0]

    if len(sys.argv) < 3:
        print usageString
        return

    inputFile = sys.argv[1]
    outputFile = sys.argv[2]
    

    inputTree = etree.parse(inputFile)
    inputRoot = inputTree.getroot()
    
    for child in inputRoot:

        if child.tag == "DRIVER":
            dvp = child.attrib['DurationValuePairs']
            tokens = ConvertToListOfTokens(dvp)
            for i in range(0, len(tokens), 2):
                time = float(tokens[i])
                gene = ExtractGeneNumber(tokens[i + 1])
                if i == 0:
                    dvp2 = "%f [[g(%d)]] %f [[g(%d)]]" % (time / 2, gene * 2, time / 2, gene * 2 + 1)
                else:
                    dvp2 = dvp2 + " %f [[g(%d)]] %f [[g(%d)]]" % (time / 2, gene * 2, time / 2, gene * 2 + 1)
            
            child.attrib['DurationValuePairs'] = dvp2
    
    out = open(outputFile, 'w')
    out.write(etree.tostring(inputRoot))
    out.close();
    
    
# program starts here

main()
