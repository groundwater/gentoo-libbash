ARRAY=(hi hello 1 2 3)
EAPI="3"
EAPI4="$((EAPI+1))"
FOO001="${EAPI:-hello}"
FOO002="${EAPI3:-hello}"
FOO003=123
FOO004=$EAPI
FOO005=$(( 1+1 ))
FOO006=${EAPI:=hello}
FOO007=${FOO008:=hello}
FOO009=${EAPI:+hello}
FOO010=${NOT_EXIST:+hello}
FOO011=${FOO009:0}
FOO012=${FOO009:2}
FOO013=${FOO009: -2}
FOO014=${FOO009:100}
FOO015=${FOO009: -100}
FOO016=${FOO009:(-5 + 5)}
FOO017=${NOT_EXIST:0}
FOO018=${FOO009:0:2}
FOO019=${FOO009:2:2}
FOO020=${FOO009: -2:2}
FOO021=${FOO009:2:100}
FOO022=${FOO009: -2:100}
FOO023=${NOT_EXIST:0:2}
FOO024=${#FOO009}
FOO025=${#NOT_EXIST}
FOO026="${ARRAY[0]:-hello}"
FOO028="${ARRAY[5]:-hello}"
FOO029="${ARRAY2[0]:=hello}"
FOO030="${ARRAY2[0]:=hi}"
FOO031="${ARRAY2[0]:+hi}"
FOO032="${ARRAY2[1]:+hi}"
FOO033="${ARRAY[1]:1}"
FOO034="${ARRAY[1]:1:3}"
FOO035="${#ARRAY[0]}"
FOO036="${#ARRAY[@]}"
FOO037="${#ARRAY[*]}"
FOO038="${#ARRAY}"
