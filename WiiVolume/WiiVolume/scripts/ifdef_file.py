#!/usr/bin/python -u

# this routine reads C files and basically makes the whole file conditional on a define

import sys
import os
import string
import array
import struct

def ifdef_file():
    usageString = 'Reads a C file and ifdefs the whole contents\nUsage:\n%s def_token file1 file2 file3 ...' % sys.argv[0]

    if len(sys.argv) < 3:
        print usageString
        sys.exit(1)
    
    def_token = sys.argv[1]
    file_list = []
    for i in range(2, len(sys.argv)):
        file_list.append(sys.argv[i])

    for file in file_list:
    
        print "Processing %s" % file
        
        fin = open(file, 'r')
        lines = fin.readlines()
        fin.close()
        
        fout = open(file, 'w')
        fout.write('#ifdef %s\n\n' % (def_token))
        for line in lines:
            fout.write(line)
        fout.write('\n\n#endif // %s\n' % (def_token))
        fout.close()



# program starts here

if __name__ == '__main__':
    ifdef_file()   
   