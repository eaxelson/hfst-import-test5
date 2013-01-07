#!/bin/bash
cat > lexicon.lexc <<ENDLEXC
LEXICON Root
  Nouns ;

LEXICON Nouns
  cat NumberS ;
  ax NumberES ;

LEXICON NumberS
  0 # ;
  s # ;

LEXICON NumberES
  0  # ;
  es # ;
ENDLEXC

hfst-lexc lexicon.lexc -o lexc-lexicon.hfst

echo "@Root@ @Nouns@ @LEX@" > strings
echo "@Nouns@ c a t @NumberS@ @LEX@" >> strings
echo "@Nouns@ a x @NumberES@ @LEX@" >> strings
echo "@NumberS@ @END@ @LEX@" >> strings
echo "@NumberS@ s @END@ @LEX@" >> strings
echo "@NumberES@ @END@ @LEX@" >> strings
echo "@NumberES@ e s @END@ @LEX@" >> strings
hfst-strings2fst --has-spaces --disjunct-strings < strings  | 
    hfst-repeat -f 1 > bag_of_morphs

echo "%@Root%@ [? - %@LEX%@]*"  |  hfst-regexp2fst > start
echo "%@END%@ %@LEX%@"  |  hfst-regexp2fst > end

echo "%@NumberS%@ %@LEX%@ %@NumberS%@ [? - %@LEX%@]*"  |  hfst-regexp2fst > cont1
echo "%@NumberES%@ %@LEX%@ %@NumberES%@  [? - %@LEX%@]*"  |  hfst-regexp2fst > cont2
echo "%@Nouns%@ %@LEX%@ %@Nouns%@  [? - %@LEX%@]*"  |  hfst-regexp2fst > cont3
hfst-disjunct cont1 cont2 | hfst-disjunct - cont3 |
    hfst-repeat -f 1 > conts
hfst-concatenate start conts | hfst-concatenate - end > morphotactics
hfst-compose bag_of_morphs  morphotactics > lexicon
