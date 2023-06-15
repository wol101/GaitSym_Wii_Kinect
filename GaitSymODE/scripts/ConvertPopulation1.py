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
     

def ParseGenome(token_list, start_index):
    """Reads in a type -1 or type -2 genome"""
    
    i = start_index
    genome_type = int(token_list[i]); i = i + 1
    num_genes = int(token_list[i]); i = i + 1
    genes = []
    low_bounds = []
    high_bounds = []
    mutation_sd = []
    circular_flags = []
    for j in range(0, num_genes):
        genes.append(token_list[i]); i = i + 1
        low_bounds.append(token_list[i]); i = i + 1
        high_bounds.append(token_list[i]); i = i + 1
        mutation_sd.append(token_list[i]); i = i + 1
        if genome_type == -2:
            circular_flags.append(token_list[i]); i = i + 1
    extras = []
    for j in range(0, 5):
        extras.append(token_list[i]); i = i + 1
    
    end_index = i
    
    return {'genome_type':genome_type, 'num_genes':num_genes, 'genes':genes, 'low_bounds':low_bounds ,'high_bounds':high_bounds, 
    'mutation_sd':mutation_sd, 'circular_flags':circular_flags, 'extras':extras, 'end_index':end_index}

def WriteGenome(outf, genome):
    """Writes out type -1 or type -2 genomes to an already opened file"""
    
    outf.write(str(genome['genome_type']) + "\n")
    outf.write(str(genome['num_genes']) + "\n")
    
    for i in range(0, genome['num_genes']):
        outf.write(str(genome['genes'][i]) + "\t")
        outf.write(str(genome['low_bounds'][i]) + "\t")
        outf.write(str(genome['high_bounds'][i]) + "\t")
        if (genome['genome_type'] == -1):
            outf.write(str(genome['mutation_sd'][i]) + "\n")
        else:
            outf.write(str(genome['mutation_sd'][i]) + "\t")
            outf.write(str(genome['circular_flags'][i]) + "\n")
            
    outf.write(str(genome['extras'][0]) + "\t")
    outf.write(str(genome['extras'][1]) + "\t")
    outf.write(str(genome['extras'][2]) + "\t")
    outf.write(str(genome['extras'][3]) + "\t")
    outf.write(str(genome['extras'][4]) + "\n")
    
    return


def main():
    
    """Reads a type Population file and converts Type -1 genomes to Type -2"""
    
    usageString = "Reads a type Population file and converts Type -1 genomes to Type -2\nUsage:\n%s input_population input_model_genome output_population" % sys.argv[0]

    if len(sys.argv) < 4:
        PrintExit(usageString)
    
    input_population = sys.argv[1]
    input_model_genome = sys.argv[2]
    output_population = sys.argv[3]
    
    model_genome_tokens = ConvertFileToTokens(input_model_genome)
    if int(model_genome_tokens[0]) > 0: # this is actually a population
        model_genome = ParseGenome(model_genome_tokens, 1)
    else:
        model_genome = ParseGenome(model_genome_tokens, 0)
    
    tokens = ConvertFileToTokens(input_population)
    
    i = 0
    n_genomes = int(tokens[i]); i = i + 1
    
    outf = open(output_population, "w")
    outf.write(str(n_genomes) + "\n")
    
    start_index = i;
    for i_genome in range(0, n_genomes):
        print 'Processing genome %d' % (i_genome)
        
        genome = ParseGenome(tokens, start_index)
        
        if (genome['genome_type'] == -1):
            genome['circular_flags'] = model_genome['circular_flags']
            genome['genome_type'] = -2
        
        WriteGenome(outf, genome)
        
        start_index = genome['end_index']
        
    outf.close();
         
# program starts here

main()
   
