#!/bin/sh
#
# msword2latex --- Or how to translate MS Word documents into raw LaTeX.
#
# This script is supposed to be a little bit more advanced than using the
# famous UNIX 'strings' tool. You will need the mswordview and html2latex
# tools (as distributed by Debian) to be able to use this time-saving
# chainsaw. Yes --- it's just a chainsaw, which means: don't expect this
# script to produce a clean LaTeX output, but its better than nothing.
#
#
# (c) Copyright 1999, Martijn Tipker <mtipker@icce.rug.nl> ICCE,
# Institute for Educational Technology, University of Groningen.
# 
# This is GNU software. Use it at your own risk. Information about TeX/LaTeX
# can be found at the Comprehensive TeX Archive Network http://www.dante.de/


usage()
{
    echo "$0: Usage: $0 msword.doc [...]" >&2
    exit 1
}

msword2latex()
{
    tmpf="$HOME/tmp/$1.html"

    # Dump the MS-Word document as raw HTML.

    mswordview --nocredits         \
               --nohorizontalwhite \
               --verticalwhite 0   \
               --noannotations     \
               --mainonly $1 > $tmpf

    if [ $? -ne 0 ]
    then
	echo "$0: Cannot write tempory file: $tmpf" >&2
	exit 1
    fi

    # Get rid of HTML-comments, entities and other hazardous waste or
    # mistypings, which could totally confuse the html2latex convertor:

	# change &quot; into \" 
	# discard one-line HTML-comments
	# escape ampersands

    echo `sed -e 's#\&quot;#\"#g'  \
              -e 's#<\!--.*-->##g' \
              -e 's#\&#\\\&#g'     \
         $tmpf` > $tmpf

    # Get rid of ".doc" filename extension, if any.

    file=`echo $file | sed s/\.doc$//`


    # Convert $tmpf.html to $file.tex 

    html2latex -n -s $tmpf | fold --spaces --width 72 > $file.tex
}

main()
{

    if [ -z $1 ]
    then
	usage
    fi

    for file in "$@"
    do
	if [ ! -e $file ]
	then
	    echo "$0 : No such file: $file -- skipping ..." >&2
	    continue
        fi
	msword2latex $file
    done
    rm -f $tmpf || (echo "$0: Cannot remove $tmpf" >&2 ; exit 1)
    exit 0
}

main "$@"

# For each file argument, html2latex converts the text as HTML markup
# to LaTeX markup. Input will be taken from standard input for files
# named -. Output will to a similarly named file with a .tex extension
# (html2latex recognises .html extensions).
#
#
#        html2latex [opt ...] [file ...]
#        where opt is one of 
#                -n (number sections)
#                -t string (set title to string)
#                -a string (set author to string)
#                -p (page break after title/table of contents)
#                -c (table of contents)
#                -h string (string appears after \begin{document})
#                -f string (string appears before \end{document})
#                -o (specify LaTeX \documentstyle options
#                -s (write to stdout)
#        If file is - then input is taken from stdin and written to stdout.
#
# The mswordview program uses lls(1) to break the  OLE  Word document into
# its component streams, and then converts the document to html, and outputs
# the html to stdout.





