function hi()
{
	echo "Hello World"
}
hi
echo hello world
true
false
FOO001=$(echo hello)
echo $FOO001
FOO002=$(hi)
echo $FOO002
true && 
    echo "right"
false && echo "wrong"
! true && echo "wrong"
false || echo "right"
true || 
    echo "wrong"
echo right1 && echo right2 && false && echo wrong
false ||  echo right3 || echo wrong
true && false || echo right4 && echo right5
echo "end"
: ${DEFAULTED:="yes"}
FOO="abc" echo "command environment"
export FOO003=1 FOO004=abc FOO005=(1 2 3) FOO002
echo $FOO003 $FOO004 $FOO005 $FOO002
export foo
echo $foo
echo "hi" > /dev/null

function unset_inner()
{
    local FOO006=3 
    unset FOO006 FOO007
}
function unset_outer()
{
    local FOO006=1 FOO007=2
    local gjl_${FOO006}="${FOO007}"
    local f version install{{site,vendor}{arch,lib},archlib}
    unset_inner
    echo "FOO006=$FOO006 in unset_outer"
    echo "FOO007=$FOO007 in unset_outer"
    echo "gjl_1=${gjl_1}"
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
echo $FOO008
eval "FOO009=10"
echo $FOO009
eval "echo abc" "def" "xyz"
shopt -s extglob
printf "%s %s\n" abc def
printf "%s %s\n" $FOO001, def
printf "123-%s" 456 789
printf "\n"
printf 'debug-%s ' dvc kbd nla
printf "123-%s"
((FOO010=1))
echo $FOO010
echo "abc #av### ##" # for comment
echo $'abc\tdef\nxyz'
echo -e "\'\"\t\n"
echo 'quotes should be handled correctly'
echo "$(echo 'hi')"
echo "`echo 'hi'`"
array=(`echo 1` `echo 2` 3)
echo ${array[@]}
ech\
o Hello\
 world
echo \`\(\)\$\>\<\`
export SRC_URI="${SRC_URI} http://www.oracle.com/technology/products/berkeley-db/db/update/${MY_PV}/patch.${MY_PV}.${i}"
> /dev/null
(( i=1, j=2)) && echo $i $j
a="ab cd ef"
variable=a
echo "${!variable// /_}"
echo "${!#// /_}"
echo "${!1// /_}"
echo "${!variable// /$'\n'}"
eval abc+=\( \"\$@\" \)
declare MOZILLA_FIVE_HOME="/usr/share/${PN}"
declare foo=23 empty bar=42
echo $MOZILLA_FIVE_HOME $foo $lol $bar
