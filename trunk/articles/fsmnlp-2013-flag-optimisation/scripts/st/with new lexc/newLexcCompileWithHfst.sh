#!/bin/bash

# compiles those languages into .hfst and .foma transducers
#deu
#grn
#kmr


# deu

echo '--- deu ---'
cd deu/src/

rm ../bin/twol.hfst
rm ../tmp/deu-all.lex
rm ../bin/deu-lex-new.hfst
rm ../bin/deu.hfst
rm ../bin/deu.foma


echo '***twolc***\n'

hfst-twolc -f foma -i twol-deu.txt -o ../bin/twol.hfst


echo '***building lexicon***\n'
   
cat deu-lex.txt noun-deu-morph.txt noun-deu-lex.txt \
        adj-deu-lex.txt adv-deu-lex.txt verb-deu-lex.txt verb-deu-morph.txt \
    pron-deu-lex.txt punct-deu-lex.txt numeral-deu-lex.txt \
    pp-deu-lex.txt cc-deu-lex.txt cs-deu-lex.txt interj-deu-lex.txt  > ../tmp/deu-all.lex   
    
    
# hfst-lexc -f foma -o ../bin/deu-lex.hfst ../tmp/deu-all.lex
# new lexc with automatic flags
hfst-lexc2fst -f foma -o ../bin/deu-lex-new.hfst ../tmp/deu-all.lex

echo '***compose lexicon and rules***\n '


hfst-compose-intersect ../bin/deu-lex-new.hfst ../bin/twol.hfst | hfst-minimize > ../bin/deu.hfst 

hfst-fst2fst -f foma -b ../bin/deu.hfst -o ../bin/deu.foma

cd ../../

# end deu
##########################################################
##########################################################
# grn
echo '--- grn ---'
cd grn/src/

rm ../tmp/grn-lex.all
rm ../bin/grn-bin.hfst
rm ../bin/grn.hfst 
rm ../bin/grn.foma
rm ../bin/grn-lex-new.hfst


echo '***building lexicon***\n'

cat grn-lex.txt noun-grn-morph.txt noun-grn-lex.txt \
    adj-grn-lex.txt adv-grn-lex.txt \
    verb-grn-lex.txt verb-grn-morph.txt \
    pron-grn-lex.txt \
    pp-grn-lex.txt \
    num-grn-lex.txt \
    cc-grn-lex.txt \
    interj-grn-lex.txt \
    cs-grn-lex.txt \
    punct-grn-lex.txt > ../tmp/grn-lex.all
    

#hfst-lexc -f foma -o ../bin/grn-lex-new.hfst ../tmp/grn-lex.all
hfst-lexc2fst -f foma -o ../bin/grn-lex-new.hfst ../tmp/grn-lex.all

echo '***building rules***\n'

cat xfst-grn.txt | sed 's/\s\!.*$/ /g' | grep -v '^!' | sed 's/$/ /g' | grep -v 'echo' > Rules_clean.tmp

printf "source Rules_clean.tmp \n\
    quit \n" > ./xfst-grn-script
foma < ./xfst-grn-script
rm -f ./xfst-grn-script

gunzip -c ../bin/xfst-grn.hfst > Rules_unziped
hfst-fst2fst -f foma Rules_unziped -o ../bin/grn-bin.hfst

rm -f Rules_unziped
rm -f Rules_clean.tmp

echo ' ---rules done ---'
echo '***compose lexicon and rules***\n '

hfst-compose -F -1 ../bin/grn-lex-new.hfst -2 ../bin/grn-bin.hfst | hfst-minimize > ../bin/grn.hfst 
hfst-fst2fst -f foma -b ../bin/grn.hfst -o ../bin/grn.foma

cd ../../
# end grn
##########################################################
##########################################################
# kmr
echo '--- kmr ---'
cd kmr/src/


rm ../bin/twol.hfst
rm ../tmp/kmr-all.lex
rm ../bin/kmr.hfst
rm ../bin/kmr.foma
rm ../bin/kmr-lex-new.hfst

echo '***twolc***\n'

hfst-twolc -f foma -i twol-kmr.txt -o ../bin/twol.hfst


echo '***building lexicon***\n'
   
cat kmr-lex.txt \
    noun-kmr-lex.txt \
    noun-kmr-morph.txt \
    adj-kmr-lex.txt \
    adj-kmr-morph.txt \
    verb-kmr-lex.txt \
    verb-kmr-morph.txt \
    adv-kmr-lex.txt \
    pron-kmr-lex.txt \
    punct-kmr-lex.txt \
    pp-kmr-lex.txt \
    cc-kmr-lex.txt \
    cs-kmr-lex.txt \
    interj-kmr-lex.txt   > ../tmp/kmr-all.lex   
    
    
#hfst-lexc -f foma -o ../bin/kmr-lex.hfst ../tmp/kmr-all.lex
hfst-lexc2fst -f foma -o ../bin/kmr-lex-new.hfst ../tmp/kmr-all.lex


echo '***compose lexicon and rules***\n '


hfst-compose-intersect ../bin/kmr-lex-new.hfst ../bin/twol.hfst | hfst-minimize > ../bin/kmr.hfst 

#echo "xfst result 26.5 Kb. 328 states, 535 arcs, 11966 paths. \n" 
#hfst-summarize ../bin/kmr.hfst
hfst-fst2fst -f foma -b ../bin/kmr.hfst -o ../bin/kmr.foma

cd ../../
# end kmr





