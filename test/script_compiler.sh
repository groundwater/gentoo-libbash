#!/bin/sh

declare -i error=0

for script in $@
do
    ./variable_printer $script 2>&1 | diff -u $script.result -
    error+=$?
done

exit $error
