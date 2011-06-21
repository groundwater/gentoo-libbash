#!/bin/sh

declare -i error=0
result=$(mktemp)

for script in $@
do
    sed "s/@srcdir@/$srcdir/" $script.result > $result
    ./variable_printer $script 2>&1 | diff -u $result -
    error+=$?
done

rm -rf $result
exit $error
