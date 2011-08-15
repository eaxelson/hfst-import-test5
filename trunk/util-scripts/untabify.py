#!/usr/bin/python

'''Print names of code files with literal tabs in hfst tree (given as arg).
Run with option -f, change tabs to spaces. Change tab width with -t argument.'''

import os
import sys
import string
import getopt

def shouldfix(fullpath):
    if fullpath.endswith('.cc') is False and fullpath.endswith('.h') is False:
        return False
    if '.svn' in fullpath:
        return False
    if fullpath.endswith('config.h'):
        return False
    return True

def main():
    tabsize = 4
    fix = False
    try:
        opts, args = getopt.getopt(sys.argv[1:], "ft:")
        if not args:
            raise getopt.error, "Give path of hfst tree as argument."
    except getopt.error:
        print "usage:", sys.argv[0], "[-f] [-t tabwidth] path to tree"
        return
    for optname, optvalue in opts:
        if optname == '-t':
            tabsize = int(optvalue)
        if optname == '-f':
            fix = True
    for (dirpath, dirnames, filenames) in os.walk(args[0]):
        for filename in filenames:
            process(os.path.join(dirpath, filename), tabsize, fix)

def process(file, tabsize, fix):
    if shouldfix(file) == False:
        return
    newtext = ''
    try:
        f = open(file)
        text = f.read()
        f.close()
        f = open(file)
        for line in f.readlines():
            if line.startswith('	'):
                newtext += string.expandtabs(line, tabsize)
            else:
                newtext += line
        f.close()
    except IOError, msg:
        print "%s: I/O error: %s" % (`file`, str(msg))
        return
    if newtext == text:
        return
#    backup = file + "~"
#    try:
#        os.unlink(backup)
#    except os.error:
#        pass
#    try:
#        os.rename(file, backup)
#    except os.error:
#        pass
    if fix:
        f = open(file, "w")
        f.write(newtext)
        f.close()
    print file

if __name__ == '__main__':
    main()
