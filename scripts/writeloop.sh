#!/bin/bash
index=0
while true
do
 echo "frame $index `date +"%N"`" > /dev/tecmp_primatec
 echo "Frame $index > /dev/tecmp_primatec"
 
 index=$((index+1))
 sleep 1
done
