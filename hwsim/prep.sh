#!/bin/bash

JSCLOSURE="java -jar /usr/local/bin/closure.jar"
JSFLAGS="--compilation_level ADVANCED --rewrite_polyfills=false"

cat $1 | while read line; do
  if [[ $line == *"script src"*"script"* ]]; then
    jsfile=${line#*\"}
    jsfile=${jsfile%\"*}
    echo "<script>"
    $JSCLOSURE $JSFLAGS $jsfile
    echo "</script>"
  else
    echo $line
  fi
done
