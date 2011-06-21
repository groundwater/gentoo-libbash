#!/bin/sh

illegal="${srcdir}/scripts/illegal_script.sh"
output=$(./variable_printer "$illegal" 2>&1)
[[ $output == "${illegal}(1)  : error 3 : 120:1: command_atom : ( compound_command | function | simple_command );, at offset 3"* ]]
