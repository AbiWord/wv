#!/bin/sh

#the idea here is generate all the necessary pics from the basic ones.
#so to replace these pics with better ones, which i would be very glad to
#recieve, make 14.gif to 25.gif transparent gifs with black as the foreground
#colour, and let this script loose on them to automatically generate the others
#
#C.

number=26;
for colour in \#0000ff \#00ffff \#00ff00 \#ff00ff \#ff0000 \#ffff00 \#ffffff \#000087 \#008787 \#008700 \#870087 \#870000 \#878700 \#a9a9a9 \#d3d3d3
	do
	for filename in 14.gif 15.gif 16.gif 17.gif 18.gif 19.gif 20.gif 21.gif 22.gif 23.gif 24.gif 25.gif
		do
		giftrans -g \#000000=$colour $filename -o $number.gif;
		number=$[number+1]
		done
	done
