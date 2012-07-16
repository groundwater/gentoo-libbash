EAPI="1"
echo $EAPI
DESCRIPTION="SunPinyin is a SLM (Statistical Language Model) based IME"
echo $DESCRIPTION
HOMEPAGE="http://sunpinyin.googlecode.com"
echo $HOMEPAGE
SRC_URI="http://open-gram.googlecode.com/files/dict.utf8.tar.bz2
echo $SRC_URI
		http://open-gram.googlecode.com/files/lm_sc.t3g.arpa.tar.bz2"
LICENSE="LGPL-2.1 CDDL"
echo $LICENSE
SLOT="0"
echo $SLOT
KEYWORDS="~amd64 ~x86"
echo $KEYWORDS
IUSE=""
echo $IUSE
RDEPEND="dev-db/sqlite:3"
echo $RDEPEND
declare RDEPEND
echo $RDEPEND
DEPEND="${RDEPEND}
echo $DEPEND
		dev-util/pkgconfig"
MY_PATCH=ldflags.patch
echo $MY_PATCH
PATCH=("1.patch" 2.patch)
echo ${PATCH[@]}
ARRAY01=(1 2 3 [5]=4 5)
echo ${ARRAY01[@]}
declare ARRAY02
ARRAY02=(1 2 3)
echo ${ARRAY02[@]}
ARRAY02[2]=4
ARRAY02[3]=5
echo ${ARRAY02[@]}
ARRAY02[3,2]=4
echo ${ARRAY02[@]}
echo ${ARRAY02[2]}
echo ${ARRAY02[3]}
EMPTY=
echo $EMPTY
PARTIAL[5]=5
echo ${PARTIAL[@]}
ARRAY_LAST=${ARRAY01[6]}
echo $ARRAY_LAST
EMPTY_ARRAY=()
echo ${EMPTY_ARRAY[@]}
ARRAY03=(1 2 3)
echo ${ARRAY03[@]}
ARRAY03[0]=
echo ${ARRAY03[@]}
ARRAY04=(1 2 3)
echo ${ARRAY04[@]}
# The following one is equivalent to ARRAY04[0]=
ARRAY04=
echo ${ARRAY04[@]}
ARRAY05=(1 2 3 4 5)
echo ${ARRAY05[@]}
ARRAY06=${ARRAY05[@]}
echo ${ARRAY06[@]}
ARRAY07=${ARRAY05[*]}
echo ${ARRAY07[@]}
ARRAY08="${ARRAY05[@]}"
echo ${ARRAY08[@]}
ARRAY09="${ARRAY05[*]}"
echo ${ARRAY09[@]}
IFS=";,:"
echo "$IFS"
ARRAY10="${ARRAY05[*]}"
echo ${ARRAY10[@]}
FOO001="networkmanager"
echo $FOO001
FOO002="0.8.2"
echo $FOO002
FOO003=${FOO001}-${FOO002}
echo $FOO003
FOO004=$?
echo $FOO004
echo $FOO004
FOO005=abc
echo $FOO005
FOO005+=def
echo $FOO005
function foo() {
    local -i foo=1
    local -a bar=(1 2 3)
    eval local foobar=23
    echo $foo
    echo ${bar[@]}
    echo foobar
}
foo
bar=@
echo $bar
echo $foobar
ARRAY11=(1 2 3 [10]=15)
ARRAY11+=(1 [15]=20)
echo ${ARRAY11[@]}
ARRAY12+=(4 5 6)
echo ${ARRAY12[@]}
ARRAY13=()
ARRAY13+=(4 5 6)
echo ${ARRAY13[@]}
declare num=42
echo $num
unset num
echo $num
