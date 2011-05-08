for file in " foo bar "
do
    echo $file
done

for foo in abc def  ghi
do
    echo $foo
done

for (( i=1; i<4; ++i ))
do
    echo $i
done

for ((;i<10;))
do
    echo $((++i))
done

for ((;i<0;))
do
    echo "Shouldn't print this"
done

i=0;
while [ $i != 4 ]
do
    i=$(( i + 1 ))
    echo $i
done

while [ $i \< 0 ]
do
    echo "Shouldn't print this"
done

i=0;
until [ $i == 4 ]
do
    i=$(( i + 1 ))
    echo $i
done

until [ $i \> 0 ]
do
    echo "Shouldn't print this"
done

a=1
b=2
if [ $a == $b ]
then
    echo "Shouldn't print this"
fi
if [ $a != $b ]
then
    echo $a
fi

if [ $a == $b ]
then
    echo "Shouldn't print this"
elif [ $a != $b ]
then
    echo $b
fi

if [ $a == $b ]
then
    echo "Shouldn't print this"
else
    echo $a
fi

if [ $a == $b ]
then
    echo "Shouldn't print this"
elif [ $a == $b ]
then
    echo "Shouldn't print this"
else
    echo $a
fi

target=123
case $target in
    1.3)
        echo "Shouldn't print this"
        ;;
    \d+)
        echo "Shouldn't print this"
        ;;
    456|1?*|789)
        echo yep
        ;;
    123)
        echo "Shouldn't print this"
        ;;
esac
case $target in
    bcd)
        echo "Shouldn't print this"
        ;;
    abc)
        echo "Shouldn't print this"
        ;;
    *)
        echo "default"
        ;;
esac
echo "case end"
