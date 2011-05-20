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

func_with_return()
{
    return 1
    NOT_EXIST=1
}
func_with_return
RETURN_STATUS=$?
func_with_return2()
{
    true
    return
    NOT_EXIST=1
}
func_with_return2
RETURN_STATUS2=$?

func_nested1() {
    echo $foo_nested ${bar_nested[0]}
}
func_nested2() {
    local foo_nested=hi bar_nested=(1 2
    3)
    func_nested1
}
func_nested2
let() {
    echo "overloaded let"
}
let "1 + 2"
