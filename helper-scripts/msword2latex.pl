#!/usr/bin/perl -w
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

use strict;

my $Tmpf;

sub usage
{
    warn "$0: Usage: $0 msword.doc ...\n";
    exit 1;
}

sub err
{
    warn @_, "\n";
    if (-e $Tmpf)
    {
        if (!unlink $Tmpf) {
            warn "$0: Cannot remove $Tmpf\n";
        }
    }
    exit 2;
}

sub filterhtml
{
    local $_ = $_[0];
 
    s#\&quot;#\"#g;     # Change &quot; into \"
    s#<\!--.*-->##g;    # Discard one-line <!-- HTML-comments -->
    s#\&#\\\&#g;        # Escape ampersands, i.e. & becomes \&

    return $_;
}

sub filterlatex
{
    local $_ = $_[0];

    my $n = "\n";

    s#(\\chapter)#$n$n$1#;
    s#(\\section)#$n$n$1#;
    s#(\\subsection)#$n$n$1#;
    s#(\\subsubsection)#$n$n$1#;
    s#(\\par)#$n$1#;
    s#(\\begin)#$n$n$1#;
    s#(\\end)#$n$1#;
    s#(\\par)##;        # Get rid of \par. Use empty lines to mark paragraphs.

    return $_;
}

sub msword2latex
{
    my $file = shift @_ 
        or die "$0: Empty function argument in msword2latex()\n";

    my $options = '--nocredits --nohorizontalwhite --verticalwhite 0 --mainonly';

    open(HTML, "mswordview $options $file |")
        or err "Cannot execute mswordview $file : $!";
    
    $Tmpf = "$ENV{'HOME'}/tmp/$file.html"; # temporal *.html in ~/tmp/
        
    open(TMP, ">$Tmpf")
        or err "Cannot write temporal file: $Tmpf : $!";

    # Dump the MS-Word document as raw HTML and get rid of comments, entities
    # and other hazardous waste or mistypings, which could totally confuse the
    # html2latex convertor:

    my $str;

    while (defined($str = <HTML>)) {
	print TMP &filterhtml($str);
    }
    close HTML;
    close TMP;

    # Get rid of the ".doc" DOS filename extension, if any.

    $file =~ s/\.doc$//;

    open(LaTeX, "html2latex -n -s $Tmpf | fold -s -w 72 |")
        or err "Conversion from MS Word-HTML to LaTeX failed: $!";

    open(FILE, ">$file.tex")
        or err "Cannot write LaTeX file: $file : $!";

    # Convert $tmpf.html to $file.tex and try to "beautify" the
    # raw LaTeX code:

    while (defined($str = <LaTeX>)) {
	print FILE &filterlatex($str);
    }
    close LaTeX;
    close FILE;
}

sub main
{
    usage if (!@ARGV);
    foreach my $file (@ARGV)
    {
        if (!-e $file)
	{
            warn "$0 : No such file: $file -- skipped\n";
            next;
        }
        msword2latex($file);
    }
    if (-e $Tmpf)
    {
        if (!unlink($Tmpf))
	{
            warn "$0: Cannot remove $Tmpf\n";
	    exit 1;
	}
    }
    exit 0;
}

main;

# For each file argument, html2latex converts the text as HTML markup
# to LaTeX markup. Input will be taken from standard input for files
# named -. Output will to a similarly named file with a .tex extension
# (html2latex recognises .html extensions).
#
#
#        html2latex [opt ...] [file ...]
#
#        Where opt is one of:
#
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















