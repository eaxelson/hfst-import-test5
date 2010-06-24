check_MATERIAL = samibasicout samicmpndout capsin caps1out caps2out caps3out caps4out

SAMI_TRANSDUCER = $(top_builddir)/transducers/sami.hfst.ol
SMENOB_TRANSDUCER = $(top_builddir)/transducers/sme-nob.automorf.hfst.ol
CAPS_TRANSDUCER = $(top_builddir)/transducers/capitalization-test.hfst.ol
CAPS_GEN_TRANSDUCER = $(top_builddir)/transducers/capitalization-test-gen.hfst.ol
PROC = $(top_builddir)/src/hfst-proc

check_SCRIPTS = basic.sh samibasic.sh samicount.sh samicmpnd.sh caps.sh
TESTS = $(check_SCRIPTS)

basic.sh: Makefile
	@echo '$(PROC) --version 2> /dev/null' > $@
	@echo '$(PROC) --version 2> /dev/null' >> $@
	@chmod +x basic.sh

samibasicout: Makefile
	@echo 'almmolaš+A+Der3+Der/vuohta+N+Sg+Nom#' > $@
	@echo 'almmolašvuohta+N+Sg+Nom#' >> $@
	@echo '' >> $@

samicmpndout: Makefile
	@echo '^Jávregáttis/Jávri<N><Sg><Nom><Cmp>+gáddi<N><Sg><Loc>/Jávri<N><Sg><Nom><Cmp>+gáddi<N><Sg><Loc>/Jávri<N><Sg><Nom><Cmp>+gáddi<N><Sg><Acc><PxSg3>/Jávri<N><Sg><Nom><Cmp>+gáddi<N><Sg><Gen><PxSg3>/Jávri<N><Sg><Nom><Cmp>+gáddi<N><Sg><Acc><PxSg3>/Jávri<N><Sg><Nom><Cmp>+gáddi<N><Sg><Gen><PxSg3>$' > $@
	@echo '' >> $@

capsin: Makefile
	@echo 'test' > $@
	@echo 'Test' >> $@
	@echo 'TEST' >> $@
	@echo 'TeSt' >> $@
	@echo 'tesT' >> $@
	@echo 'TEst' >> $@

caps1out: Makefile
	@echo '^test/test+n$' > $@
	@echo '^Test/Test+np/Test+n$' >> $@
	@echo '^TEST/TEST+np/TEST+n$' >> $@
	@echo '^TeSt/Test+np/Test+n$' >> $@
	@echo '^tesT/test+n$' >> $@
	@echo '^TEst/Test+np/Test+n$' >> $@

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
	@echo '^test/test+n$' > $@
	@echo '^Test/Test+np$' >> $@
	@echo '^TEST/*TEST$' >> $@
	@echo '^TeSt/*TeSt$' >> $@
	@echo '^tesT/*tesT$' >> $@
	@echo '^TEst/*TEst$' >> $@

caps4out: Makefile
	@echo '^test/test+n$' > $@
	@echo '^Test/Test+np/test+n$' >> $@
	@echo '^TEST/Test+np/test+n$' >> $@
	@echo '^TeSt/Test+np/test+n$' >> $@
	@echo '^tesT/test+n$' >> $@
	@echo '^TEst/Test+np/test+n$' >> $@

# in the following scripts we succeed if SAMI_TRANSDUCER doesn't exist, which it doesn't
# when distchecking - in other words the following checks are only "really done" with
# make check
samibasic.sh: samibasicout Makefile
	@echo 'test -e $(SAMI_TRANSDUCER) || exit 0' > $@
	@echo 'echo almmolašvuohta | $(PROC) $(SAMI_TRANSDUCER) > temp' >> $@
	@echo 'test -z `diff temp samibasicout | head -1` || exit 1' >> $@
	@chmod a+x $@

samicount.sh: Makefile
	@echo 'test -e $(SAMI_TRANSDUCER) || exit 0' > $@
	@echo 'echo láhkaásahus | $(PROC) -N 2 $(SAMI_TRANSDUCER) > temp' >> $@
	@echo '[ `wc -l temp | cut -c 1` = "3" ] || exit 1' >> $@
	@chmod a+x $@

samicmpnd.sh: samicmpndout Makefile
	@echo 'test -e $(SMENOB_TRANSDUCER) || exit 0' > $@
	@echo 'echo Jávregáttis | $(PROC) $(SMENOB_TRANSDUCER) > temp' >> $@
	@echo 'test -z `diff temp samicmpndout | head -1` || exit 1' >> $@
	@chmod a+x $@

caps.sh: capsin caps1out caps2out caps3out caps4out Makefile
	@echo 'test -e $(CAPS_TRANSDUCER) || exit 0' > $@
	@echo 'test -e $(CAPS_GEN_TRANSDUCER) || exit 0' > $@
	@echo 'cat capsin | $(PROC) $(CAPS_TRANSDUCER) > temp' >> $@
	@echo 'test -z `diff temp caps1out | head -6` || exit 1' >> $@
	@echo "cat capsin | $(PROC) $(CAPS_TRANSDUCER) | cut -f2- -d'/'  | sed 's/\$//g' | sed 's/\//\n/g'  | sed 's/^/^/g' | sed 's/$/$/g' | $(PROC) -g $(CAPS_GEN_TRANSDUCER) > temp'" >> $@
	@echo 'test -z `diff temp caps2out | head -10` || exit 1' >> $@
	@echo 'cat capsin | $(PROC) -c $(CAPS_TRANSDUCER) > temp' >> $@
	@echo 'test -z `diff temp caps3out | head -6` || exit 1' >> $@
	@echo 'cat capsin | $(PROC) -w $(CAPS_TRANSDUCER) > temp' >> $@
	@echo 'test -z `diff temp caps4out | head -6` || exit 1' >> $@
	@chmod a+x $@

CLEANFILES = $(check_SCRIPTS) $(check_MATERIAL) temp

