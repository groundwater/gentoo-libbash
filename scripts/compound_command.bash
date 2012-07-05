for file in " foo bar "
do
    echo $file
done

for foo in abc def  ghi
do
    echo $foo
done

bar=
# We behave differently from bash with "for foo in ''"
for foo in $bar
do
    echo "Shouldn't print this"
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

for file in foo bar
do
    if [[ $file == "foo" ]]; then
        continue
    fi
    echo $file
done

for file in foo bar
do
    if [[ $file == "bar" ]]; then
        continue
    fi
    echo $file
done

for file in foo bar
do
    if [[ $file == "foo" ]]; then
        break
    fi
    echo $file
done

for file in foo bar
do
    if [[ $file == "bar" ]]; then
        break
    fi
    echo $file
done

for outer in 1 2 3
do
    for file in foo bar
    do
        if [[ $file == "foo" && $outer == 1 ]]; then
            continue 2
        fi
        echo "$outer $file"
    done
done

for outer in 1 2 3
do
    for file in foo bar
    do
        if [[ $file == "foo" && $outer == 1 ]]; then
            break 2
        fi
        echo "$outer $file"
    done
done

function positional_for()
{
    for arg; do
      echo $arg
    done

    for arg do
      echo $arg
    done
}
positional_for foo bar

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

i=0
while [ $i != 4 ]
do
    i=$(( i + 1 ))
    if [[ $i == 1 ]]; then
        continue
    fi
    echo $i
done

i=0
while [ $i != 4 ]
do
    i=$(( i + 1 ))
    if [[ $i == 1 ]]; then
        break
    fi
    echo $i
done

i=0
j=1
while [ $i != 4 ]
do
    i=$(( i + 1 ))
    
    while [ $j == 1 ]
    do
        if [[ $i == 1 ]]; then
            continue 2
        fi
        echo $i
        let ++j
    done
done

i=0
j=1
while [ $i != 4 ]
do
    i=$(( i + 1 ))
    
    while [ $j == 1 ]
    do
        if [[ $i == 1 ]]; then
            break 2
        fi
        echo $i
        let ++j
    done
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

i=0
until [ $i == 4 ]
do
    i=$(( i + 1 ))
    if [[ $i == 1 ]]; then
        continue
    fi
    echo $i
done

i=0
until [ $i == 4 ]
do
    i=$(( i + 1 ))
    if [[ $i == 1 ]]; then
        break
    fi
    echo $i
done

i=0
j=1
until [ $i == 4 ]
do
    i=$(( i + 1 ))
    
    while [ $j == 1 ]
    do
        if [[ $i == 1 ]]; then
            continue 2
        fi
        echo $i
        let ++j
    done
done

i=0
j=1
until [ $i == 4 ]
do
    i=$(( i + 1 ))
    
    while [ $j == 1 ]
    do
        if [[ $i == 1 ]]; then
            break 2
        fi
        echo $i
        let ++j
    done
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
target=xyz
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
case $target in
    abc|bcd|xyz)
        ;;
    *)
        echo "Shouldn't print this"
        ;;
esac
target=a
case $target in
    [def])
        echo "Shouldn't print this"
        ;;
    [abc])
        echo yep
        ;;
esac
case $target in
    [def])
        echo "Shouldn't print this"
        ;;
    [a])
        echo yep
        ;;
esac
case $target in
    [!abc])
        echo "Shouldn't print this"
        ;;
    [!def])
        echo yep
        ;;
esac
case $target in
    [d-z])
        echo "Shouldn't print this"
        ;;
    [a-c])
        echo yep
        ;;
esac
case $target in
    [!a-c])
        echo "Shouldn't print this"
        ;;
    [!d-z])
        echo yep
        ;;
esac
target=bar
case $target in
    a[a-cx-z]r)
        echo "Shouldn't print this"
        ;;
    b[!d-fx-z]r)
        echo yep
        ;;
esac
target="a"
case $target in
    [[:alnum:][:alpha:][:ascii:][:blank:][:cntrl:][:digit:][:graph:][:lower:][:print:][:punct:][:space:][:upper:][:word:][:xdigit:]])
        echo yep
        ;;
esac
target="a"
case $target in
    [[:ascii:]])
        echo yep
        ;;
esac
target="_"
case $target in
    [[:word:]])
        echo yep
        ;;
esac
echo "case end"

for((i=1,j=2;i!=2&&j!=4;++i))
do
    echo $i $j
done
python_versions=(2.6 2.7 3.0 3.1)
for ((i = "${#python_versions[@]}"; i >= 0; i--))
do
    echo $i
done
EAPI="4"
case ${EAPI} in
    0|1)
        echo "Unsupported EAPI=${EAPI} (too old) for ruby-ng.eclass" ;;
    2|3);;
    4)
        # S is no longer automatically assigned when it doesn't exist.
        S="${WORKDIR}"
        ;;  
    *)  
        echo "Unknown EAPI=${EAPI} for ruby-ng.eclass"
esac

if [[   1 == 0 ||
        1 == 0
|| 1 == 0
        || 1 == 0 || # Extra space here
        1 == 1 ]]; then
        echo or
fi

if [[   1 == 1 # Extra space here
        && 1 == 1 && 1 == 1 &&
        1 == 1&&
        1 == 1 ]]; then
        echo and
fi

if [[   1 == 1 &&
        1 == 0 || 1 == 1
        && 1 == 1 ||
        1 == 1 && 1 == 1 ]]; then
        echo and or
fi
