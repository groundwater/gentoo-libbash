#!/bin/bash

declare -i error=0
result=$(mktemp)

for script in $@
do
    srcdir=$srcdir ./bash $script > $result 2>&1
    srcdir=$srcdir bash +O cmdhist \
                        +O force_fignore \
                        +O hostcomplete \
                        +O interactive_comments \
                        +O progcomp \
                        +O promptvars \
                        +O sourcepath \
                        $script | diff -u $result -
    error+=$?
done

rm -rf $result
rm -rf scripts/input_output_test
exit $error
