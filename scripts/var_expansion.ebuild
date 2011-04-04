EAPI="3"
EAPI4="$(($EAPI+1))"
FOO="${EAPI:-hello}"
FOO2="${EAPI3:-hello}"
FOO3=123
FOO4=$EAPI
FOO5=$(( 1+1 ))
