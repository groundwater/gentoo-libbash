#!/bin/sh

declare -i error=0

./ast_printer -f $srcdir/bashast/features_script/features.sh\
    | diff -u $srcdir/bashast/features_script/features.sh.ast -
error+=$?

./ast_printer -t libbash.tokens -f $srcdir/bashast/features_script/features.sh\
    | diff -u $srcdir/bashast/features_script/features.sh.tokens -
error+=$?

exit $error
