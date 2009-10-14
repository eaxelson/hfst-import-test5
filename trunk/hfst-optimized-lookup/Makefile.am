SUBDIRS = src test

EXTRA_DIST = \
	transducers \
	transducers/sami.hfst.ol \
	transducers/COPYING

# the following is done because distcheck doesn't know anything about
# EXTRA_DIST files; the test scripts circumvent failure in this case
# but we still want to run the tests when distchecking
distcheck-hook:
	make check
