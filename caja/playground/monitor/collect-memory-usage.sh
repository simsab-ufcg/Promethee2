#!/bin/bash

export LC_NUMERIC="C"
TIME_BETWEEN_COMMANDS=1
echo TIMESTAMP, TOTAL, USED, FREE, SHARED, BUFFER/CACHE, AVAILABLE
while [ -e /proc/$1 ]; do
  free -k | grep Mem: | awk -v date="$( date +"%s" )" '{ print date", "$2", "$3", "$4", "$5", "$6", "$7 }' 2> /dev/null
  sleep $TIME_BETWEEN_COMMANDS
done
