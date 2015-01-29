#!/bin/sh
#
# This is the help message for tool finnish-analyze.sh.
#
# Uncomment the lines beginning with two '#' chars when you add this piece of code
# to the script finnish-analyze.sh.
#

## if [ "$1" == "--help" -o "$1" == "-h" ] ; then
    echo
    echo Usage: $0 [OUTPUT FORMAT] [FILENAME]
    echo
    echo This is a tool for analyzing Finnish text.
    echo
    echo OUTPUT FORMAT can be one of the following: cg/CG, apertium, xerox, xerox being
    echo the default. If FILENAME is not given, input is read from standard input, i.e.
    echo 'line by line from the user (enter must be typed twice after each line). Ctrl+d or'
    echo  Ctrl+c ends the program.
    echo
    echo 'Analysis for input "taloissa" for different output formats:'
    echo
    echo 'apertium:  ^taloissa/talo N Ine Pl$'
    echo '      cg:  "<taloissa>"'
    echo '                   "talo N Ine Pl"'
    echo '   xerox:  taloissa'
    echo '           taloissa        talo N Ine Pl'
    echo
    echo For information on the analysis tags and their interpretation, see
    echo https://code.google.com/p/omorfi/wiki/TagSets.
    echo
    echo Example: analyze file finnish.txt, give output 'in' apertium format, save output to
    echo file finnish.analysis
    echo
    echo '  '$0 apertium finnish.txt '>' finnish.analysis
    echo
    echo Example: analyze user input line by line, give output 'in' default '('xerox')' format:
    echo
    echo '  '$0
    echo
    echo 'For more information (in Finnish) using the tool, see'
    echo 'https://kitwiki.csc.fi/twiki/bin/view/FinCLARIN/KielipankkiOhjeetTekstinAutomaattinenJasennys'
    echo
##   exit 0
## fi
