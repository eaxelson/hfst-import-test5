SUBDIRS=EnglishNumerals NumbersToNumerals FinnishNumerals \
	EinsteinsPuzzle Esperanto
#	Other directories are not included until we have the rule compiler.
#	DateParser disables until we get hfst-regexp2fst working
#	hfst-calculate cannot be used because make distcheck 
#	complains that it is missing

TESTS=english-numerals-test.sh \
      numbers-to-numerals-test.sh finnish-numerals-test.sh \
      einsteins-puzzle-test.sh esperanto-test.sh 
# brazilian-portuguese1-test.sh doesn't work yet
#	date-parser-test.sh

EXTRA_DIST=xfst-att-to-hfst-att.sh remove-complement.sh test-example.sh \
	   foma-att-to-hfst-att.sh $(TESTS)
