#!/bin/bash

./variable_printer $srcdir/scripts/naughty_tests | diff -u $srcdir/scripts/naughty_tests.result -
