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
print <<__END_OF_HTML_CODE1__;

<html>
<head><title>wvBugReport, Your Bug Report</title></head>
<body link="#CC0000" alink="#FF3300" vlink="#330099" text="#000000" bgcolor="#336666">
<table CELLPADDING=0 CELLSPACING=2 border="0">  <!-- begin table-->
<tr>
<td>
<br>
</td>
</tr>

<tr>

<td rowspan="2" valign="top" width="110">

<table CELLPADDING=2 CELLSPACING=0 width="100%" bgcolor="#000000" border="0">
<tr>
<td>
<table CELLPADDING=3 CELLSPACING=0 width="100%" bgcolor="#ffffff" border="0">
<tr>
<td>
<div align="center">
<font color="#336666">
<b>Navigation</b>
</font>
</div>
<hr width="25%">
<a href="../wvHome.html">wvHome</a><br>
&nbsp;&nbsp;&nbsp;<a href="../wvWare.html">wvWare</a><br>
&nbsp;&nbsp;&nbsp;<a href="../wvOnlineDemo.html">wvOnlineDemo</a><br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;wvBugReport<br>
</td>
</tr>
</table>
</td>
</tr>
</table>

</td>

<td>

<table CELLPADDING=2 CELLSPACING=0 width="100%" border="0" bgcolor="#000000">
<tr>
<td>
<table CELLPADDING=3 CELLSPACING=0 width="100%" border="0" bgcolor="#ffffff">
<tr>
<td>

<table CELLPADDING=0 CELLSPACING=0 width="100%" border="0">
<tr>
<td>

<font color="#336666">
<h1>wvBugReport</h1>
<h2>Your Bug Report for wvHtml</h2>
</font>
</td>
<td>
<div align=right>
<img src="../pics/wvMedium.jpg" alt = "A Word 8 converter for Unix">
</div>
</td>
</tr>
</table>
</td>
</tr>
</table>
</td>
</tr>
</table>

</td>
<td valign="top" width="10">
</td>

</tr>

<tr>
<td>
<table CELLPADDING=2 CELLSPACING=0 width="100%" bgcolor="#000000" border="0">
<tr>
<td>
<table CELLPADDING=3 CELLSPACING=0 width="100%" bgcolor="#ffffff" background="../pics/weave.jpg" border="0">
<tr>
<td>
<h1>wvHtml Online Demonstration</h1>
__END_OF_HTML_CODE1__

print "<table border=1><tr><td>Filename</td><td>Comment</td><td>Status</td></tr><tr>\n";
print "<td valign=\"top\">$in{'name'}</td>\n";
print "<td valign=\"top\">$in{'comments'}&nbsp;</td>\n";
print "<td valign=\"top\">Pending</td>\n";
print "</tr></table>\n";
print "<a href=\"../wvBugListing.html\">Full Bug Listing</a>\n";

print <<__END_OF_HTML_CODE2__;

</td>
</tr>
</table>
</td>
</tr>
</table>

</TD>
<td valign="top" width="10">
</td>
</TR>
</TABLE>

</BODY>
</HTML>

__END_OF_HTML_CODE2__
}

sub print_file
{
  open(DATAFILE,">>wvBugList.shtml") || die print "Failed to open links.htm";
		print DATAFILE "<tr>\n";
		print DATAFILE "<td valign=\"top\">$in{'name'}</td>\n";
		print DATAFILE "<td valign=\"top\">$in{'comments'}</td>\n";
		print DATAFILE "<td valign=\"top\">Pending</td>\n";
		print DATAFILE "<td valign=\"top\">&nbsp;</td>\n";
		print DATAFILE "</tr>\n";
		close DATAFILE;
}

