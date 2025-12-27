#!/bin/bash

file="options.txt"
dialog_str="dialog --stdout --checklist \"Build options\" 30 60 5 "

while read -r line; do
	dialog_str+="\"$line\" \"\" off "	
done < $file
res=$(eval $dialog_str)
clear

retval=$?
if [ $retval -eq 1 ]; then
	exit 0
fi

if [ $retval -eq 255 ]; then
	echo "Error!"
	exit 255
fi

cd build
cmd="cmake .. "
onoff="ON"
for result in $res; do
	if [ "$result" = "Disable" ]; then
		onoff="OFF"
		continue
	fi
	cmd+="-D$result=$onoff "
done
eval $cmd > /dev/null
