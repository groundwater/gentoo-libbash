EAPI=3

src_unpack() {
	EAPI=$((EAPI+1))
}

src_unpack
src_unpack

MY_PV=2.0.3-r1

nested_func_with_args() {
	ARG6=$1
	ARG7=$3
}

func_with_args() {
	ARG1=$1
	ARG2=$2
	ARG3=$3
	ARG4=$4
	ARG5=$5
	nested_func_with_args $4
}
FOO001="4		5"
ARRAY=(1 2 3)
func_with_args ${ARRAY[@]} $FOO001
func_with_args 100 $ARG2 $ARG3 $ARG4

func-with-return()
{
    return 1
    NOT_EXIST=1
}
func-with-return
RETURN_STATUS=$?
func_with-return2()
{
    true
    return
    NOT_EXIST=1
}
func_with-return2
RETURN_STATUS2=$?

func_nested1() {
    echo $foo_nested ${bar_nested[0]} $localbar
}
func_nested2() {
    local foo_nested=hi bar_nested=(1 2 3) localbar
    localbar=1
    echo ${bar_nested[@]}
    echo ${not_exist[@]}
    echo ${#bar_nested[@]}
    echo ${#non_exist[@]}
    echo ${#foo_nested}
    echo ${#non_exist}
    echo ${foo_nested:-wrong}
    echo ${non_exist:-right}
    func_nested1
}
func_nested2
echo $localbar

let() {
    echo "overloaded let"
}
let "1 + 2"
func_positional_args() {
    IFS="abc" echo "$*"
    IFS="abc" echo $*
    IFS=abc
    echo "$*"
    echo $*
}
func_positional_args 1 2 3
IFS=" \t\n"

nested_func_override_positional_args() {
  echo $@
  set -- 40 50 60
  echo $@
}
func_override_positional_args() {
  echo $@
  nested_func_override_positional_args 4 5 6
  set -- 10 20 30
  echo $@
}
set -- foo bar
func_override_positional_args 1 2 3
echo $@

if true; then
    function_in_compound_statement() {
        echo "function_in_compound_statement"
    }
fi
if false; then
    function_in_compound_statement() {
        echo "I should not get called"
    }
fi
function_in_compound_statement

function shift_test() {
    shift
    echo $1
}

shift_test 1 2
test-flag-CC() {
    function="$1"
    echo "'${function}' function is not defined"
    echo "CC" "$1";
}
test-flag-CC abc
