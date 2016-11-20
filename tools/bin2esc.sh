#!/bin/bash

if [ "$1" == "" ]; then
	echo Missing name of file to convert 
	echo -e "\nUsage:\n$0 [filename] \n" 
	exit 1
fi

echo const char $(echo $1 | tr '.' '_')\[\] PROGMEM =

cat $1 | \
	xxd -i -c 32 | \
	sed 's/  0/"\\/g' | \
	sed 's/, 0/\\/g' | \
	sed 's/,//g' | \
	sed "s/$/\"/"

echo \;


