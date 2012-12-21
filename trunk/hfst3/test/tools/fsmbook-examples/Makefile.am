SUBDIRS=BetterColaMachine BrazilianPortuguese1 BrazilianPortuguese2 \
	DateParser EinsteinsPuzzle EnglishNumerals Esperanto FinnishNumerals \
	NumbersToNumerals MonishAnalysis YaleShooting	 
# Palindromes PlusOrMinus
#	Other directories are not included until we have the rule compiler.
#	DateParser disables until we get hfst-regexp2fst working
#	hfst-calculate cannot be used because make distcheck 
#	complains that it is missing

TESTS=better-cola-machine-test.sh \
	brazilian-portuguese1-test.sh \
	brazilian-portuguese2-test.sh \
	date-parser-test.sh \
	einsteins-puzzle-test.sh \
	english-numerals-test.sh \
	esperanto-test.sh \
	finnish-numerals-test.sh \
	numbers-to-numerals-test.sh \
	monish-analysis-test.sh \
	yale-shooting-test.sh 
	
	
 # finnish-ot-prosody-test.sh - bug somewhere
 
 # finnish-prosody.sh - not standard
  
 # 	monish-guesser-analyzer-test.sh  ---- weird error with foma type when tested with foma - bug No 110


 # lingala - can't make att from xfst: *** ERROR: Sorry. This network is too large to be saved as an AT&T-format transducer file by xfst. You need to license a professional version of this program. ***
 # 										^^^^^^^^ same with prolog
 #			- no substitution `[RAgr1, Func, Sub];
 #			- some other things might be missing, ie. what is Func?
 

 # palindromes - there is no xfst.script!, not standard - works fine, add to make file somehow!!!!!
 # plus-or-minus - missing  =>
 
 
EXTRA_DIST=xfst-att-to-hfst-att.sh remove-complement.sh test-example.sh \
	foma-att-to-hfst-att.sh $(TESTS)