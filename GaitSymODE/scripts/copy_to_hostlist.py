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

def path_components(string):
    components = []
    head = string
    while (1):
        (head, tail) = os.path.split(head)
        if (len(tail) == 0):
            components.append(head)
            break;
        else:
            components.append(tail)
        
    components.reverse()
    return components
    

def main():
    """Copies over an executable duplicating the path so that mpirun works as expected"""

    usageString = "Usage:\n%s [--hostfile file] executable" % sys.argv[0]

    hostsListFile = "hostfile.txt"
    
    if len(sys.argv) <2:
        PrintExit(usageString)
    
    i = 1;
    while i < len(sys.argv) - 1:
        if sys.argv[i] == "--hostfile":
            i = i + 1
            if i >= len(sys.argv) - 1: PrintExit(usageString)
            hostsListFile = sys.argv[i] 
            i = i + 1
            continue 

        PrintExit(usageString)

    executable = sys.argv[len(sys.argv) - 1]
    username = os.environ['USER']
    
    fin = open(hostsListFile, 'r');
    lines = fin.readlines();
    fin.close();
    hostsList = [];
    for line in lines:
        tokens = ConvertToListOfTokens(line)
        hostsList.append(tokens[0])

    components = path_components(executable)

    for host in hostsList:
    
        subpath = components[0]
        for i in range(1, len(components) - 1):
            subpath = os.path.join(subpath, components[i])
            command = 'ssh -l %s %s "mkdir %s"' % (username, host, subpath)
            print command
            os.system(command)
        
        
        command = "scp %s %s@%s:%s" % (executable, username, host, executable)
        print command
        os.system(command)
                        
            
# program starts here

main()
   

