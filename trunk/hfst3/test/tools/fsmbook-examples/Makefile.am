SUBDIRS=EnglishNumerals NumbersToNumerals FinnishNumerals \
	EinsteinsPuzzle Esperanto BrazilianPortuguese1 BrazilianPortuguese2 
# FinnishOTProsody YaleShooting
#	Other directories are not included until we have the rule compiler.
#	DateParser disables until we get hfst-regexp2fst working
#	hfst-calculate cannot be used because make distcheck 
#	complains that it is missing

TESTS=english-numerals-test.sh \
	numbers-to-numerals-test.sh finnish-numerals-test.sh \
	einsteins-puzzle-test.sh esperanto-test.sh \
	brazilian-portuguese1-test.sh \
	brazilian-portuguese2-test.sh \
	monish-analysis-test.sh
 # date-parser-test.sh - missing =>, there are 2 hfst files - regexp & via tools
 # monish-guesser-analyzer-test.sh - weird bug in Root - it looks like there is some issue with alphabet 
 # better-cola-machine - missing ^ operator
 # finnish-ot-prosody.sh - TODO: a @-> ... B 
 # finnish-prosody.sh - not standard
 # lingala - can't make att from xfst: *** ERROR: Sorry. This network is too large to be saved as an AT&T-format transducer file by xfst. You need to license a professional version of this program. ***
 # 										^^^^^^^^ same with prolog
 # palindromes - there is no xfst.script!
 # plus-or-minus - missing  =>
 # yale-shooting.sh - no "=>" rules
 
 
 # monish-guesser-analyzer-test.sh
 # better-cola-machine
 # date-parser-test.sh
 # finnish-ot-prosody.sh 
 # finnish-prosody.sh
 # lingala
 # monish-analysis
 # monish-guesser-analyzer
 # palindromes - there is no xfst.script!
 # plus-or-minus - missing <- , =>
 # yale-shooting.sh - no "=>" rules



EXTRA_DIST=xfst-att-to-hfst-att.sh remove-complement.sh test-example.sh \
	foma-att-to-hfst-att.sh $(TESTS)