[ a = b ]
echo $? # 1
test a = a
echo $? # 0
#[ ]
#echo $? # 1
[ abc ]
echo $? # 0
[ ! abc ]
echo $? # 1
#[ ! ]
#echo $?
#[ abc -a bcd ]
#echo $?
#[ abc -o bcd ]
#echo $?
test abc == abd
echo $? # 1
[ abc != bcd ]
echo $? # 0
[ abc != abc ]
echo $? # 1
[ abc \> bcd ]
echo $? # 1
[ abc \< bcd ]
echo $? # 0
[[ abc ]] && echo true1
[[ abc < bcd ]] && echo true2
[[ abc > bcd ]] || echo true3
[[ abc != bcd ]] && echo true4
[[ abc = bcd ]] || echo true5
[[ abc == abc ]] && echo true6
[[ ! abc = bcd ]] && echo true7
[[ abc = bcd || abc == abc ]] && echo true8
[[ abc = bcd && abc == abc ]] || echo true9
# abc=bcd is treated as a simple string
[[ abc=bcd && abc == abc ]] || echo wrong
[[ -a "/" ]] && echo "true10"
[[ . -ef . ]] && echo "true11"
[[ 2 -ge 2 ]] && echo "true12"
