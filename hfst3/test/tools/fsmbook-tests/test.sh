XFST="/home/eaxelson/xfst/bin/xfst"
HFST="../../../tools/src/parsers/hfst-xfst2fst"
SH="/bin/bash"

tooldir="../../../tools/src/"
format="openfst-tropical"

for example in \
    BetterColaMachine;
#    BrazilianPortuguese1 \
#    BrazilianPortuguese2 \
#    DateParser \
#    EinsteinsPuzzle \
#    EnglishNumerals \
#    Esperanto \
#    FinnishNumerals \
#    FinnishOTProsody \
#    FinnishProsody \
#    Lingala \
#    MonishAnalysis \
#    MonishGuesserAnalyzer \
#    NumbersToNumerals \
#    Palindromes \
#    PlusOrMinus \
#    YaleShooting;
do
    if [ -f xfst-scripts/$example.xfst.script -a \
         -f hfst-scripts/$example.hfst.script ]; 
    then
        # Compile with xfst, hfst-xfst and hfst tools
        $XFST -f xfst-scripts/$example.xfst.script && \
        mv Result Result_from_xfst && \
        $HFST -F xfst-scripts/$example.xfst.script && \
        mv Result Result_from_hfst && \
        $SH hfst-scripts/$example.hfst.script $format $tooldir && \
        mv Result Result_from_hfst_script && \
        rm FOO BAR
        
        
    fi
done
