#!/bin/sh

#the idea here is test all the word docs looking for a crash
#im using this on my 
#
#C.

for filename in *
	do
	echo filename is $filename
    ../wvText "$filename" > /dev/null
	test=$?
	echo status is $test
	if [ "$test" = "0" ] ; then
	echo $filename >> /tmp/simple
	fi
	if [ "$test" = "1" ] ; then
	echo $filename >> /tmp/complex
	fi
	if [ "$test" = "2" ] ; then
	echo $filename >> /tmp/neither
	fi
	done
