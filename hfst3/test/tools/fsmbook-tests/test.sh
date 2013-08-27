XFST="/home/eaxelson/xfst/bin/xfst -q"
FOMA="foma -q"
HFST="../../../tools/src/parsers/hfst-xfst2fst -q"
SH="/bin/bash"

tooldir="../../../tools/src/"
format="openfst-tropical"
common_format="openfst-tropical"

# EinsteinsPuzzle
# Esperanto
# FinnishNumerals
# FinnishProsody
# Palindromes
# PlusOrMinus

# DateParser  

for example in Lingala \
    BetterColaMachine \
    BrazilianPortuguese1 \
    BrazilianPortuguese2 \
    EnglishNumerals \
    FinnishOTProsody \
    Lingala \
    MonishAnalysis \
    MonishGuesserAnalyzer \
    NumbersToNumerals \
    YaleShooting ;
do
    if [ -f xfst-scripts/$example.xfst.script -a \
         -f hfst-scripts/$example.hfst.script ]; 
    then
        echo "Testing "$example"..."

        # Compile with xfst, hfst-xfst and hfst tools
        if [ "$example" = "FinnishOTProsody" -o \
             "$example" = "Lingala" -o \
             "$example" = "YaleShooting" ]; then
            echo "  compiling with foma (result from xfst will be too big).."
            $FOMA -f xfst-scripts/$example.xfst.script > /dev/null 2> /dev/null && \
                mv Result Result_from_xfst
        else
            echo "  compiling with xfst.."
            $XFST -f xfst-scripts/$example.xfst.script && \
                mv Result Result_from_xfst
        fi
        #echo "  compiling with hfst-xfst.."
        #$HFST -F xfst-scripts/$example.xfst.script && \
        #mv Result Result_from_hfst &&
        echo "  compiling hfst script.."
        $SH hfst-scripts/$example.hfst.script $format $tooldir && \
            mv Result Result_from_hfst_script
        rm -f FOO BAR
        
        # Convert all results to hfst binary transducers in common format
        # for comparison
        cat Result_from_xfst | ./xfst-att-to-hfst-att.sh > tmp && \
            mv tmp Result_from_xfst
        cat Result_from_xfst | $tooldir/hfst-txt2fst -f $common_format > tmp && \
            mv tmp Result_from_xfst

        #cat Result_from_hfst | $tooldir/hfst-txt2fst -f $common_format > tmp && \
        #mv tmp Result_from_hfst

        cat Result_from_hfst_script | $tooldir/hfst-fst2fst -f $common_format > tmp && \
            mv tmp Result_from_hfst_script

        # Compare
        #if ! ($tooldir/hfst-compare -q Result_from_xfst Result_from_hfst); then
        #    echo "  FAIL: Results from xfst and hfst-xfst differ in test "$example
        #fi
        if ! ($tooldir/hfst-compare -q Result_from_xfst Result_from_hfst_script); then
            echo "  FAIL: Results from xfst and hfst script differ in test "$example
        fi
    else
        echo "WARNING: missing files, skipping test "$example
    fi
done
