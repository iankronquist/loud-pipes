#!/bin/bash
#
# This shell script is only an example of how the same type of
# processing as is required for your Homweork #3 C program can
# be done in a Bash script.
#
# Jesse
#
# $Author: chaneyr $
# $Date: 2014/10/31 04:11:36 $
# $RCSfile: word_freq.bash,v $
# $Revision: 1.1 $
#

function removeNonAlpha {
    # convert all the non-alphabetic characters into spaces
    sed 's/[^a-zA-Z]/ /g' $1
}

function lowerCase {
    # convert all upper-case characters to lower case
    tr '[A-Z]' '[a-z]'
}

function splitLines {
    # split all the words on a single line into a bunch of lines
    awk '{ for(i = 1; i <= NF; i++) { print $i; } }'
}

removeNonAlpha $1 | lowerCase | splitLines | sort | uniq -c
