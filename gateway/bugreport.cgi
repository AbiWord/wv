#!/usr/bin/perl

print "Content-type:text/html\n\n";

require ("cgi-lib.pl");
&ReadParse;

if ($in{'name'})
{
  &print_web_page;
  &print_file;
}
else
{
  &print_error_page;
}

exit;

sub print_error_page
{
  print "<html>\n";
  print "<body>\n";
  print "There was an error in submitting this bug report\n, sorry\n";
  print "</body>\n";
  print "</html>\n"; 
}

sub print_web_page
{  
  print "<html>";
  print "<body>"; 
  print "<br><br><br>\n";
  print "Thank you for taking the time to fill out the bugreport.\n";
  print "<br><br><br>\n";
  print "<center>\n";
  print "</body>\n";
  print "</html>\n";
}

sub print_file
{
  open(DATAFILE,">>/home/sysadm/caolan/tmp/upload-mswordview/fileinfo") || die print "Failed to open links.htm";
  		print DATAFILE "--begin commend--\nFilename is ";
  		print DATAFILE "$in{'name'}\n";
        if ($in{'comments'})
        {
          print DATAFILE "$in{'comments'}\n";
        }     
        print DATAFILE "--end comment--\n";
  close DATAFILE;
}

