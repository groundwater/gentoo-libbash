#!/bin/sh

num_of_ebuild_files=$(grep 'scripts/.*.ebuild' $srcdir/Makefile.am | wc -l)
[[ $num_of_ebuild_files == $(ls $srcdir/scripts/*.ebuild* | wc -l) ]]
exit $?
