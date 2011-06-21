#!/bin/bash

num_of_ebuild_files=$(grep 'scripts/.*.bash' $srcdir/Makefile.am | wc -l)
[[ $num_of_ebuild_files == $(ls $srcdir/scripts/*.bash* | wc -l) ]]
exit $?
