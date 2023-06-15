#!/usr/bin/python -u

import sys
import os
import re

def regex_replace():

    usageString = "Replace all occurances using regex in a list of files\nUsage:\n%s search_term replace_term list_of_files" % sys.argv[0]

    if len(sys.argv) < 4:
        print(usageString)
        exit(1)
        
    search_term = sys.argv[1]
    replace_term = sys.argv[2]
    list_of_files = sys.argv[3:]
    
    for file_name in list_of_files:
    
        fin = open(file_name, 'r')
        lines = fin.readlines()
        fin.close()

        changed_count = 0
        mod_lines = ''
        for line in lines:
            line_mod = re.sub(search_term, replace_term, line)
            mod_lines = mod_lines + line_mod
            if (line_mod != line):
                changed_count = changed_count + 1

        print '%s: %s %d' % (file_name, search_term, changed_count)
    
        if changed_count > 0:
            os.rename(file_name, file_name + '.bak')
            fout = open(file_name, 'w')
            fout.write(mod_lines)
            fout.close()
        
# program starts here

if __name__ == '__main__':
    regex_replace()   


