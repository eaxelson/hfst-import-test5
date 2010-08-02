HYPH_SOURCES= \
			  hyph-bg.tex hyph-ca.tex hyph-cop.tex hyph-cs.tex hyph-cy.tex \
			  hyph-da.tex hyph-de-1901.tex hyph-de-1996.tex \
			  hyph-el-monoton.tex hyph-el-polyton.tex hyph-en-gb.tex \
			  hyph-en-us.tex hyph-es.tex hyph-et.tex hyph-eu.tex \
			  hyph-fi.tex hyph-fr.tex hyph-ga.tex hyph-gl.tex hyph-grc.tex \
			  hyph-hr.tex hyph-hu.tex \
			  hyph-hsb.tex hyph-ia.tex hyph-id.tex hyph-is.tex hyph-it.tex \
			  hyph-la.tex hyph-mn-cyrl.tex hyph-mn-cyrl-x-2a.tex hyph-nb.tex \
			  hyph-nl.tex hyph-nn.tex hyph-no.tex hyph-pl.tex hyph-pt.tex \
			  hyph-ro.tex hyph-ru.tex hyph-sa.tex hyph-sh-cyrl.tex \
			  hyph-sh-latn.tex hyph-sk.tex hyph-sl.tex hyph-sr-cyrl.tex \
			  hyph-sv.tex hyph-tr.tex hyph-uk.tex hyph-zh-latn.tex
HYPH_TARGETS= \
			  hyph-bg.hfst hyph-ca.hfst hyph-cop.hfst hyph-cs.hfst  \
			  hyph-cy.hfst hyph-da.hfst hyph-de-1901.hfst hyph-de-1996.hfst \
			  hyph-el-monoton.hfst hyph-el-polyton.hfst hyph-en-gb.hfst \
			  hyph-en-us.hfst hyph-es.hfst hyph-et.hfst  \
			  hyph-eu.hfst hyph-fi.hfst hyph-fr.hfst hyph-ga.hfst hyph-gl.hfst \
			  hyph-grc.hfst hyph-hu.hfst \
			  hyph-hr.hfst hyph-hsb.hfst hyph-ia.hfst hyph-id.hfst  \
			  hyph-is.hfst hyph-it.hfst hyph-la.hfst hyph-mn-cyrl.hfst \
			  hyph-mn-cyrl-x-2a.hfst hyph-nb.hfst hyph-nl.hfst hyph-nn.hfst \
			  hyph-no.hfst hyph-pl.hfst hyph-pt.hfst hyph-ro.hfst hyph-ru.hfst \
			  hyph-sa.hfst hyph-sh-cyrl.hfst hyph-sh-latn.hfst hyph-sk.hfst \
			  hyph-sl.hfst hyph-sr-cyrl.hfst hyph-sv.hfst hyph-tr.hfst \
			  hyph-uk.hfst hyph-zh-latn.hfst \
			  hyph-bg.hfst.ol hyph-ca.hfst.ol hyph-cop.hfst.ol hyph-cs.hfst.ol \
			  hyph-cy.hfst.ol hyph-da.hfst.ol hyph-de-1901.hfst.ol \
			  hyph-de-1996.hfst.ol hyph-el-monoton.hfst.ol \
			  hyph-el-polyton.hfst.ol hyph-en-gb.hfst.ol hyph-en-us.hfst.ol \
			  hyph-es.hfst.ol hyph-et.hfst.ol hyph-eu.hfst.ol \
			  hyph-fi.hfst.ol hyph-fr.hfst.ol hyph-ga.hfst.ol hyph-gl.hfst.ol \
			  hyph-grc.hfst.ol hyph-hu.hfst.ol \
			  hyph-hr.hfst.ol hyph-hsb.hfst.ol hyph-ia.hfst.ol hyph-id.hfst.ol \
			  hyph-is.hfst.ol hyph-it.hfst.ol hyph-la.hfst.ol \
			  hyph-mn-cyrl.hfst.ol hyph-mn-cyrl-x-2a.hfst.ol hyph-nb.hfst.ol \
			  hyph-nl.hfst.ol hyph-nn.hfst.ol hyph-no.hfst.ol hyph-pl.hfst.ol \
			  hyph-pt.hfst.ol hyph-ro.hfst.ol hyph-ru.hfst.ol hyph-sa.hfst.ol \
			  hyph-sh-cyrl.hfst.ol hyph-sh-latn.hfst.ol hyph-sk.hfst.ol \
			  hyph-sl.hfst.ol hyph-sr-cyrl.hfst.ol hyph-sv.hfst.ol \
			  hyph-tr.hfst.ol hyph-uk.hfst.ol hyph-zh-latn.hfst.ol 

HUNSPELL_DICS=\
de-BE.dic  de_DE.dic  de-DE.dic  de-LU.dic  en_US.dic  en-US.dic  fr_BE.dic \
fr_CA.dic  fr_CH.dic  fr_FR.dic  fr_LU.dic  fr_MC.dic  hu_HU.dic  se.dic \
se_FI.dic  se_NO.dic  se_SE.dic  
HUNSPELL_AFFS=\
de-BE.aff  de_DE.aff  de-DE.aff  de-LU.aff  en_US.aff  en-US.aff  fr_BE.aff \
fr_CA.aff  fr_CH.aff  fr_FR.aff  fr_LU.aff  fr_MC.aff  hu_HU.aff  se.aff \
se_FI.aff  se_NO.aff  se_SE.aff  
HUNSPELL_TARGETS=\
de-BE.hfst  de_DE.hfst  de-DE.hfst  de-LU.hfst  en_US.hfst  en-US.hfst \
fr_BE.hfst fr_CA.hfst  fr_CH.hfst  fr_FR.hfst  fr_LU.hfst  fr_MC.hfst \
hu_HU.hfst  se.hfst se_FI.hfst  se_NO.hfst  se_SE.hfst  \


all: hunspell-dic2lexc hunspell-aff2lexc

hyph-utf8-hfst.tar.gz: $(HYPH_TARGETS)
	-mkdir -p hyph-utf8-hfst
	cp -v $(HYPH_TARGETS) README.hyph hyph-utf8-hfst
	tar zcvf $@ hyph-utf8-hfst


# tex pattern translation
%.hfst: %.tex
	python texpat2twolc.py --input=$< --output=$*
	hfst-twolc -w -i $*.1.htwolc -o $*.1.hfst -v
	hfst-twolc -w -i $*.2.htwolc -o $*.2.hfst -v
	hfst-twolc -w -i $*.3.htwolc -o $*.3.hfst -v
	hfst-twolc -w -i $*.4.htwolc -o $*.4.hfst -v
	hfst-twolc -w -i $*.5.htwolc -o $*.5.hfst -v
	hfst-twolc -w -i $*.6.htwolc -o $*.6.hfst -v
	hfst-twolc -w -i $*.7.htwolc -o $*.7.hfst -v
	hfst-twolc -w -i $*.8.htwolc -o $*.8.hfst -v
	hfst-twolc -w -i $*.9.htwolc -o $*.9.hfst -v
	hfst-compose $*.1.hfst $*.2.hfst -v |\
		hfst-compose $*.3.hfst -v |\
		hfst-compose $*.4.hfst -v |\
		hfst-compose $*.5.hfst -v |\
		hfst-compose $*.6.hfst -v |\
		hfst-compose $*.7.hfst -v |\
		hfst-compose $*.8.hfst -v |\
		hfst-compose $*.9.hfst -v |\
		hfst-minimize -v -o $@

%.hfst.ol: %.hfst
	hfst-lookup-optimize < $< > $@

# hunspell compilation

%.hfst: %.aff %.dic
	if grep -m 1 '^AF' $*.aff > /dev/null ; then \
		./hunspell-dic2lexc AF $*.dic $*.dic.lexc ; \
	elif grep -m 1 'FLAG *UTF-8' $*.aff; then \
		./hunspell-dic2lexc UTF-8 $*.dic $*.dic.lexc ; \
	elif grep -m 1 'FLAG *LONG' $*.aff ; then \
		./hunspell-dic2lexc LONG $*.dic $*.dic.lexc ; \
	elif grep -m 1 'FLAG *NUM' $*.aff ; then \
		./hunspell-dic2lexc NUM $*.dic $*.dic.lexc ; \
	else \
		./hunspell-dic2lexc ASCII $*.dic $*.dic.lexc ; \
	fi
	./hunspell-aff2lexc $*.aff $*.aff.lexc $*.rules.twolc $*.err.lexc $*.symbols.lexc $*.symbols.twolc
	hfst-lexc -w -v -o $*.lex.hfst $*.symbols.lexc $*.dic.lexc $*.aff.lexc
	hfst-lexc -w -v -o $*.1errm.hfst hunspell_one_error.lexc $*.err.lexc
	cat $*.symbols.twolc $*.rules.twolc > $*.twolc
	hfst-twolc --resolve -w -v -o $*.twol.hfst $*.twolc
	hfst-compose-intersect -v -l $*.lex.hfst $*.twol.hfst |\
		hfst-minimize -v -o $*.dbg.hfst
	hfst-project -p lower -v $*.dbg.hfst |\
		hfst-minimize -v -o $@
	hfst-lookup-optimize -v -i $@ -o $@.ol
	cat $*.1errm.hfst $@  > $*.sug.hfst

# hunspell tools

hunspell-aff2lexc: hunspell-aff.tab.o hunspell-aff.o
	g++ -o $@ $^

hunspell-aff.o: hunspell-aff.cc hunspell-aff.tab.hh

hunspell-aff.cc: hunspell-aff.ll
	flex -o $@ $<

hunspell-aff.tab.cc: hunspell-aff.yy
	bison -d $<

hunspell-aff.tab.hh: hunspell-aff.yy
	bison -d $<

hunspell-dic2lexc: hunspell-dic.tab.o hunspell-dic.o
	g++ -o $@ $^

hunspell-dic.o: hunspell-dic.cc hunspell-dic.tab.hh

hunspell-dic.cc: hunspell-dic.ll
	flex -o $@ $<

hunspell-dic.tab.cc: hunspell-dic.yy
	bison -d $<

hunspell-dic.tab.hh: hunspell-dic.yy
	bison -d $<
