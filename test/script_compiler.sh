#!/bin/sh

declare -i error=0

for script in $@
do
    ./variable_printer $script | diff -u $script.result -
    error+=$?
done

exit $error
