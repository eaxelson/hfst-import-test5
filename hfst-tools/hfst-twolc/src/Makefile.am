AM_YFLAGS= -d -o y.tab.c
AM_CXXFLAGS= -O3 -Wall -Wcast-qual -Wno-deprecated
bin_PROGRAMS= htwolcpre htwolcpre2 htwolcpre3
dist_bin_SCRIPTS = hfst-twolc
EXTRA_DIST=hfst-twolc.sh

# Make dested build work
htwolcpre_LDADD = ../../../hfst2/libhfst.la
htwolcpre2_LDADD = $(htwolcpre_LDADD)
htwolcpre3_LDADD = $(htwolcpre_LDADD)
AM_CPPFLAGS = -I../../../ -I ../../../hfst2

htwolcpre_SOURCES= htwolcpre.yy scanner.ll \
	           string_manipulation.cc string_manipulation.h \
	           input_defs.h \
                   InputReader.h InputReader.cc \
	           RuleModifier.h RuleModifier.cc \
	           rule_parts.h rule_parts.cc \
		   VariableComputer.h VariableComputer.cc \
		   VariableContainer.h VariableContainer.cc \
		   RuleVariableComputer.h RuleVariableComputer.cc \
		   GrammarDisplayer.h GrammarDisplayer.cc \
	           grammar_defs.h \
		   CommandLineParser.h CommandLineParser.cc 

htwolcpre2_SOURCES= htwolcpre2.yy scanner2.ll \
	           string_manipulation.cc string_manipulation.h \
		   AlphabetCollector.h AlphabetCollector.cc \
		   GrammarCollector.h GrammarCollector.cc \
	           input_defs.h \
                   InputReader.h InputReader.cc \
	           rule_parts.h rule_parts.cc \
		   VariableComputer.h VariableComputer.cc \
		   VariableContainer.h VariableContainer.cc \
		   RuleVariableComputer.h RuleVariableComputer.cc \
		   GrammarDisplayer.h GrammarDisplayer.cc \
	           grammar_defs.h \
		   CommandLineParser.h CommandLineParser.cc 

htwolcpre3_SOURCES= htwolcpre3.yy scanner3.ll \
	           string_manipulation.h string_manipulation.cc \
		   InputReader.h InputReader.cc \
		   CompileUtilities.h CompileUtilities.cc \
		   RuleCompiler.h RuleCompiler.cc \
		   RightArrowRule.h RightArrowRule.cc \
		   LeftArrowRule.h LeftArrowRule.cc \
		   OtherRules.h OtherRules.cc \
		   GrammarCompiler.h GrammarCompiler.cc \
		   CommandLineParser.h CommandLineParser.cc \
		   RuleStorer.h RuleStorer.cc

hfst-twolc: hfst-twolc.sh
	cat $< | sed -e 's|BINDIR|$(bindir)|' > $@
