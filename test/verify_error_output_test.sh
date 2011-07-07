#!/bin/sh

illegal="${srcdir}/scripts/illegal_script.sh"
output=$(./variable_printer "$illegal" 2>&1)
[[ $output == "${illegal}(1)  : error 10 : Missing token, at offset 3"* ]]
