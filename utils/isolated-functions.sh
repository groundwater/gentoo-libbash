die(){
    echo "Die is called: $*"
}

has() {
    hasq $*
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
        return 1
    fi   
    eval $__export_funcs_var+=\" $*\"
}

use() {
    echo "use shouldn't be called"
    return 1
}

useq() {
    echo "useq shouldn't be called"
    return 1
}

use_with() {
    echo "use_with shouldn't be called"
    return 1
}

use_enable() {
    echo "use_enable shouldn't be called"
    return 1
}

eerror() {
    echo "eerror: $*"
}

debug-print() {
    echo "debug-print: $*"
}

ewarn() {
    echo "ewarn: $*"
}
