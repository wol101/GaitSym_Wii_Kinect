#!/usr/bin/python -u

import sys
import string
import array
import os
import re
import time
import math
import pty
import signal

def PtySSH(hostname, username, password, commands):
    output = ""
    pid, fd = pty.fork()
    if pid == 0:
        os.execv("/usr/bin/ssh",["ssh", "-l", username, hostname])
    else:
        time.sleep(2)
        s = os.read(fd,1024)
        output = output + s
        print s,
    
        if (len(password) > 0):
            os.write(fd, password + "\n")
            time.sleep(2) 
            s = os.read(fd,1024)
            output = output + s
            print s,
    
        os.write(fd, "\n")
        time.sleep(2)
        s = os.read(fd,1024)
        output = output + s
        print s,
        os.write(fd, "\n")
        time.sleep(2)
        s = os.read(fd,1024)
        output = output + s
        print s,
            
        for command in commands:
            os.write(fd, command + "\n")
            time.sleep(2)
            s = os.read(fd,1024)
            output = output + s
            print s,

        os.write(fd, "\n")
        time.sleep(2)
        s = os.read(fd,1024)
        output = output + s
        print s,
        
        os.write(fd, "exit\n")
        time.sleep(2)
        s = os.read(fd,1024)
        output = output + s
        print s,
    
    return output

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

def GetNumberListFromTokenList(tokenList, match):
    """returns value after the matching token in the tokenList (match + 1)
       raises NoMatch on error"""
    l = []
    for i in range(0, len(tokenList) - 1):
        if tokenList[i] == match:
	    for j in range(i + 1, len(tokenList)):
	    	if IsANumber(tokenList[j]):
		    l.append(float(tokenList[j]))
		else:
		    return l
	    return l
        
    raise "NoMatch", match

def IsANumber(theString):
    """checks to see whether a string is a valid number"""
    if re.match('([+-]?)(?=\d|\.\d)\d*(\.\d*)?([Ee]([+-]?\d+))?', theString) == None: return 0
    return 1

def main():
    """Copies over the files in the current folder and starts a command on a list of hosts
       HostFile format is hostname username numInstances"""

    usageString = "Usage:\n%s [--hostsList file] [--remoteCommand command]" % sys.argv[0]

    hostsListFile = "HostsList.txt"
    remoteCommand = "gaitsym_tcp -L hosts_remote.txt -q"
    
    i = 1;
    while i < len(sys.argv):
        if sys.argv[i] == "--hostsList":
            i = i + 1
            if i >= len(sys.argv): PrintExit(usageString)
            hostsListFile = sys.argv[i] 
            i = i + 1
            continue 

        if sys.argv[i] == "--remoteCommand":
            i = i + 1
            if i >= len(sys.argv): PrintExit(usageString)
            remoteCommand = sys.argv[i] 
            i = i + 1
            continue 
            
        PrintExit(usageString)


    hostsList = ConvertFileToTokens(hostsListFile)

    index = 0
    while index < len(hostsList):
    
        host = hostsList[index]
        username = hostsList[index + 1]
        numInstances = int(hostsList[index + 2])
        index = index + 3
    
        for i in range(0, numInstances):
            # create the directory name
            theTime = time.localtime(time.time())
            remFolder = "Run_%04d-%02d-%02d_%02d.%02d.%02d" % (theTime[0], theTime[1], theTime[2], theTime[3], theTime[4], theTime[5])
            command = PtySSH(host, username, "", ["mkdir %s" % (remFolder)])

            # copy the files over 
            fileList = os.listdir('.')
            fileList.sort()
            for f in fileList:
                command = "scp %s %s@%s:%s/%s" % (f, username, host, remFolder, f)
                print command
                os.system(command)

            # run the command
            command = PtySSH(host, username, "", ["cd %s" % (remFolder), "nohup ./%s &" % (remoteCommand)])   
    
                
            
# program starts here

main()
   

