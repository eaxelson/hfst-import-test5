# programs
XFST="/home/eaxelson/xfst/bin/xfst -q"
FOMA="foma -q"
HFST="../../../tools/src/parsers/hfst-xfst2fst -q"
SH="/bin/bash"

# location of hfst tools
tooldir="../../../tools/src/"
# back-end formats used in testing     
backend_formats="sfst openfst-tropical foma"
# back-end format used when comparing results
common_format="openfst-tropical" 

# EinsteinsPuzzle
# FinnishNumerals
# FinnishProsody
# Palindromes

# todo: fix and add:
#   DateParser
#   NumbersToNumerals
#   PlusOrMinus

examples="BetterColaMachine BrazilianPortuguese1 BrazilianPortuguese2 EnglishNumerals "\
"EsperantoAdjectives EsperantoNounsAdjectivesAndVerbs EsperantoNounsAndAdjectivesWithTags "\
"EsperantoNounsAndAdjectives EsperantoNouns FinnishOTProsody Lingala "\
"MonishAnalysis MonishGuesserAnalyzer YaleShooting"

if ! [ "$1" = "" ]; then
    examples=$1
fi

for example in $examples;
do
    if [ -f xfst-scripts/$example.xfst.script -a \
         -f hfst-scripts/$example.hfst.script ]; 
    then
        echo "Testing "$example"..."

        # (1) Compile with xfst/foma
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
        # convert from prolog to openfst-tropical for comparing
        if ! (cat Result | $tooldir/hfst-txt2fst --prolog -f $common_format > tmp && \
            mv tmp Result_from_xfst); then
            exit 1;
        fi

        # (2) Compile hfst script with all back-end formats and compare the results
        #     with the result from xfst/foma
        for format in $backend_formats; 
        do
            echo "  compiling hfst script with back-end format "$format".."
            if ! ($SH hfst-scripts/$example.hfst.script $format $tooldir); then
                exit 1
            fi
            cat Result | $tooldir/hfst-fst2fst -f $common_format > tmp
            mv tmp Result_from_hfst_script_$format

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
        done

    else
        echo "WARNING: missing files, skipping test "$example
    fi
done

rm -f Result_from_xfst 
for format in $backend_formats;
do 
    rm -f Result_from_hfst_script_$format
done
