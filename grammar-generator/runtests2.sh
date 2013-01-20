#!/bin/bash

fails=0

function check() {
    if [[ $? == 0 ]] ; then
        echo OK
        echo
    else
        echo FAIL
        echo
        fails=`expr $fails + 1`
    fi
}

function checkfail() {
    if [[ $? != 0 ]] ; then
        echo OK
        echo
    else
        echo FAIL
        echo
        fails=`expr $fails + 1`
    fi
}

# passables
for i in tests/test* ; do
    echo cat -E $i
    cat -E $i
    echo "cat $i | ./generator"
    cat $i | ./generator
    check
done

# failables
for i in tests/fail* ; do
    echo cat -E $i
    cat -E $i
    echo "cat $i | ./generator"
    cat $i | ./generator
    checkfail
done

if [[ $fails != 0 ]] ; then
    echo ====================
    echo Failed $fails tests.
    exit $fails
else
    echo ==============
    echo All tests pass
    exit 0
fi
