#!/bin/sh

declare -i error=0

for ebuild in $@
do
    ECLASSDIR=$srcdir/scripts ./metadata_generator $ebuild | diff -u $ebuild.result -
    error+=$?
done

exit $error
