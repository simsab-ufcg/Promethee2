#!/bin/bash

export LC_ALL="C"
TIME_BETWEEN_COMMANDS=1
echo TIMESTAMP, PID, COMMAND, KB_READ/S, KB_WRITE/S
while [ -e /proc/$1 ]; do
  sudo /usr/bin/pidstat -d $TIME_BETWEEN_COMMANDS 1 | grep -e '[0-9]\{2\}:[0-9]\{2\}:[0-9]\{2\}[ \t]*[0-9]' | awk -v date="$( date +"%s" )" '{ if ($8) $7=$8; if (NF) print date", "$3", "$7", "$4", "$5 ; }' 2> /dev/null
done
