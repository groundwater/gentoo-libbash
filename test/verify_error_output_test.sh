#!/bin/sh

illegal="${srcdir}/scripts/illegal_script.sh"
output=$(./variable_printer "$illegal" 2>&1)
[[ $output == "${illegal}(1)  : error 5 : Unexpected token, at offset 3"* ]]
