SUBDIRS=NumbersToNumerals BetterColaMachine BrazilianPortuguese1 BrazilianPortuguese2 \
	DateParser EinsteinsPuzzle EnglishNumerals Esperanto FinnishNumerals \
	FinnishProsody Lingala MonishAnalysis MonishGuesserAnalyzer Palindromes PlusOrMinus YaleShooting  
#	Other directories are not included until we have the rule compiler.
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
	finnish-prosody-test.sh \
	lingala.sh \
	numbers-to-numerals-test.sh \
	monish-analysis-test.sh \
	monish-guesser-analyzer-test.sh \
	palindromes.sh \
	plus-or-minus.sh \
	yale-shooting-test.sh 
	
	
 # finnish-ot-prosody-test.sh - bug somewhere
 #		probably problem with alphabet minimization (containment in xfst, replace rules context in foma)
  
 
 
EXTRA_DIST=xfst-att-to-hfst-att.sh remove-complement.sh test-example.sh \
	foma-att-to-hfst-att.sh test-foma.sh $(TESTS)