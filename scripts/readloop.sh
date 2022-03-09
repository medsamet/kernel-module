#!/bin/bash

while true
do
  
  content=$(cat /dev/tecmp_primatec_out)
  if [ "$content" != "" ]
  then
    echo "`date +"%N"` -- $content"
  fi
done
