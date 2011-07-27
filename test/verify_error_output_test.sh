#!/bin/sh

illegal="${srcdir}/scripts/illegal_script.sh"
output=$(./variable_printer "$illegal" 2>&1)
[[ $output == *"(1)  : error 1 : Unexpected token, at offset"* ]]
