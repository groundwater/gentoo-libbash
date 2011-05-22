function hi()
{
	echo "Hello World"
}
hi
echo hello world
true
false
FOO001=$(echo hello)
FOO002=$(hi)
true && echo "right"
false && echo "wrong"
! true && echo "wrong"
false || echo "right"
true || echo "wrong"
echo "end"
: ${DEFAULTED:="yes"}
FOO="abc" echo "command environment"
true > /dev/null
