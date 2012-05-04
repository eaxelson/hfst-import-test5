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
	brazilian-portuguese2-test.sh
# finnish-ot-prosody.sh 
# yale-shooting.sh - containment and cmplement are not developed, testing => rules
# brazilian-portuguese1-test.sh - xfst has OTHER - not comparable with hfst 
# date-parser-test.sh

EXTRA_DIST=xfst-att-to-hfst-att.sh remove-complement.sh test-example.sh \
	foma-att-to-hfst-att.sh $(TESTS)