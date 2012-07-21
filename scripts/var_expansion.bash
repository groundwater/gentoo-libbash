shopt -s extglob
ARRAY=(hi hello 1 2 3)
EAPI="3"
echo "$((EAPI+1))"
echo "${EAPI:-hello}"
echo "${EAPI3:-hello}"
echo 123
echo $EAPI
echo $(( 1+1 ))
echo ${EAPI:=hello}
echo ${FOO008:=hello world}
FOO009=${EAPI:+hello}
echo ${NOT_EXIST:+hello}
echo ${FOO009:0}
echo ${FOO009:2}
echo ${FOO009: -2}
echo ${FOO009:100}
echo ${FOO009: -100}
echo ${FOO009:(-5 + 5)}
echo ${FOO009:(-5 + 5,0)}
echo ${FOO009:(-5 + 5,1)}
echo ${NOT_EXIST:0}
echo ${FOO009:0:2}
echo ${FOO009:2:2}
echo ${FOO009: -2:2}
echo ${FOO009:2:100}
echo ${FOO009: -2:100}
echo ${NOT_EXIST:0:2}
echo ${#FOO009}
echo ${#NOT_EXIST}
echo "${ARRAY[0]:-hello}"
echo "${ARRAY[5]:-hello}"
echo "${ARRAY2[0]:=hello}"
echo "${ARRAY2[0]:=hi}"
echo "${ARRAY2[0]:+hi}"
echo "${ARRAY2[1]:+hi}"
echo "${ARRAY[1]:1}"
echo "${ARRAY[1]:1:3}"
echo "${#ARRAY[0]}"
echo "${#ARRAY[@]}"
echo "${#ARRAY[*]}"
echo "${#ARRAY}"
FOO039="Hello World"
echo ${FOO039/nothing/nothing}
echo ${FOO039/o W/ow}
echo ${FOO039//o/e}
echo ${FOO039/#He/he}
echo ${FOO039/#he/he}
echo ${FOO039/%rld/rlD}
echo ${FOO039/%rlD/rlD}
echo ${FOO039/o W}
echo ${FOO039//o}
echo ${FOO039/#He}
echo ${FOO039/#he}
echo ${FOO039/%rld}
echo ${FOO039/%rlD}
echo ${FOO039/aaaaaaaaaaaa}
echo ${FOO039#hello}
echo ${FOO039##hello}
echo ${FOO039#Hello}
echo ${FOO039##Hello}
echo ${FOO039%world}
echo ${FOO039%%world}
echo ${FOO039%World}
echo ${FOO039%%World}
echo ${FOO039#Hel*}
echo ${FOO039##Hel*}
echo ${FOO039%*rld}
echo ${FOO039%%*rld}
echo ${FOO039/l/r}
echo ${FOO039//l/r}
echo ${FOO039/#He/he}
echo ${FOO039/#ello/i}
echo ${FOO039/%ld/d}
echo ${FOO039/%rl/r}
echo ${FOO039/+(l)/}
echo ${FOO039/+(l|e)}
echo ${FOO039/*(l)}
echo ${FOO039/?(l)}
echo ${FOO039/@([a-c]|[k-m])}
echo ${FOO039//@([a-c]|[k-m])}
FOO40=("foo bar" foo bar foobar)
FOO40=("foo bar foo" bar foo foobar)
echo ${FOO40[@]/foo/poo}
echo ${FOO40[@]//foo/poo}
echo ${FOO40[@]#foo}
echo ${FOO40[@]##f*o}
echo ${FOO40[@]%foo}
echo ${FOO40[@]%%f*o}
echo ${FOO40[@]/#foo}
echo ${FOO40[@]/%foo}
echo ${FOO40[@]}
target="abc123abc"
echo "${target##+(ab[c])*([[:digit:]])}"
function positional_parameter_test(){
    echo $*
    echo ${*}
    echo ${*:1}
    echo ${*:1:2}
    echo ${*: -1}
    echo ${*: -2:5}
    echo ${*:0}
    echo $@
    echo ${@}
    echo ${@:1}
    echo ${@:1:2}
    echo ${@: -1}
    echo ${@: -2:5}
    echo ${@:0}
    echo $#
}
positional_parameter_test 1 2 3 4 5
target="abc*abc"
echo ${target/*}
echo ${target/'*'}
echo ${target/"*"}
: ${FOO089:=}
ARRAY=(1 2 3 4 5)
echo ${ARRAY[@]:1}
echo ${ARRAY[@]:1:3}
echo $#
echo a{b,c}d
echo a{a,bc}d{e,}f
echo a{ab,cd}d{ef,gh}
foo=
unset bar
echo ${foo-abc}
foo=
unset bar
echo ${foo+abc}
foo=
unset bar
echo ${foo=abc}
foo=
unset bar
echo ${bar-abc}
foo=
unset bar
echo ${bar+abc}
foo=
unset bar
echo ${bar=abc}

search_paths="a%#b"
root=123
echo "${search_paths/%/${root}}"
echo "${search_paths/#/${root}}"

[[ " ${FUNCNAME[@]:2} " =~ " "(_python_final_sanity_checks|python_execute_function|python_mod_optimize|python_mod_cleanup)" " ]]
[[ "$(declare -p PYTHON_SANITY_CHECKS_EXECUTED)" != "declare -- PYTHON_SANITY_CHECKS_EXECUTED="* || " ${FUNCNAME[@]:1} " =~ " "(python_set_active_version|python_pkg_setup)" " && -z "${PYTHON_SKIP_SANITY_CHECKS}" ]]
[[ " ${FUNCNAME[@]:1} " =~ " "(python_set_active_version|python_pkg_setup)" " ]]
[[ "$(echo "2p" "${file}")" =~ ^"# Gentoo '".*"' wrapper script generated by python_generate_wrapper_scripts()"$ ]]

PYTHON_DEPEND="2:2.6"
version_components_group_regex="(2|3|\*)(:([[:digit:]]+\.[[:digit:]]+)?(:([[:digit:]]+\.[[:digit:]]+)?)?)?"
version_components_groups="${PYTHON_DEPEND}"
[[ "${version_components_groups}" =~ ^((\!)?[[:alnum:]_-]+\?\ )?${version_components_group_regex}(\ ${version_components_group_regex})?$ ]] && echo true
[[ "${version_components_groups}" =~ ("*".*" "|" *"|^2.*\ (2|\*)|^3.*\ (3|\*)) ]] && echo true
