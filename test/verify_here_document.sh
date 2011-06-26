#!/bin/bash

./ast_printer -f $srcdir/scripts/here_document.sh | diff -u $srcdir/scripts/here_document.ast -
