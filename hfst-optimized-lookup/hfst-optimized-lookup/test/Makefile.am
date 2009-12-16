check_MATERIAL = samibasicout

SAMI_TRANSDUCER = $(top_builddir)/transducers/sami.hfst.ol
OPTIMIZED_LOOKUP = $(top_builddir)/src/hfst-optimized-lookup

check_SCRIPTS = basic.sh samibasic.sh samicount.sh
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
	@echo 'echo láhkaásahus | $(OPTIMIZED_LOOKUP) -n 2 $(SAMI_TRANSDUCER) > temp' >> $@
	@echo '[ `wc -l temp | cut -c 1` = "3" ] || exit 1' >> $@
	@chmod a+x $@



CLEANFILES = $(check_SCRIPTS) $(check_MATERIAL) temp

