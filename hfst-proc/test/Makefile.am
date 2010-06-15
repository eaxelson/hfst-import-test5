check_MATERIAL = samibasicout samicmpndout

SAMI_TRANSDUCER = $(top_builddir)/transducers/sami.hfst.ol
SMENOB_TRANSDUCER = $(top_builddir)/transducers/sme-nob.automorf.hfst.ol
OPTIMIZED_LOOKUP = $(top_builddir)/src/hfst-proc

check_SCRIPTS = basic.sh samibasic.sh samicount.sh samicmpnd.sh
TESTS = $(check_SCRIPTS)

basic.sh: Makefile
	@echo '$(OPTIMIZED_LOOKUP) --version 2> /dev/null' > $@
	@echo '$(OPTIMIZED_LOOKUP) --version 2> /dev/null' >> $@
	@chmod +x basic.sh

samibasicout: Makefile
	@echo 'almmolaš+A+Der3+Der/vuohta+N+Sg+Nom#' > $@
	@echo 'almmolašvuohta+N+Sg+Nom#' >> $@
	@echo '' >> $@

# in the following scripts we succeed if SAMI_TRANSDUCER doesn't exist, which it doesn't
# when distchecking - in other words the following checks are only "really done" with
# make check
samibasic.sh: samibasicout Makefile
	@echo 'test -e $(SAMI_TRANSDUCER) || exit 0' > $@
	@echo 'echo almmolašvuohta | $(OPTIMIZED_LOOKUP) $(SAMI_TRANSDUCER) > temp' >> $@
	@echo 'test -z `diff temp samibasicout | head -1` || exit 1' >> $@
	@chmod a+x $@

samicount.sh: Makefile
	@echo 'test -e $(SAMI_TRANSDUCER) || exit 0' > $@
	@echo 'echo láhkaásahus | $(OPTIMIZED_LOOKUP) -N 2 $(SAMI_TRANSDUCER) > temp' >> $@
	@echo '[ `wc -l temp | cut -c 1` = "3" ] || exit 1' >> $@
	@chmod a+x $@

samicmpndout: Makefile
	@echo '^Jávregáttis/Jávri<N><Sg><Nom><Cmp>+gáddi<N><Sg><Loc>/Jávri<N><Sg><Nom><Cmp>+gáddi<N><Sg><Loc>/Jávri<N><Sg><Nom><Cmp>+gáddi<N><Sg><Acc><PxSg3>/Jávri<N><Sg><Nom><Cmp>+gáddi<N><Sg><Gen><PxSg3>/Jávri<N><Sg><Nom><Cmp>+gáddi<N><Sg><Acc><PxSg3>/Jávri<N><Sg><Nom><Cmp>+gáddi<N><Sg><Gen><PxSg3>$' > $@
	@echo '' >> $@

samicmpnd.sh: Makefile
	@echo 'test -e $(SMENOB_TRANSDUCER) || exit 0' > $@
	@echo 'echo Jávregáttis | $(OPTIMIZED_LOOKUP) $(SMENOB_TRANSDUCER) > temp' >> $@
	@echo 'test -z `diff temp samicmpndout | head -1` || exit 1' >> $@
	@chmod a+x $@

CLEANFILES = $(check_SCRIPTS) $(check_MATERIAL) temp

