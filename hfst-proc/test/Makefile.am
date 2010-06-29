check_MATERIAL = samibasicout samicmpndout capsin caps1out caps2out caps3out \
	caps4out cmpndfilterout

SMENOB_TRANSDUCER = $(top_builddir)/transducers/sme-nob.automorf.hfst.ol
CAPS_TRANSDUCER = $(top_builddir)/transducers/capitalization-test.hfst.ol
CAPS_GEN_TRANSDUCER = $(top_builddir)/transducers/capitalization-test-gen.hfst.ol
PROC = $(top_builddir)/src/hfst-proc

check_SCRIPTS = basic.sh samibasic.sh samicmpnd.sh caps.sh \
	cmpndfilter.sh
TESTS = $(check_SCRIPTS)

# in the following scripts we succeed if SAMI_TRANSDUCER doesn't exist, which it doesn't
# when distchecking - in other words the following checks are only "really done" with
# make check

# covers the application starting up
basic.sh: Makefile
	@echo '$(PROC) --version 2> /dev/null' > $@
	@echo '$(PROC) --version 2> /dev/null' >> $@
	@chmod +x basic.sh

# covers basic analysis
samibasicout: Makefile
	@echo '^almmolašvuohta/almmolaš<A><Der3><Der_vuohta><N><Sg><Nom>$$' > $@

samibasic.sh: samibasicout Makefile
	@echo 'test -e $(SAMI_TRANSDUCER) || exit 0' > $@
	@echo 'echo almmolašvuohta | $(PROC) $(SMENOB_TRANSDUCER) > temp' >> $@
	@echo 'test -z `diff temp samibasicout | head -1` || exit 1' >> $@
	@chmod a+x $@

# covers compound analyses
samicmpndout: Makefile
	@echo '^Jávregáttis/Jávri<N><Sg><Nom><Cmp>+gáddi<N><Sg><Acc><PxSg3>/Jávri<N><Sg><Nom><Cmp>+gáddi<N><Sg><Gen><PxSg3>/Jávri<N><Sg><Nom><Cmp>+gáddi<N><Sg><Loc>$$' > $@

samicmpnd.sh: samicmpndout Makefile
	@echo 'test -e $(SMENOB_TRANSDUCER) || exit 0' > $@
	@echo 'echo Jávregáttis | $(PROC) $(SMENOB_TRANSDUCER) > temp' >> $@
	@echo 'test -z `diff temp samicmpndout | head -1` || exit 1' >> $@
	@chmod a+x $@

# covers capitalization preservation in various modes
capsin: Makefile
	@echo 'test' > $@
	@echo 'Test' >> $@
	@echo 'TEST' >> $@
	@echo 'TeSt' >> $@
	@echo 'tesT' >> $@
	@echo 'TEst' >> $@

caps1out: Makefile
	@echo '^test/test+n$$' > $@
	@echo '^Test/Test+n/Test+np$$' >> $@
	@echo '^TEST/TEST+n/TEST+np$$' >> $@
	@echo '^TeSt/Test+n/Test+np$$' >> $@
	@echo '^tesT/test+n$$' >> $@
	@echo '^TEst/Test+n/Test+np$$' >> $@

caps2out: Makefile
	@echo 'test' > $@
	@echo 'Test' >> $@
	@echo 'Test' >> $@
	@echo 'TEST' >> $@
	@echo 'TEST' >> $@
	@echo 'Test' >> $@
	@echo 'Test' >> $@
	@echo 'test' >> $@
	@echo 'Test' >> $@
	@echo 'Test' >> $@

caps3out: Makefile
	@echo '^test/test+n$$' > $@
	@echo '^Test/Test+np$$' >> $@
	@echo '^TEST/*TEST$$' >> $@
	@echo '^TeSt/*TeSt$$' >> $@
	@echo '^tesT/*tesT$$' >> $@
	@echo '^TEst/*TEst$$' >> $@

caps4out: Makefile
	@echo '^test/test+n$$' > $@
	@echo '^Test/test+n/Test+np$$' >> $@
	@echo '^TEST/test+n/Test+np$$' >> $@
	@echo '^TeSt/test+n/Test+np$$' >> $@
	@echo '^tesT/test+n$$' >> $@
	@echo '^TEst/test+n/Test+np$$' >> $@
	
caps.sh: capsin caps1out caps2out caps3out caps4out Makefile
	@echo 'test -e $(CAPS_TRANSDUCER) || exit 0' > $@
	@echo 'test -e $(CAPS_GEN_TRANSDUCER) || exit 0' >> $@
	@echo 'cat capsin | $(PROC) $(CAPS_TRANSDUCER) > temp' >> $@
	@echo 'test -z "`diff temp caps1out | head -1`" || exit 1' >> $@
	@echo "cat capsin | $(PROC) $(CAPS_TRANSDUCER) | cut -f2- -d'/'  | sed 's/\\$$//g' | sed 's/\\//\\n/g' | sed 's/^/^/g' | sed 's/$$/$$/g' | $(PROC) -g $(CAPS_GEN_TRANSDUCER) > temp" >> $@
	@echo 'test -z `diff temp caps2out | head -1` || exit 1' >> $@
	@echo 'cat capsin | $(PROC) -c $(CAPS_TRANSDUCER) > temp' >> $@
	@echo 'test -z `diff temp caps3out | head -1` || exit 1' >> $@
	@echo 'cat capsin | $(PROC) -w $(CAPS_TRANSDUCER) > temp' >> $@
	@echo 'test -z `diff temp caps4out | head -1` || exit 1' >> $@
	@chmod a+x $@

# covers filtering/preservation of compound analyses
cmpndfilterout: Makefile
	@echo '^strategiijaplána/strategiijaplána<N><Sg><Nom>/strategiijaplána<N><Sg><Acc>/strategiijaplána<N><Sg><Gen>$$' > $@
	@echo '^strategiijaplána/strategiijaplána<N><Sg><Nom>/strategiijaplána<N><Sg><Acc>/strategiijaplána<N><Sg><Gen>/strategiija<N><Sg><Nom><Cmp>+plána<N><Sg><Nom>/strategiija<N><Sg><Nom><Cmp>+plána<N><Sg><Acc>/strategiija<N><Sg><Nom><Cmp>+plána<N><Sg><Gen>/strategiija<N><Sg><Gen><Cmp>+plána<N><Sg><Nom>/strategiija<N><Sg><Gen><Cmp>+plána<N><Sg><Acc>/strategiija<N><Sg><Gen><Cmp>+plána<N><Sg><Gen>$$' >> $@

cmpndfilter.sh: cmpndfilterout Makefile
	@echo 'test -e $(SMENOB_TRANSDUCER) || exit 0' > $@
	@echo 'echo strategiijaplána | $(PROC) $(SMENOB_TRANSDUCER) > temp' >> $@
	@echo 'echo strategiijaplána | $(PROC) -k $(SMENOB_TRANSDUCER) >> temp' >> $@
	@echo 'test -z `diff temp cmpndfilterout | head -1` || exit 1' >> $@
	@chmod a+x $@
	
CLEANFILES = $(check_SCRIPTS) $(check_MATERIAL) temp

