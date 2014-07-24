#!/bin/bash

if [ $# -lt 1 ]; then
  echo 'Usage: <git-full-commit.sh "message">'
else
  make clean
  rm *.txt
  git add .
  git commit -m "$1"
  git push -u origin master
fi
