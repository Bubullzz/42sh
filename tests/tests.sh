#!/bin/sh

if [ -z $MODE ]; then MODE=42
fi

#------------ COLOR ------------#
RED="\e[0;31m"
MRED="\e[1;31m"
DRED="\e[2;31m"
GRN="\e[0;32m"
MGRN="\e[1;32m"
YEL="\e[0;33m"
MYEL="\e[1;33m"
BLU="\e[0;34m"
MBLU="\e[1;34m"
PUR="\e[0;35m"
MPUR="\e[1;35m"
DPUR="\e[2;35m"
CYA="\e[0;36m"
MCYA="\e[1;36m"
DCYA="\e[2;36m"
WHI="\e[0;37m"
MWHI="\e[1;37m"
GRE="\e[2;37m"


#--------------- PWD ---------------#
ORI_PWD=$PWD

#------------ PASS|FAIL ------------#
TOTAL_RUN=0
TOTAL_FAIL=0

#------------ FILE OUT ------------#
reff_out=/tmp/.reff_file_out
reff_err=/tmp/.reff_file_err
myf_out=/tmp/.myf_out
myf_err=/tmp/.myf_err


#------------- CD TEST -------------#
run_cdtest() {
    # If MODE=42 then all usage
    if [ $MODE -eq 42 ] && [ $# -eq 1 ]; then
        run_test $1 1
    elif [ $# -eq 2 ]; then
        run_test $1 1 2
    fi

    # Check if path exists
    [ -e $1 ] || echo "${RED}Missing test file $1\n" 1>&2

    success=true
    TOTAL_RUN=$((TOTAL_RUN + 1))

    if [ $# -eq 1 ];
    then
        echo -ne "${GRE}FILE.SH: ${WHI}"
    elif [ $# -eq 2 ]; then
        echo -ne "${GRE}STDIN: ${WHI}"
    else
        echo -ne "${GRE}STRING: ${WHI}"
    fi

    echo -ne "${MBLU}_" && printf "%s" $1 | cut -d '.' -f 1 | tr "\n" "_" && echo -ne "${WHI}\t"

    ###                                 ###
    #       CALL FILE.TEST AND REF        #
    ###                                 ###

    if [ $# -eq 1 ];
    then
        bash --posix "$1" > $reff_out 2> $reff_err;
    elif [ $# -eq 2 ]; then
        bash --posix < "$1" > $reff_out 2> $reff_err
    else
        bash --posix -c "$(cat "$1")" > $reff_out 2> $reff_err;
    fi
    REF_CODE=$?
    REF_PWD=$PWD

    cd $ORI_PWD

    if [ $# -eq 1 ];
    then
        ../../src/42sh "$1" > $myf_out 2> $myf_err;
    elif [ $# -eq 2 ]; then
        ../../src/42sh < "$1" > $myf_out 2> $myf_err
    else
        ../../src/42sh -c "$(cat "$1")" > $myf_out 2> $myf_err
    fi
    MY_CODE=$?
    MY_PWD=$PWD


    diff --color=always -u $reff_out $myf_out > $1.diff
    DIFF_CODE=$?

    cd $ORI_PWD

    ###                                 ###
    #               RETURN                #
    ###                                 ###

    if [ $MY_PWD != $REF_PWD ]; then
        success=false
    fi

    echo -ne "${WHI}RETURN: "

    # Bad return code
    if [ $REF_CODE -ne $MY_CODE ];
    then
        echo -ne "${MRED}:( ${GRE}> $REF_CODE != $MY_CODE${WHI}\t"
        success=false
    else
        echo -ne "${MGRN}:D ${GRE}> $REF_CODE${WHI}\t"
    fi


    echo -ne "${WHI}STDOUT: "

    # Bad stdout
    if [ $DIFF_CODE -ne 0 ];
    then
        echo -ne "${MRED}:( ${WHI}\t"
        success=false
    else
        echo -ne "${MGRN}:D ${WHI}\t"
    fi


    echo -ne "${WHI}STDERR: "

    # Bad stderr
    if { [ -s $reff_err ] && [ ! -s $myf_err ]; } ||
       { [ ! -s $reff_err ] && [ -s $myf_err ]; };
    then
        echo -ne "${MRED}:( ${WHI}\t"
        success=false
    else
        echo -ne "${MGRN}:D ${WHI}\t"
    fi


    ###                                 ###
    #            FINAL CHECK              #
    ###                                 ###

    # Check SEGV
    if [ -s $myf_err ] && [ $(grep -Ei ".* [SEGV | AddressSanitizer] .*" $myf_err | wc -c) -ne 0 ]
    then
        echo -ne "${MRED}=> SEGV$WHI"
        success=false
    fi

    # Check Success
    if $success;
    then
        echo -e "${MGRN}=> PASSED\n$WHI"
    else
        echo
        # Check if exists && size > 0
        if [ -s "$(realpath $1.diff)" ];
        then
            if [ $(cat $(realpath $1.diff) | tail -n +4 | wc -c) -gt 100 ]; then
                tail -n +4 $(realpath $1.diff) | head -c 100;
                echo -ne "${GRE}[...]${WHI}\n"
            else
                tail -n +4 $(realpath $1.diff);
            fi;
        fi
        echo
        TOTAL_FAIL=$((TOTAL_FAIL + 1))
    fi

    # rm tmp
    rm -f $1.diff $reff_err $reff_out $myf_err $myf_out
}

#----------- NORMAL TEST -----------#
run_test() {
    # If MODE=42 then all usage
    if [ $MODE -eq 42 ] && [ $# -eq 1 ]; then
        run_test $1 1
    elif [ $# -eq 2 ]; then
        run_test $1 1 2
    fi

    # Check if path exists
    [ -e $1 ] || echo "${RED}Missing test file $1\n" 1>&2

    success=true
    TOTAL_RUN=$((TOTAL_RUN + 1))

    if [ $# -eq 1 ];
    then
        echo -ne "${GRE}FILE.SH: ${WHI}"
    elif [ $# -eq 2 ]; then
        echo -ne "${GRE}STDIN: ${WHI}"
    else
        echo -ne "${GRE}STRING: ${WHI}"
    fi

    echo -ne "${MBLU}_" && printf "%s" $1 | cut -d '.' -f 1 | tr "\n" "_" && echo -ne "${WHI}\t"

    ###                                 ###
    #       CALL FILE.TEST AND REF        #
    ###                                 ###

    if [ $# -eq 1 ];
    then
        bash --posix "$1" > $reff_out 2> $reff_err;
    elif [ $# -eq 2 ]; then
        bash --posix < "$1" > $reff_out 2> $reff_err
    else
        bash --posix -c "$(cat "$1")" > $reff_out 2> $reff_err;
    fi
    REF_CODE=$?

    if [ $# -eq 1 ];
    then
        ../../src/42sh "$1" > $myf_out 2> $myf_err;
    elif [ $# -eq 2 ]; then
        ../../src/42sh < "$1" > $myf_out 2> $myf_err
    else
        ../../src/42sh -c "$(cat "$1")" > $myf_out 2> $myf_err
    fi
    MY_CODE=$?

    diff --color=always -u $reff_out $myf_out > $1.diff
    DIFF_CODE=$?


    ###                                 ###
    #               RETURN                #
    ###                                 ###

    echo -ne "${WHI}RETURN: "

    # Bad return code
    if [ $REF_CODE -ne $MY_CODE ];
    then
        echo -ne "${MRED}:( ${GRE}> $REF_CODE != $MY_CODE${WHI}\t"
        success=false
    else
        echo -ne "${MGRN}:D ${GRE}> $REF_CODE${WHI}\t"
    fi


    echo -ne "${WHI}STDOUT: "

    # Bad stdout
    if [ $DIFF_CODE -ne 0 ];
    then
        echo -ne "${MRED}:( ${WHI}\t"
        success=false
    else
        echo -ne "${MGRN}:D ${WHI}\t"
    fi


    echo -ne "${WHI}STDERR: "

    # Bad stderr
    if { [ -s $reff_err ] && [ ! -s $myf_err ]; } ||
       { [ ! -s $reff_err ] && [ -s $myf_err ]; };
    then
        echo -ne "${MRED}:( ${WHI}\t"
        success=false
    else
        echo -ne "${MGRN}:D ${WHI}\t"
    fi


    ###                                 ###
    #            FINAL CHECK              #
    ###                                 ###

    # Check SEGV
    if [ -s $myf_err ] && [ $(grep -Ei ".* [SEGV | AddressSanitizer] .*" $myf_err | wc -c) -ne 0 ]
    then
        echo -ne "${MRED}=> SEGV$WHI"
        success=false
    fi

    # Check Success
    if $success;
    then
        echo -e "${MGRN}=> PASSED\n$WHI"
    else
        echo
        # Check if exists && size > 0
        if [ -s "$(realpath $1.diff)" ];
        then
            if [ $(cat $(realpath $1.diff) | tail -n +4 | wc -c) -gt 100 ]; then
                tail -n +4 $(realpath $1.diff) | head -c 100;
                echo -ne "${GRE}[...]${WHI}\n"
            else
                tail -n +4 $(realpath $1.diff);
            fi;
        fi
        echo
        TOTAL_FAIL=$((TOTAL_FAIL + 1))
    fi

    rm -f $1.diff $reff_err $reff_out $myf_err $myf_out
    # rm tmp
}

#------------ RUN FILE ------------#
run_category() {
    cd $1
    source ./tests.sh
    cd ->/dev/null
}

run_testsuite() {
    for category in $@;
    do
        [ $category = "." ] && continue
        echo -ne "${DPUR}[${MPUR}===${MBLU}====${MCYA}=====${WHI}${MYEL} "
        printf "%s" "${category#*/}" | tr '[:lower:]' '[:upper:]'
        echo -ne " ${DCYA}=====${MBLU}====${MPUR}===${DPUR}]${WHI}\n"

        run_category $category
        echo
    done
}

source ./deldif.sh
run_testsuite $(find . -type d)

PERCENT_SUCCESS=$(((TOTAL_RUN - TOTAL_FAIL) * 100 / TOTAL_RUN))

echo -e "${DRED}[${WHI}${MRED}====${MYEL}=====${MGRN}===========${MYEL}=====${MRED}====${DRED}]${WHI}"

if [ $PERCENT_SUCCESS -eq 100 ];
then
    printf "%11s"
    echo -e "${MGRN}$PERCENT_SUCCESS % 100${MWHI}"
elif [ $PERCENT_SUCCESS -ge 50 ];
then
    printf "%11s"
    echo -e "${MYEL}$PERCENT_SUCCESS % 100${MWHI}"
else
    printf "%11s"
    echo -e "${MRED}$PERCENT_SUCCESS % 100${MWHI}"
fi
printf "%9s " "$((TOTAL_RUN - TOTAL_FAIL))"
echo -e "sur $TOTAL_RUN tests"
echo -e "${DRED}[${WHI}${MRED}====${MYEL}=====${MGRN}===========${MYEL}=====${MRED}====${DRED}]${WHI}"
