#!/bin/sh

illegal="${srcdir}/scripts/illegal_script.sh"
output=$(./variable_printer "$illegal" 2>&1)
[[ $output == "${illegal}(1)  : error 3 : 128:1: command : ( compound_command | function | simple_command );, at offset 3"* ]]
