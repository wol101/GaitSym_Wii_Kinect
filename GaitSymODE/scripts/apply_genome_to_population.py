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

def ConvertFileToLines(filename):
    """reads in a file and converts to a list of lines"""

    input = open(filename, 'r')
    theLines = input.read().splitlines()
    input.close()

    return theLines 


def main():
    usageString = "Usage:\n%s [-r root_config_file] [-e apply_genome_executable] [-p population_file]" % sys.argv[0]
    
    root_config_file = 'workingConfigFile.xml'
    home_folder = os.environ['HOME']
    population_file = ''
    apply_genome_executable = home_folder + '/Unix/cvs/ApplyGenome/bin/apply_genome'
    if (os.path.isfile(apply_genome_executable) == 0):
        apply_genome_executable = home_folder + '/cvs/ApplyGenome/bin/apply_genome'
        if (os.path.isfile(apply_genome_executable) == 0):
            print "Cannot find apply_genome_executable"
            return

    i = 1;
    while i < len(sys.argv):
        if sys.argv[i] == "-r":
            i = i + 1
            if i >= len(sys.argv): PrintExit(usageString)
            root_config_file = sys.argv[i]
            i = i + 1
            continue

        if sys.argv[i] == "-e":
            i = i + 1
            if i >= len(sys.argv): PrintExit(usageString)
            apply_genome_executable = sys.argv[i]
            i = i + 1
            continue
            
        if sys.argv[i] == "-p":
            i = i + 1
            if i >= len(sys.argv): PrintExit(usageString)
            population_file = sys.argv[i]
            i = i + 1
            continue
            
        sys.stderr.write(usageString + "\n");
        sys.exit(1)

    working_folder = './'
    if len(population_file) == 0:
        fileList = os.listdir(working_folder)
        fileList.sort()
        fileList.reverse()
        for name in fileList:
            if StartsWith(name, "Population_") and EndsWith(name, ".txt"):
                population_file = name
                break
    
    if len(population_file) == 0:
        print "Cannot find population_file"
        return

    
    # read the population
    population_lines = ConvertFileToLines(population_file)
    
    i = 0
    n_genomes = int(population_lines[i])
    i = i + 1
    
    for j in range(0, n_genomes):
        
        genome_name = 'genome_%07d.txt' % j
        genome_xml_name = 'genome_%07d.xml' % j
        fid = open(genome_name, 'w')
        
        genome_type = int(population_lines[i])
        fid.write(population_lines[i] +' \n')
        i = i + 1
        genome_size = int(population_lines[i])
        fid.write(population_lines[i] +' \n')
        i = i + 1
        
        for k in range(0, genome_size):
            fid.write(population_lines[i] +' \n')
            i = i + 1
        
        fid.write(population_lines[i] +' \n')
        i = i + 1
        
        fid.close()
     
        command = '"%s" --inputXML "%s" --inputGenome "%s" --outputXML "%s"' % (apply_genome_executable, root_config_file, genome_name, genome_xml_name)
        print command
        os.system(command)
     

    
# program starts here

main()
   

