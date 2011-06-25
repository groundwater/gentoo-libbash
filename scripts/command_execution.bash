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
true && 
    echo "right"
false && echo "wrong"
! true && echo "wrong"
false || echo "right"
true || 
    echo "wrong"
echo "end"
: ${DEFAULTED:="yes"}
FOO="abc" echo "command environment"
export FOO003=1 FOO004=abc FOO005=(1 2 3) FOO002
export foo
abc=1 export foo
echo "hi" > /dev/null

function unset_inner()
{
    local FOO006=3 
    unset FOO006 FOO007
}
function unset_outer()
{
    local FOO006=1 FOO007=2
    unset_inner
    echo "FOO006=$FOO006 in unset_outer"
    echo "FOO007=$FOO007 in unset_outer"
    unset FOO006
    echo "FOO006=$FOO006 in unset_outer"
}
unset_outer
echo "FOO006=$FOO006 in global"
FOO006=0
echo "FOO006=$FOO006 in global"
unset FOO006
echo "FOO006=$FOO006 in global"
declare -F unset_outer
unset -f unset_outer
declare -F unset_outer
echo '$FOO006 "abc" $(( 1 + 2 )) $(echo hi) ...'
echo "abc $(echo def) ghi"
FOO008="abc $(echo def) ghi"
eval "FOO009=10"
eval "echo abc" "def" "xyz"
shopt -s extglob
shopt -p
printf "%s %s\n" abc def
printf "%s %s\n" $FOO001, def
((FOO010=1))
