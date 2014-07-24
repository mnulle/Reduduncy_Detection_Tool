#!/bin/bash
#sweep through number of seconds for expiry time
for variable in {0..360..5}
do
	#analyse file given in $1, put output to test-output.txt
	./analyse-file $variable $1 >> test-output.txt
	#show output as it's compiled
	tail -1 test-output.txt
done

