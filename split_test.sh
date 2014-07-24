#!/bin/bash

#analyse all the refreshingsplit files. Used for finding some simpler files that would demonstrate odd behavior
FILES=./pcaps/Refreshingsplit/*
for f in $FILES
do
	#analyse each file
	./analyse-file 120 $f >> test
	#get the output for that file
	output=$(tail -1 test)
	empty="120,0"
	#if no bytes saved
	echo "$output = $empty"
	if [ $output == $empty ]
	  then
		#get rid of the file
		echo "empty" 
		rm $f
	  else
		#show it's useful
		echo "not empty"
	fi
done
