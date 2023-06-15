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

def main():
    usageString = "Usage:\n%s [--controlFile file] [--dirPrefix dir]" % sys.argv[0]

    controlFile = "control.txt"
    dirPrefix = ""
    jumpFlag = 0;

    i = 1;
    while i < len(sys.argv):
        if sys.argv[i] == "--controlFile":
            i = i + 1
            if i >= len(sys.argv): PrintExit(usageString)
            controlFile = sys.argv[i] 
            i = i + 1
            continue 

        if sys.argv[i] == "--dirPrefix":
            i = i + 1
            if i >= len(sys.argv): PrintExit(usageString)
            dirPrefix = sys.argv[i] 
            i = i + 1
            continue 

        PrintExit(usageString)


    tokenList = ConvertFileToTokens(controlFile)

    # compulsory values
    try:
        kRangeControlStartPercent = float(GetValueFromTokenList(tokenList, "startPercent"))
        kRangeControlEndPercent = float(GetValueFromTokenList(tokenList, "endPercent"))
        kRangeControlIncrementPercent = float(GetValueFromTokenList(tokenList, "incrementPercent"))
        rootConfigFile1 = GetValueFromTokenList(tokenList, "rootConfigFile1")
        rootConfigFile2 = GetValueFromTokenList(tokenList, "rootConfigFile2")
        rootParameterFile = GetValueFromTokenList(tokenList, "rootParameterFile")
        rootGenomeFile = GetValueFromTokenList(tokenList, "rootGenomeFile")
        rootPopulationFile = GetValueFromTokenList(tokenList, "rootPopulationFile")
        rootModelStateFile = GetValueFromTokenList(tokenList, "rootModelStateFile")
        workingConfigFile = GetValueFromTokenList(tokenList, "workingConfigFile")
        workingParameterFile = GetValueFromTokenList(tokenList, "workingParameterFile")
        workingModelState = GetValueFromTokenList(tokenList, "workingModelState")
        gaCommand = GetValueFromTokenList(tokenList, "gaCommand")
        objectiveCommand = GetValueFromTokenList(tokenList, "objectiveCommand")
        mergeCommand = GetValueFromTokenList(tokenList, "mergeCommand")
        workingGenome = GetValueFromTokenList(tokenList, "workingGenome")
        workingPopulation = GetValueFromTokenList(tokenList, "workingPopulation")
        usePopulationFlag = int(GetValueFromTokenList(tokenList, "usePopulationFlag"))
        repeatsAtEachValue = int(GetValueFromTokenList(tokenList, "repeatsAtEachValue"))
        newStartCycleCount = float(GetValueFromTokenList(tokenList, "newStartCycleCount"))
        newStartCycleCountAfterMorph = float(GetValueFromTokenList(tokenList, "newStartCycleCountAfterMorph"))
        fitnessMinimum = float(GetValueFromTokenList(tokenList, "fitnessMinimum"))
        fitnessMinimumRepeats = float(GetValueFromTokenList(tokenList, "fitnessMinimumRepeats"))
        kinematicsFile = GetValueFromTokenList(tokenList, "kinematicsFile")
        kinematicsSkip = int(GetValueFromTokenList(tokenList, "kinematicsSkip"))

    except "NoMatch", data:
        PrintExit("Error: %s undefined" % data)
        
    if usePopulationFlag:
        print "Copying %s to %s" % (rootPopulationFile, workingPopulation)
        CopyFile(rootPopulationFile, workingPopulation)
    else:
        print "Copying %s to %s" % (rootGenomeFile, workingGenome)
        CopyFile(rootGenomeFile, workingGenome)

    print "Copying %s to %s" % (rootModelStateFile, workingModelState)
    CopyFile(rootModelStateFile, workingModelState)

    dirName = range(0, repeatsAtEachValue)
    repeatCount = 0
    kRangeControlPercent = kRangeControlStartPercent
    while kRangeControlPercent <= kRangeControlEndPercent:
        kRangeControl = kRangeControlPercent / 100.0 

        # build the working config file
        # merge the relevant files
        command = "%s -r ROOT_CONFIG_FILE1 %s -r ROOT_CONFIG_FILE2 %s -r WORKING_MODEL_STATE %s -r MERGE_FRACTION %s -r WORKING_CONFIG_FILE %s" % (mergeCommand, rootConfigFile1, rootConfigFile2, workingModelState, kRangeControl, workingConfigFile)
        print command
        if os.system(command):
            PrintExit("Error running %s" % command)

        # build the working parameter file

        theOutput = open(workingParameterFile, 'w')
        if usePopulationFlag: theOutput.write("startingPopulation %s\n\n" % workingPopulation)
        else: theOutput.write("modelGenome %s\n\n" % workingGenome)
        theOutput.write("extraDataFile %s\n\n" % workingConfigFile)
        theInput = open(rootParameterFile, 'r');
        theData = theInput.read();
        theInput.close();
        theOutput.write(theData);
        theOutput.close();

        fitnessMinimumRepeatCount = 0
        while fitnessMinimumRepeatCount < fitnessMinimumRepeats:
            # create the directory name
            theTime = time.localtime(time.time())
            dirName[repeatCount] = "%s%.4f/Run_%04d-%02d-%02d_%02d.%02d.%02d" % (dirPrefix, kRangeControl, theTime[0], theTime[1], theTime[2], theTime[3], theTime[4], theTime[5])
            os.makedirs(dirName[repeatCount])
            command = "%s -p %s -o %s" % (gaCommand, workingParameterFile, dirName[repeatCount])
            print command
            if os.system(command):
                PrintExit("Error running %s" % command)        

            # read the score
            fileList = os.listdir(dirName[repeatCount])
            fileList.sort()
            fileList.reverse()
            for name in fileList:
                if StartsWith(name, "BestGenome_") and EndsWith(name, ".txt"): break
            tokenList = ConvertFileToTokens(dirName[repeatCount] + "/" + name)
            if int(tokenList[0]) != -1:
                print "Error reading genome: %s" % (dirName[repeatCount] + "/" + name)
                continue
            genomeLength = int(tokenList[1])
            score = float(tokenList[genomeLength * 4 + 2])
            if score >= fitnessMinimum:
                print "Score %f >= fitnessMinimum %f so copying" % (score, fitnessMinimum)
                # copy the files over
                for name in fileList:
                    if StartsWith(name, "BestGenome_") and EndsWith(name, ".txt"): 
                        print "Copying %s to %s" % (dirName[repeatCount] + "/" + name, workingGenome)
                        CopyFile(dirName[repeatCount] + "/" + name, workingGenome)
                        break

                for name in fileList:
                    if StartsWith(name, "BestGenome_") and EndsWith(name, ".xml"): 
                        print "Copying %s to %s" % (dirName[repeatCount] + "/" + name, workingConfigFile)
                        CopyFile(dirName[repeatCount] + "/" + name, workingConfigFile)
                        break

                for name in fileList:
                    if StartsWith(name, "Population_") and EndsWith(name, ".txt"): 
                        print "Copying %s to %s" % (dirName[repeatCount] + "/" + name, workingPopulation)
                        CopyFile(dirName[repeatCount] + "/" + name, workingPopulation)
                        break
                break
            else:
                print "Score %f < fitnessMinimum %f so repeating" % (score, fitnessMinimum)
                fitnessMinimumRepeatCount = fitnessMinimumRepeatCount + 1
                if fitnessMinimumRepeatCount >= fitnessMinimumRepeats:
                    PrintExit("Unable to reach fitnessMinimum")
                    
        # run the objective

        command = "%s -c %s --outputModelStateAtCycle %s -S %d -K %s" % (objectiveCommand, workingConfigFile, newStartCycleCount, kinematicsSkip, kinematicsFile)
        print command
        if os.system(command): 
            PrintExit("Error running %s" % command)  
              
        print "Copying %s to %s" % (workingModelState, dirName[repeatCount] + "/" + workingModelState)    
        CopyFile(workingModelState, dirName[repeatCount] + "/" + workingModelState);
        print "Copying %s to %s" % (kinematicsFile, dirName[repeatCount] + "/" + kinematicsFile)    
        CopyFile(kinematicsFile, dirName[repeatCount] + "/" + kinematicsFile);
         
        repeatCount = repeatCount + 1;
        if repeatCount >= repeatsAtEachValue:
            repeatCount = 0;
            kRangeControlPercent = kRangeControlPercent + kRangeControlIncrementPercent

            # find the best score from the previour block
            maxScore = -1e30
            bestDir = 0
            for i in range(0, repeatsAtEachValue):
                fileList = os.listdir(dirName[i])
                fileList.sort()
                fileList.reverse()
                for name in fileList:
                    if StartsWith(name, "BestGenome_") and EndsWith(name, ".txt"): break
                tokenList = ConvertFileToTokens(dirName[i] + "/" + name)
                if int(tokenList[0]) != -1:
                    print "Error reading genome: %s" % (dirName[i] + "/" + name)
                    continue
                genomeLength = int(tokenList[1])
                score = float(tokenList[genomeLength * 4 + 2])
                print "Score", score
                if score > maxScore: 
                    maxScore = score
                    bestDir = i
            
            print "Best score %f found in %s" % (maxScore, dirName[bestDir])
            
            # copy the files over
            fileList = os.listdir(dirName[bestDir])
            fileList.sort()
            fileList.reverse()
            for name in fileList:
                if StartsWith(name, "BestGenome_") and EndsWith(name, ".txt"): 
                    print "Copying %s to %s" % (dirName[bestDir] + "/" + name, workingGenome)
                    CopyFile(dirName[bestDir] + "/" + name, workingGenome)
                    break
            
            for name in fileList:
                if StartsWith(name, "BestGenome_") and EndsWith(name, ".xml"): 
                    print "Copying %s to %s" % (dirName[bestDir] + "/" + name, workingConfigFile)
                    CopyFile(dirName[bestDir] + "/" + name, workingConfigFile)
                    break
            
            for name in fileList:
                if StartsWith(name, "Population_") and EndsWith(name, ".txt"): 
                    print "Copying %s to %s" % (dirName[bestDir] + "/" + name, workingPopulation)
                    CopyFile(dirName[bestDir] + "/" + name, workingPopulation)
                    break
                    
            # run the objective

            command = "%s -c %s --outputModelStateAtCycle %s" % (objectiveCommand, workingConfigFile, newStartCycleCountAfterMorph)
            print command
            if os.system(command):  
                PrintExit("Error running %s" % command)        
            
# program starts here

main()
   

