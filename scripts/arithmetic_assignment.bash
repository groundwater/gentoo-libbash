value="$((100))"
echo "$((value*=10))"
echo "$((value/=10))"
echo "$((value%=9))"
echo "$((value+=10))"
echo "$((value-=10))"
echo "$((value<<=2))"
echo "$((value>>=2))"
echo "$((value&=10))"
echo "$((value^=5))"
echo "$((value|=10))"
FOO011=("CREATED" 2)
echo "$((${FOO011[0]}=10))"
echo "$((3!=5))"
value=100
echo $value
let "value=${value}"
echo $value
a=c
b=d
c=1
d=2
e="1+2"
echo $(($a + $b))
echo $(($a + $e))
(( a=1 ))
echo $a
(( a=(b=2,3) ))
echo $a $b
