#!/bin/bash
# I was having issues where it would sometimes 'save' different amounts of bytes on different trials
# so I wrote this script to run the same thing over and over again to find irregularities
for f in {1..100} 
do
	# analyse the appropriate file
	#./analyse-file 120 pcaps/Refreshingsplit/Refreshingsplit_00099_20140709141853.pcap >> test
	./analyse-file 120 pcaps/Refreshing.pcapng >> test
	
	# store the output as a variable
	output=$(tail -1 test)
	# expected output
	expected="120,244009"
	echo "$output ?= $expected"

	# check if it's wrong
	if [ $output != $expected ]
	  then
		# warn that it's off
		echo "not right"
	fi
done
