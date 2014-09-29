#! /bin/sh
# -*- coding: utf-8 -*-

progname=`basename $0`

if [ "$srcdir" = "" ] ; then
    srcdir=. ;
fi
tooldir=../../tools/src
tmpdir=$srcdir
verbosity=1
debug=
logpass=
logfail=
logverbose=
logfile=$srcdir/$progname.log
number_tests=

compiler=hfst-pmatch2fst
compiler_opts=
runner=hfst-pmatch
runner_opts=

preamble=
testset_descr=
test_descr=
input=
output=
template=

testcount=0
failcount=0
subtestcount=0
subfailcount=0


error () {
    echo "$0: $1" > /dev/stderr
    exit 77
}

warn () {
    echo "$0: Warning: $1" > /dev/stderr
}

getopts_func () {
    _result_var=$1
    shift
    _opts=
    _count=0
    while true; do
	case $1 in
	    --* )
		_opts="$_opts $1"
		_count=$(($_count + 1))
		shift
		;;
	    * )
		break
		;;
	esac
    done
    eval "$_result_var=\$_opts"
    return $_count
}


usage () {
    cat <<EOF
	    cat <<EOF
Usage: $progname [options]
Test the HFST Pmatch tool

Options:
  --help          show this help
  --tmpdir DIR    use DIR for temporary files (default: .)
  --tooldir DIR   use hfst-pmatch2fst and hfst-pmatch found in DIR (default: .)
  --log none|failed|all|verbose
                  log the results of no tests, failed tests, all tests or all
                  tests verbosely (default: none)
  --logfile FILE  write the log output to FILE (default: 
  --verbose       print verbose output of the tests to stdout
  --number-tests  show test and subtest numbers
  --quiet         do not print any output
  --debug         do not remove temporary files
EOF
    exit 0
}

cmdline="$0 ""$@"

# Test if GNU getopt
getopt -T > /dev/null
if [ $? -eq 4 ]; then
    # This requires GNU getopt
    args=`getopt -o "h" -l "help,debug,log:,logfile:,number-tests,quiet,tmpdir:,tooldir:,verbose" -n "$progname" -- "$@"`
    if [ $? -ne 0 ]; then
	exit 1
    fi
    eval set -- "$args"
fi
# If not GNU getopt, arguments of long options must be separated from
# the option string by a space; getopt allows an equals sign.

# Process options
while [ "x$1" != "x" ] ; do
    case "$1" in
	-h | --help )
	    usage
	    ;;
	--debug )
	    debug=1
	    ;;
	--verbose )
	    verbosity=2
	    ;;
	--quiet )
	    verbosity=0
	    ;;
	--tmpdir )
	    shift
	    tmpdir="$1"
	    tmpdir_set=1
	    ;;
	--tooldir )
	    shift
	    tooldir="$1"
	    tooldir_set=1
	    ;;
	--log )
	    shift
	    case $1 in
		all )
		    logpass=1
		    logfail=1
		    ;;
		fail* )
		    logfail=1
		    ;;
		none )
		    logpass=
		    logfail=
		    logverbose=
		    ;;
		verbose )
		    logpass=1
		    logfail=1
		    logverbose=1
		    ;;
		* )
		    warn "Unrecognized argument value \"$1\" for --log"
		    ;;
	    esac
	    ;;
	--logfile )
	    shift
	    logfile="$1"
	    ;;
	--number-tests )
	    number_tests=1
	    ;;
	-- )
	    shift
	    break
	    ;;
	--* )
	    warn "Unrecognized option: $1"
	    ;;
	* )
	    break
	    ;;
    esac
    shift
done

for prog in $compiler $runner; do
    if test ! -x $tooldir/$prog; then
	msg="Executable $prog not found in $tooldir"
	if [ "x$tooldir_set" = "x" ]; then
	    msg="$msg; please specify directory with --tooldir"
	fi
	error "$msg"
    fi
done

file_base=$tmpdir/$progname.$$
pmatch_file=$file_base.pmatch
err_file=$file_base.err
out_file=$file_base.out
expect_out_file=$file_base.expect

if touch $err_file 2> /dev/null; then
    rm $err_file
else
    msg="Cannot create files in temporary directory $tmpdir"
    if [ "x$tmpdir_set" = "x" ]; then
	msg="$msg; please specify directory with --tmpdir"
    fi
    error "$msg"
fi


log () {
    logtype=$1
    shift
    outputlog=
    case $logtype in
	pass )
	    test "x$logpass" = "x" || outputlog=1
	    ;;
	fail )
	    test "x$logfail" = "x" || outputlog=1
	    ;;
	verbose )
	    test "x$logverbose" = "x" || outputlog=1
	    ;;
	meta )
	    if [ "x$logpass" != "x" ] || [ "x$logfail" != "x" ]; then
		outputlog=1
	    fi
	    ;;
    esac
    if [ "x$outputlog" != "x" ]; then
	echo "$@" >> $logfile
    fi
}

echo_log () {
    verbosity_level=$1
    shift
    log "$@"
    shift
    if [ "$verbosity" -ge "$verbosity_level" ]; then
	echo "$@"
    fi
}

test_number () {
    if [ "x$number_tests" != "x" ]; then
	printf " %d" $testcount
    fi
}

subtest_number () {
    if [ "x$number_tests" != "x" ]; then
	printf "`test_number`.%d" $subtestcount
    fi
}

report_results () {
    verbosity_level=$1
    shift
    result="`expr $2 - $3` / $2 $1 passed"
    if test "x$3" != "x0"; then
	result_type=fail
	result="$result, $3 FAILED"
	if [ "x$1" != "xsubtests in total" ]; then
	    result="FAIL: $result"
	fi
    else
	result_type=pass
	result="PASS: All $result"
    fi
    if [ "x$1" = "xsubtests" ]; then
	result="*`test_number` $result"
    fi
    echo_log $verbosity_level $result_type "$result
"
}

test_end () {
    report_results 1 "subtests" $subtestcount $subfailcount
    if test "x$subfailcount" != "x0"; then
	failcount=`expr $failcount + 1`
    fi
    subtestcount_total=`expr $subtestcount_total + $subtestcount`
    subfailcount_total=`expr $subfailcount_total + $subfailcount`
    subfailcount=0
}

testset_end () {
    if [ "x$subtestcount" != "x0" ]; then
	test_end
    fi
    report_results 1 "tests" $testcount $failcount
    report_results 2 "subtests in total" $subtestcount_total $subfailcount_total
    if test "x$failcount" != "x0"; then
	exit 1
    else
	exit 0
    fi
}

testset_begin () {
    if [ "x$testcount" != "x0" ]; then
	testset_end
    fi
    testset_descr="$1"
    echo_log 1 meta "Test set: $testset_descr
"
    preamble=
    test_descr=
    input=
    output=
    template=
    testcount=0
    failcount=0
    subtestcount_total=0
    subfailcount_total=0
}

test_begin () {
    if [ "x$subtestcount" != "x0" ]; then
	test_end
    fi
    test_descr="$1"
    testcount=`expr $testcount + 1`
    echo_log 1 meta "Test`test_number`: $test_descr"
    subtestcount=0
    subfailcount=0
}

cleanup () {
    if [ "x$testcount" != "x0" ]; then
	testset_end
	testcount=0
    fi
    if [ "x$debug" = "x" ]; then
	rm -f $pmatch_file $err_file $out_file $expect_out_file $file_base.src
	rm -f $file_base-*.pmatch $file_base-*.src $file_base-*.descr
    fi
    echo_log 3 meta "
Test completed at "`date +"%Y-%m-%d %H:%M:%S"`"
"
}

cleanup_abort () {
    echo_log 1 meta "
Caught a signal; aborting tests."
    exit 77
}


trap cleanup 0
trap cleanup_abort 1 2 13 15

echo_log 3 meta "Test run on "`uname -n`" at "`date +"%Y-%m-%d %H:%M:%S"`"
"
echo_log 3 meta "Command line: $cmdline
"


set_compiler_opts() {
    compiler_opts="$@"
}

set_runner_opts() {
    runner_opts="$@"
}

common_preamble () {
    preamble="$1"
}

compile_to () {
    compile_only=
    if [ "x$1" = "x--compile-only" ]; then
	compile_only=1
	shift
    fi
    code="$1"
    codefile="$2"
    if [ "x$preamble" != "x" ]; then
	code="$preamble
$code"
    fi
    srcfile=`basename $codefile .pmatch`.src
    echo "$code" > $srcfile
    $tooldir/$compiler $compiler_opts $srcfile > $codefile 2> $err_file
    if test $? != 0 || test -s $err_file; then
	if [ "x$compile_only" != "x" ]; then
	    msg="* Subtest`subtest_number` compilation FAILED: $descr"
	else
	    msg="* Compilation FAILED"
	fi
	echo_log 1 fail "$msg"
	echo_log 2 fail "--- Source:
$code
--- Errors in running $tooldir/$compiler:
`cat $err_file`"
	rm $err_file
        return 1
    else
	if [ "x$compile_only" != "x" ]; then
	    echo_log 2 pass "* Subtest`subtest_number` compilation passed: $descr"
	    echo_log 3 verbose "--- Source:
$code"
	fi
    fi
    return 0
}

compile () {
    compile_to "$@" $pmatch_file
}

compile_template () {
    opts=
    # FIXME: "shift n" is not portable
    getopts_func opts "$@" || shift $?
    compile $opts "`echo "$template" | sed -e "s/@1/$1/g" -e "s/@2/$2/g" -e "s/@3/$3/g"`"
    return $?
}

check_with_file () {
    subtestcount=`expr $subtestcount + 1`
    if [ "x$1" = "x--printf" ]; then
	print_fn=printf
	shift
    else
	print_fn=echo
    fi
    _code_file="$1"
    _descr="$2"
    shift
    shift
    srcfile=`basename $codefile .pmatch`.src
    if test "$#" -lt 2; then
	_use_input="$input"
	_use_expect_output="$output"
    else
	_use_input="$1"
	_use_expect_output="$2"
    fi
    $print_fn "$_use_expect_output" > $expect_out_file
    $print_fn "$_use_input" |
    $tooldir/$runner $runner_opts $_code_file > $out_file 2> $err_file
    diff -q $expect_out_file $out_file > /dev/null
    if test $? != 0 || test -s $err_file; then
	echo_log 1 fail "* Subtest`subtest_number` FAILED: $_descr"
	echo_log 3 fail "--- Source:
`cat $srcfile`"
	subfailcount=`expr $subfailcount + 1`
	if test -s $err_file; then
	    echo_log 2 fail "--- Errors in running $runner $runner_opts:
`cat $err_file`"
	else
	    echo_log 2 fail "--- Input:
$_use_input"
	    echo_log 2 fail "--- Expected output:
`cat $expect_out_file`"
	    echo_log 2 fail "--- Actual output:
`cat $out_file`"
	fi
    else
	echo_log 2 pass "* Subtest`subtest_number` passed: $_descr"
	echo_log 3 verbose "--- Source:
`cat $srcfile`"
	echo_log 3 verbose "--- Input:
$_use_input"
	echo_log 3 verbose "--- Output:
`cat $expect_out_file`"
    fi
}

check () {
    opts=
    # FIXME: "shift n" is not portable
    getopts_func opts "$@" || shift $?
    check_with_file $opts $pmatch_file "$@"
}

check_compile_run_single () {
    opts=
    # FIXME: "shift n" is not portable
    getopts_func opts "$@" || shift $?
    _descr="$1"
    _code="$2"
    shift
    shift
    if compile "$_code"; then
	check $opts "$_descr" "$@"
    else
	subtestcount=`expr $subtestcount + 1`
	subfailcount=`expr $subfailcount + 1`
    fi
}

check_compile_aux () {
    func=$1
    shift
    opts=
    # FIXME: "shift n" is not portable
    getopts_func opts "$@" || shift $?
    descr=$1
    shift
    subtestcount=`expr $subtestcount + 1`
    $func $opts "$@"
}

check_compile () {
    check_compile_aux compile --compile-only "$@"
}

check_compile_template () {
    check_compile_aux compile_template --compile-only "$@"
}

subst_templ_arg () {
    templ="$1"
    argnum=$2
    arg=$3
    arg="`echo "$arg" | sed -e 's/\\\\/\\\\\\\\/g'`"
    echo "$templ" | sed -e "s/@$argnum@/$arg/g"
}

check_compile_run_multi () {
    codenum=1
    codefiles=
    codetempl=
    descrtempl=
    descr=
    fname=
    common_input=
    compile_failed_count=0
    while [ "x$1" != "x" ]; do
	optname=$1
	case "$1" in
	    --code )
		shift
		if [ "$#" -lt 2 ]; then
		    error "Expected a description and code after $optname"
		fi
		fname=$file_base-$codenum.pmatch
		if compile_to "$2" "$fname"; then
		    echo "$1" > $file_base-$codenum.descr
		    codefiles="$codefiles $fname"
		    codenum=$(($codenum + 1))
		else
		    compile_failed_count=$(($compile_failed_count + 1))
		fi
		shift
		shift
		;;
	    --codetempl | --code-template )
		shift
		if [ "$#" -lt 1 ]; then
		    error "Expected a template after $optname"
		elif [ "x$codetempl" != "x" ]; then
		    warn "A code template already provided"
		fi
		codetempl="$1"
		shift
		;;
	    --descrtempl | --description-template )
		shift
		if [ "$#" -lt 1 ]; then
		    error "Expected a template after $optname"
		elif [ "x$descrtempl" != "x" ]; then
		    warn "A description template already provided"
		fi
		descrtempl="$1"
		shift
		;;
	    --templargs | --template-arguments )
		shift
		if [ "x$descrtempl" != "x" ]; then
		    if [ "$#" -lt 1 ]; then
			error "Expected at least one argument after $optname"
		    fi
		    descr="$descrtempl"
		elif [ "$#" -lt 2 ]; then
		    error "Expected a description and at least one argument after $optname"
		else
		    descr="$1"
		    shift
		fi
		codetempl_filled=$codetempl
		argnum=1
		while [ "$#" -gt 0 ]; do
		    case "$1" in
			--* )
			    break
			    ;;
			* )
			    codetempl_filled="`subst_templ_arg "$codetempl_filled" $argnum "$1"`"
			    descr="`subst_templ_arg "$descr" $argnum "$1"`"
			    argnum=$(($argnum + 1))
			    ;;
		    esac
		    shift
		done
		fname=$file_base-$codenum.pmatch
		if compile_to "$codetempl_filled" "$fname"; then
		    echo "$descr" > $file_base-$codenum.descr
		    codefiles="$codefiles $fname"
		    codenum=$(($codenum + 1))
		else
		    compile_failed_count=$(($compile_failed_count + 1))
		fi
		;;
	    --outtemplargs | --output-template-arguments )
		shift
		if [ "x$common_input" = "x" ]; then
		    error "$optname requires the use of a preceding --input"
		elif [ "x$descrtempl" != "x" ]; then
		    if [ "$#" -lt 2 ]; then
			error "Expected the expected output string and at least one template argument after $optname"
		    fi
		    descr="$descrtempl"
		elif [ "$#" -lt 3 ]; then
		    error "Expected a description, the expected output string description and at least one argument after $optname"
		else
		    descr="$1"
		    shift
		fi
		expected_output=$1
		shift
		codetempl_filled=$codetempl
		argnum=1
		while [ "$#" -gt 0 ]; do
		    case "$1" in
			--* )
			    break
			    ;;
			* )
			    codetempl_filled="`subst_templ_arg "$codetempl_filled" $argnum "$1"`"
			    descr="`subst_templ_arg "$descr" $argnum "$1"`"
			    argnum=$(($argnum + 1))
			    ;;
		    esac
		    shift
		done
		check_compile_run_single "$descr" "$codetempl_filled" "$common_input" "$expected_output"
		;;
	    --templarg-single | --template-argument-single )
		shift
		while [ "$#" -gt 0 ]; do
		    case "$1" in
			--* )
			    break
			    ;;
			* )
			    if [ "x$descrtempl" != "x" ]; then
				if [ "$#" -lt 1 ]; then
				    error "Expected at least one argument after $optname"
				fi
				descr="$descrtempl"
			    elif [ "$#" -lt 2 ]; then
				error "Expected a description and at least one argument after $optname"
			    else
				descr="$1"
				shift
			    fi
			    codetempl_filled="`subst_templ_arg "$codetempl" 1 "$1"`"
			    descr="`subst_templ_arg "$descr" 1 "$1"`"
			    fname=$file_base-$codenum.pmatch
			    if compile_to "$codetempl_filled" "$fname"; then
				echo "$descr" > $file_base-$codenum.descr
				codefiles="$codefiles $fname"
				codenum=$(($codenum + 1))
			    else
				compile_failed_count=$(($compile_failed_count + 1))
			    fi
			    ;;
		    esac
		    shift
		done
		;;
	    --inout | --input-output)
		shift
		if [ "$#" -lt 3 ]; then
		    error "Expected a description, input and expected output after $optname"
		fi
		opts=
		if [ "x$1" = "x--printf" ]; then
		    opts=$1
		    shift
		fi
		for codefile in $codefiles; do
		    descrfile=`basename $codefile .pmatch`.descr
		    descr="`cat $descrfile`"
		    if [ "x$descr" = "x" ]; then
			descr=$1
		    elif [ "x$1" != "x" ]; then
			descr="$descr, $1"
		    fi
		    check_with_file $opts $codefile "$descr" "$2" "$3"
		done
		subtestcount=$(($subtestcount + $compile_failed_count))
		subfailcount=$(($subfailcount + $compile_failed_count))
		shift
		shift
		shift
		;;
	    --input )
		shift
		if [ "$#" -lt 1 ]; then
		    error "Expected an input string after --input"
		fi
		common_input=$1
		shift
		;;
	    --* )
		error "check_compile_run: Unrecognized option $1"
		;;
	    * )
		error "check_compile_run: Unrecognized parameter $1"
		;;
	esac
    done
    if [ "x$debug" = "x" ]; then
	for codefile in $codefiles; do
	    base=`basename $codefile .pmatch`
	    rm -f $codefile $base.src $base.descr
	done
    fi
}

check_compile_run () {
    case "$1" in
	--* )
	    check_compile_run_multi "$@"
	    ;;
	* )
	    check_compile_run_single "$@"
	    ;;
    esac
}
