#!/bin/sh

source /etc/make.conf

outputdir=${1:-$(mktemp -d)}

if [ -d $outputdir && -w $outputdir]
then
    echo "$outputdir is not writable"
fi

for category_dir in ${PORTDIR:-/usr/portage}/*
do
    category=${category_dir##*\/}
    mkdir $outputdir/$category
    for file in ${category_dir}/*/*.ebuild
    do
        filename=${file##*\/}
        filename=${filename/.ebuild}
        metadata_file="$outputdir/$category/$filename"
        echo "Generating metadata of $category/$filename to $metadata_file"
        LD_LIBRARY_PATH=${srcdir:-..}/.libs ${srcdir:-..}/.libs/metadata_generator $file > $metadata_file
    done
done
