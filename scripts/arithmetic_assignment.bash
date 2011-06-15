value="$((100))"
FOO001="$((value*=10))"
FOO002="$((value/=10))"
FOO003="$((value%=9))"
FOO004="$((value+=10))"
FOO005="$((value-=10))"
FOO006="$((value<<=2))"
FOO007="$((value>>=2))"
FOO008="$((value&=10))"
FOO009="$((value^=5))"
FOO010="$((value|=10))"
FOO011=("CREATED" 2)
FOO012="$((${FOO011[0]}=10))"
FOO013="$((3!=5))"
value=100
let "value=${value}"
a=c
b=d
c=1
d=2
e="1+2"
echo $(($a + $b))
echo $(($a + $e))

