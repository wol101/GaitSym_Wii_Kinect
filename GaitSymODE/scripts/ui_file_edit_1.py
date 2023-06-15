#!/usr/bin/python -u

import sys
import string
import array
import os
import re
import time
import math
import xml.etree.ElementTree as etree

def ui_file_edit_1():
    usageString = "Edit the MainWindow ui file to convert  DoubleSpinBox to LineEdit\nUsage:\n%s inputXMLFile outputXMLFile" % sys.argv[0]

    if len(sys.argv) != 3:
        print usageString
        exit(1)
        
    inputXMLFile = sys.argv[1]
    outputXMLFile = sys.argv[2]
    
    tree = etree.parse(inputXMLFile)
    root = tree.getroot()
    
    # remove the customwidgets settings
    for customwidgets in root.findall('customwidgets'):
        root.remove(customwidgets)
        
    # walk the tree looking for the right sorts of widgets
    walkAll = tree.getiterator()
    for child in walkAll:
        # print(child)
        
        if child.tag == "widget":
            print 'OLD class="%s" name="%s"' % (child.attrib['class'], child.attrib['name'])
            
            if (child.attrib['class'] == "DoubleSpinBox"):
                name = child.attrib['name']
                child.attrib['name'] = name.replace('doubleSpinBox', 'lineEdit')
                child.attrib['class'] = 'QLineEdit'
                print 'NEW class="%s" name="%s"' % (child.attrib['class'], child.attrib['name'])
        
                for prop in child.findall('property'):
                    print 'property name="%s"' % (prop.attrib['name'])
                    if (prop.attrib['name'] == "mouseTracking"):
                        child.remove(prop)
                    if (prop.attrib['name'] == "focusPolicy"):
                        child.remove(prop)
                    if (prop.attrib['name'] == "sizePolicy"):
                        child.remove(prop)
                    if (prop.attrib['name'] == "decimals"):
                        child.remove(prop)
                    if (prop.attrib['name'] == "minimum"):
                        child.remove(prop)
                    if (prop.attrib['name'] == "maximum"):
                        child.remove(prop)
                    if (prop.attrib['name'] == "singleStep"):
                        child.remove(prop)
                    if (prop.attrib['name'] == "value"):
                        child.remove(prop)

            if (child.attrib['class'] == "SpinBox"):
                name = child.attrib['name']
                child.attrib['class'] = 'QSpinBox'
                print 'NEW class="%s" name="%s"' % (child.attrib['class'], child.attrib['name'])

            if (child.attrib['class'] == "LineEdit"):
                name = child.attrib['name']
                child.attrib['class'] = 'QLineEdit'
                print 'NEW class="%s" name="%s"' % (child.attrib['class'], child.attrib['name'])

    
    out = open(outputXMLFile, 'w')
    out.write(etree.tostring(root))
    out.close();
    


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

def GetIndexFromTokenList(tokenList, match):
    """returns the index pointing to the value in the tokenList 
       return -1 if not found"""
    for i in range(0, len(tokenList)):
        if tokenList[i] == match: return i

    return -1

# program starts here

if __name__ == '__main__':
    ui_file_edit_1()   

