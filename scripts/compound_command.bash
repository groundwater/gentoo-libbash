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
