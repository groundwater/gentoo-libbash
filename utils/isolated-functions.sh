die(){
    echo "Die is called. Something went wrong while interpreting"
}

has() {
    hasq "$@"
}

hasq() {
    for item in ${*:2}
    do
        [[ $item == $1 ]] && return 0
    done
    return 1
}

EXPORT_FUNCTIONS() {
    if [ -z "$ECLASS" ]; then 
        die "EXPORT_FUNCTIONS without a defined ECLASS"
    fi   
    $__export_funcs_var="$__export_funcs_var $*" 
}
