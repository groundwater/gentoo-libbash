#!/bin/bash

source /etc/make.conf

outputdir=${1:-$(mktemp -d)}

if [[ ! (-d $outputdir && -w $outputdir) ]]
then
    echo "$outputdir is not writable"
fi

echo "Generating metadata at $outputdir"

time ./instruo -p portage -n gentoo -o $outputdir 2>$outputdir/error_output

echo "Running diff..."

declare -i error_count=0 total_num=0

for category_dir in ${PORTDIR:-/usr/portage}/*
do
    category=${category_dir##*\/}
    cache_dir=${PORTDIR:-/usr/portage}/metadata/cache/${category}
    if [[ -d $cache_dir ]]
    then
        for path in $cache_dir/*
        do
            filename=${path##*\/}
            diff -u $cache_dir/$file $outputdir/$category/$filename > $outputdir/$category/$filename.diff 2>>$outputdir/error_output
            error_count+=$(($? != 0))
            total_num+=1
        done
    fi
done

echo "$((total_num - error_count)) out of $total_num metadata files are correct. See file.diff for more details."
