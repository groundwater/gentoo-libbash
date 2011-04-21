#!/bin/sh

declare -i error=0

for ebuild in $@
do
    ./metadata_generator $ebuild | diff -u $ebuild.result -
    error+=$?
done

exit $error
