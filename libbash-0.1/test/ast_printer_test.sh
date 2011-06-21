#!/bin/sh

declare -i error=0

./ast_printer -f $srcdir/bashast/features_script/features.sh\
    | diff -u $srcdir/bashast/features_script/features.sh.ast -
error+=$?

./ast_printer -t libbash.tokens -f $srcdir/bashast/features_script/features.sh\
    | diff -u $srcdir/bashast/features_script/features.sh.tokens -
error+=$?

./ast_printer -w libbashWalker.tokens -f $srcdir/bashast/features_script/features.sh\
    | diff -u $srcdir/bashast/features_script/features.sh.walker.tokens -
error+=$?

./ast_printer -t libbash.tokens -f $srcdir/scripts/illegal_script.sh\
    | diff -u $srcdir/scripts/illegal_script.sh.tokens -
error+=$?

./ast_printer -f $srcdir/bashast/features_script/illegal_script.sh 2 > /dev/null
if [[ $? == 0 ]]
then
    error+=1
fi

./ast_printer -e "case" 2 > /dev/null
if [[ $? == 0 ]]
then
    error+=1
fi

exit $error
