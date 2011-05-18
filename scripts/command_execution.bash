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
false || echo "right"
true || echo "wrong"
echo "end"
: ${DEFAULTED:="yes"}
FOO="abc" echo "command environment"
export FOO003=1 FOO004=abc FOO005=(1 2 3) FOO002
abc=1 export foo
