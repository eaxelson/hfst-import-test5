## Process this file with automake to produce Makefile.in
## Author: Tommi A Pirinen <tommi.pirinen@helsinki.fi>
## Licence: GPL v3 (not newer)

SUBDIRS = src doc test man po
EXTRA_DIST = HACKING
ACLOCAL_AMFLAGS=-I m4

BINDIST_TARGETS=src/hfst-lexc AUTHORS ChangeLog COPYING LISENSSI NEWS README \
	THANKS TODO

.PHONY: doc valgrind bindist bindist-bzip2 bindist-gzip bindist-zip \
	bindist-dir

doc:
	cd doc && $(MAKE) $(AM_MAKEFLAFS) $@

valgrind:
	cd src/tests && $(MAKE) $(AM_MAKEFLAGS) $@

bindist: bindist-bzip2

bindist-bzip2: bindist-dir
	cd .bindist && tar -cv * | bzip2 -9 -c > ../hfst-lexc-bin.tar.bz2

bindist-gzip: bindist-dir
	cd .bindist && tar -cv * | gzip -9 -c > ../hfst-lexc-bin.tar.gz

bindist-zip: bindist-dir
	cd .bindist && zip -rq ../hfst-lexc-bin.zip *

bindist-dir: $(BINDIST_TARGETS)
	-mkdir .bindist
	-mkdir .bindist/hfst-lexc-bin
	cp $^ .bindist/hfst-lexc-bin/

clean-local:
	-rm -rf .bindist
