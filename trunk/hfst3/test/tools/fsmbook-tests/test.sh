# programs
XFST="/home/eaxelson/xfst/bin/xfst -q"
FOMA="foma -q"
HFST="../../../tools/src/parsers/hfst-xfst2fst -q"
SH="/bin/bash"

# location of hfst tools
tooldir="../../../tools/src/"
# back-end formats used in testing (common format must be listed first as it is used for comparison)    
backend_formats="openfst-tropical sfst foma"
# back-end format used when comparing results
common_format="openfst-tropical" 

# todo: fix and add:
#   DateParser: xfst rule of type [ ? @-> foo ... bar ] is compiled differently by hfst-regexp2fst and xfst

# NOTE: FinnishNumerals depends on NumbersToNumerals, so they must be compiled in the right order.

examples="BetterColaMachine BrazilianPortuguese1 BrazilianPortuguese2 EnglishNumerals "\
"EsperantoAdjectives EsperantoNounsAdjectivesAndVerbs EsperantoNounsAndAdjectivesWithTags "\
"EsperantoNounsAndAdjectives EsperantoNouns FinnishOTProsody Lingala "\
"MonishAnalysis MonishGuesserAnalyzer NumbersToNumerals PlusOrMinus FinnishNumerals "\
"YaleShooting FinnishProsody Palindromes EinsteinsPuzzle"

if ! [ "$1" = "" ]; then
    examples=$1
fi

for example in $examples;
do
    echo "Testing "$example"..."
    
    # (1) If xfst solution exists,
    if ! [ "$example" = "FinnishNumerals" -o \
        "$example" = "FinnishProsody" -o \
        "$example" = "Palindromes" ]; then

        # compile with xfst/foma..
        if [ "$example" = "FinnishOTProsody" -o \
            "$example" = "Lingala" -o \
            "$example" = "YaleShooting" ]; then
            echo "  compiling with foma (result from xfst will be too big).."
            if ! ($FOMA -f xfst-scripts/$example.xfst.script > /dev/null 2> /dev/null); then
                exit 1;
            fi
        else
            echo "  compiling with xfst.."
            if ! ($XFST -f xfst-scripts/$example.xfst.script); then
                exit 1;
            fi
        fi
        # and convert from prolog to openfst-tropical for comparing.
        if ! (cat Result | $tooldir/hfst-txt2fst --prolog -f $common_format > tmp && \
            mv tmp Result_from_xfst); then
            exit 1;
        fi

        # Also compile with hfst-xfst using all back-end formats..
        for format in $backend_formats; 
        do
            echo "  compiling with hfst-xfst using back-end format "$format".."
            if ! ($HFST -f $format -F xfst-scripts/$example.xfst.script > /dev/null 2> /dev/null); then
                exit 1;
            fi
            # and convert from prolog to openfst-tropical and compare the results.
            if ! (cat Result | $tooldir/hfst-txt2fst --prolog -f $common_format > tmp && \
                mv tmp Result_from_hfst_xfst); then
                exit 1;
            fi
            if ! ($tooldir/hfst-compare -q Result_from_xfst Result_from_hfst_xfst); then
                echo "FAIL: results from xfst and hfst-xfst ("$format") are not equivalent"
                # todo: log results and remove Result_from_xfst and Result_from_hfst_xfst
                # exit 1;
            fi
        done

    fi

    # (2) Compile hfst script with all back-end formats and compare the results..
    for format in $backend_formats; 
    do
        echo "  compiling hfst script with back-end format "$format".."

        if [ "$example" = "FinnishNumerals" ]; then
            if ! [ -f tmpdir/NumbersToNumerals ]; then
                echo "FAIL: missing file tmpdir/NumbersToNumerals in test FinnishNumerals,"
                echo "      the test NumbersToNumerals must be run first"
                exit 1
            fi
        fi

        if ! ($SH hfst-scripts/$example.hfst.script $format $tooldir); then
            exit 1
        fi
        cat Result | $tooldir/hfst-fst2fst -f $common_format > tmp
        mv tmp Result_from_hfst_script_$format
        
        # with the result from xfst/foma, if available..
        if ! [ "$example" = "FinnishNumerals" -o \
            "$example" = "FinnishProsody" -o \
            "$example" = "Palindromes" ]; then
            # special case 1
            if [ "$example" = "EinsteinsPuzzle" ]; then
                if ! ($tooldir/hfst-fst2strings --xfst=print-space Result_from_xfst | grep "German coffee Prince fish" > /dev/null); then
                    echo "FAIL"
                fi
                if ! ($tooldir/hfst-fst2strings Result_from_hfst_script_$format | \
                      grep "fish" | grep "German coffee Prince fish" > /dev/null); then
                    echo "FAIL"
                fi
                continue
            fi
            # special case 2
            if [ "$example" = "NumbersToNumerals" ]; then
                if ! [ -d tmpdir ]; then
                    mkdir tmpdir
                fi
                cp Result_from_xfst tmpdir/NumbersToNumerals # needed in FinnishNumerals
                $tooldir/hfst-fst2strings Result_from_xfst | sort > tmp_xfst
                $tooldir/hfst-fst2strings Result_from_hfst_script_$format | sort > tmp_hfst
                if ! (diff tmp_xfst tmp_hfst); then
                    echo "FAIL"
                fi
                rm -f tmp_hfst tmp_xfst
                continue
            fi
            if ! ($tooldir/hfst-compare -q Result_from_xfst Result_from_hfst_script_$format); then
                echo "  FAIL: Results from xfst and hfst scripts ("$format") differ in test "$example", storing results in files:"
                echo "    log/"$example.result_from_xfst_script_using_xfst_tool 
                echo "    log/"$example.result_from_hfst_script_using_backend_format_$format
                if ! [ -d log ]; then
                    mkdir log
                fi
                cp Result_from_xfst log/$example.result_from_xfst_script_using_xfst_tool
                cp Result_from_hfst_script_$format log/$example.result_from_hfst_script_using_backend_format_$format
            fi
            
        # or with the result in common format.
        else
            # special case
            if [ "$example" = "FinnishNumerals" -a "$format" = "sfst" ]; then
                echo "    skipping comparing the results, result from SFST will be big because of epsilon handling in composition.."
                continue
            fi

            if ! [ "$format" = "$common_format" ]; then
                if ! ($tooldir/hfst-compare -q Result_from_hfst_script_$common_format Result_from_hfst_script_$format); then
                    echo -n "  FAIL: Results from hfst scripts ("$format" and "$common_format") differ in test "$example
                    echo ", storing results in files:"
                    echo "    log/"$example.result_from_hfst_script_using_backend_format_$format
                    echo "    log/"$example.result_from_hfst_script_using_backend_format_$common_format
                    if ! [ -d log ]; then
                        mkdir log
                    fi
                    cp Result_from_hfst_script_$common_format log/$example.result_from_hfst_script_using_backend_format_$common_format
                    cp Result_from_hfst_script_$format log/$example.result_from_hfst_script_using_backend_format_$format
                fi
            fi
        fi
    done

done

rm -f Result_from_xfst Result
for format in $backend_formats;
do 
    rm -f Result_from_hfst_script_$format
done
