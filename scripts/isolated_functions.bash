source $srcdir/utils/isolated-functions.sh
die "something wrong"
has 123 456 123 456 && echo true
has 123 456 13 456 && echo false
hasq 123 123 456
EXPORT_FUNCTIONS
echo hi
use
useq
use_with
use_enable
eerror foo
debug-print foo
ewarn foo
debug-print-function foo bar
