#!/bin/bash
index=0
while true
do
 echo "frame $index `date +"%N"`" > /dev/tecmp
 echo "Frame $index > /dev/tecmp"
 
 index=$((index+1))
 sleep 1
done
