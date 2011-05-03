AM_YFLAGS= -d -o y.tab.c
AM_CXXFLAGS= -O3 -g -Wall -Wextra -Wcast-qual -Wno-deprecated -Wfatal-errors
bin_PROGRAMS=htwolcpre1 htwolcpre2 htwolcpre3
bin_SCRIPTS=hfst-twolc hfst-twolc-loc
EXTRA_DIST=hfst-twolc.sh
IO_SRC=$(srcdir)/io_src
STRING_SRC=$(srcdir)/string_src
VARIABLE_SRC=$(srcdir)/variable_src
RULE_SRC=$(srcdir)/rule_src
ALPHABET_SRC=$(srcdir)/alphabet_src
CL_SRC=$(srcdir)/commandline_src
AM_CPPFLAGS = -I${top_srcdir}/libhfst/src
LDADD = $(top_builddir)/libhfst/src/libhfst.la

# the preprocessor scripts do not support options per se, so we whitelist
# them here
AM_INSTALLCHECK_STD_OPTIONS_EXEMPT=htwolcpre1 htwolcpre2 htwolcpre3 \
								   hfst-twolc hfst-twolc-loc

htwolcpre1_SOURCES=htwolcpre1.yy scanner1.ll HfstTwolcDefs.h \
		   $(CL_SRC)/CommandLine.h $(CL_SRC)/CommandLine.cc \
	           $(IO_SRC)/input_defs.h grammar_defs.h \
                   $(IO_SRC)/InputReader.h $(IO_SRC)/InputReader.cc \
	           $(STRING_SRC)/string_manipulation.cc \
		   $(STRING_SRC)/string_manipulation.h \
		   $(VARIABLE_SRC)/VariableValues.h \
		   $(VARIABLE_SRC)/VariableValues.cc \
		   $(VARIABLE_SRC)/VariableValueIterator.h \
		   $(VARIABLE_SRC)/VariableDefs.h \
		   $(VARIABLE_SRC)/VariableContainerBase.h \
		   $(VARIABLE_SRC)/VariableContainer.h \
		   $(VARIABLE_SRC)/VariableBlockContainer.h \
		   $(VARIABLE_SRC)/VariableBlock.h \
		   $(VARIABLE_SRC)/RuleVariablesConstIterator.cc \
		   $(VARIABLE_SRC)/RuleVariablesConstIterator.h \
		   $(VARIABLE_SRC)/RuleVariables.cc \
		   $(VARIABLE_SRC)/RuleVariables.h \
		   $(VARIABLE_SRC)/RuleSymbolVector.cc \
		   $(VARIABLE_SRC)/RuleSymbolVector.h \
		   $(VARIABLE_SRC)/MatchedConstContainerIterator.h \
		   $(VARIABLE_SRC)/MixedConstContainerIterator.h \
		   $(VARIABLE_SRC)/ConstContainerIterator.h

htwolcpre2_SOURCES=htwolcpre2.yy scanner2.ll HfstTwolcDefs.h \
		   $(CL_SRC)/CommandLine.h $(CL_SRC)/CommandLine.cc \
	           $(IO_SRC)/input_defs.h grammar_defs.h \
                   $(IO_SRC)/InputReader.h $(IO_SRC)/InputReader.cc

htwolcpre3_SOURCES=htwolcpre3.yy scanner3.ll HfstTwolcDefs.h \
		   $(CL_SRC)/CommandLine.h $(CL_SRC)/CommandLine.cc \
	           $(IO_SRC)/input_defs.h grammar_defs.h \
                   $(IO_SRC)/InputReader.cc $(IO_SRC)/InputReader.h \
	           $(STRING_SRC)/string_manipulation.cc \
		   $(STRING_SRC)/string_manipulation.h \
		   $(ALPHABET_SRC)/Alphabet.cc $(ALPHABET_SRC)/Alphabet.h \
		   $(RULE_SRC)/ConflictResolvingLeftArrowRule.cc \
		   $(RULE_SRC)/ConflictResolvingLeftArrowRule.h \
		   $(RULE_SRC)/ConflictResolvingRightArrowRule.cc \
		   $(RULE_SRC)/ConflictResolvingRightArrowRule.h \
		   $(RULE_SRC)/LeftArrowRule.cc \
		   $(RULE_SRC)/LeftArrowRule.h \
		   $(RULE_SRC)/LeftArrowRuleContainer.cc \
		   $(RULE_SRC)/LeftArrowRuleContainer.h \
		   $(RULE_SRC)/LeftRestrictionArrowRule.cc \
		   $(RULE_SRC)/LeftRestrictionArrowRule.h \
		   $(RULE_SRC)/OtherSymbolTransducer.cc \
		   $(RULE_SRC)/OtherSymbolTransducer.h \
		   $(RULE_SRC)/RightArrowRule.cc \
		   $(RULE_SRC)/RightArrowRule.h \
		   $(RULE_SRC)/RightArrowRuleContainer.cc \
		   $(RULE_SRC)/RightArrowRuleContainer.h \
		   $(RULE_SRC)/Rule.cc \
		   $(RULE_SRC)/Rule.h \
		   $(RULE_SRC)/RuleContainer.cc \
		   $(RULE_SRC)/RuleContainer.h \
		   $(RULE_SRC)/TwolCGrammar.cc \
		   $(RULE_SRC)/TwolCGrammar.h

hfst-twolc$(EXEEXT): hfst-twolc.sh
	cat $< | sed -e 's|BINDIR|$(bindir)|' > $@
	chmod ogu+rx hfst-twolc

hfst-twolc-loc$(EXEEXT): hfst-twolc.sh
	cat $< | sed -e 's|BINDIR|$(PWD)|' > $@
	chmod ogu+rx hfst-twolc-loc
