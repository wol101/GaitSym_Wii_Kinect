#!/usr/bin/python -u

import sys
import string
import array
import os
import re
import time
import math

def edit_population():
    
    usageString = "Reads a type Population file and edits a specific gene\nUsage:\n%s input_population input_gene_num replacement_string output_population" % sys.argv[0]

    if len(sys.argv) != 5:
        print(usageString)
        exit(1)
    
    input_population = sys.argv[1]
    input_gene_num = int(sys.argv[2])
    replacement_string = sys.argv[3]
    output_population = sys.argv[4]
    
    # parse the replacement_string
    tokens = ConvertToListOfTokens(replacement_string)
    replacement_gene = tokens[0]
    replacement_low_bound = tokens[1]
    replacement_high_bound = tokens[2]
    replacement_mutation_sd = tokens[3]
    if len(tokens) > 4:
        replacement_circular_flag = tokens[4]
    else:
        replacement_circular_flag = 0
    
    tokens = ConvertFileToTokens(input_population)
    
    i = 0
    n_genomes = int(tokens[i]); i = i + 1
    
    outf = open(output_population, "w")
    outf.write(str(n_genomes) + "\n")
    
    start_index = i;
    for i_genome in range(0, n_genomes):
        print 'Processing genome %d' % (i_genome)
        
        genome = ParseGenome(tokens, start_index)
        
        genome['genes'][input_gene_num] = replacement_gene
        genome['low_bounds'][input_gene_num] = replacement_low_bound
        genome['high_bounds'][input_gene_num] = replacement_high_bound
        genome['mutation_sd'][input_gene_num] = replacement_mutation_sd
        genome['circular_flags'][input_gene_num] = replacement_circular_flag
        
        WriteGenome(outf, genome)
        
        start_index = genome['end_index']
        
    outf.close();
         

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
        else:
            circular_flags.append('0')
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


def WriteTokenList(filename, tokenList):
    theOutput = open(filename, 'w')
    for i in range(0, len(tokenList)):
        theOutput.write(str(tokenList[i]) + "\n")
    theOutput.close()
    

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

def ConvertFileToTokens(filename):
    """reads in a file and converts to a list of whitespace delimited tokens"""
    
    input = open(filename, 'r')
    theBuffer = input.read()
    input.close()
    
    return ConvertToListOfTokens(theBuffer)
        

# program starts here

if __name__ == '__main__':
    edit_population()   

   
