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
