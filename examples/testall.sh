#!/bin/sh

#the idea here is test all the word docs looking for a crash
#im using this on my 
#
#C.

for filename in *
	do
	echo filename is $filename
    ../mswordview "$filename" -o /tmp/mswv--
	test=$?
	rm -rf /tmp/mswv--*
	echo status is $test
	if [ "$test" != "0" ] ; then
	exit -1
	fi
	done
