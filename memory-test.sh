#!/bin/sh

# commands for finding memory problems in my programs
# mostly here just so I don't have to remember the variables needed

#--leak-check finds memory leaks
#--track-origins=yes will go much slower, but provide more details


valgrind --leak-check=yes ./cache-test -v
#valgrind --track-origins=yes --leak-check=yes ./cache-test -v
#valgrind --leak-check=yes ./analyse-file 120 pcaps/SimpleTest2.cap -v
