HLEXC=$(HFSTHOME)hfst-lexc
HTWOLC=$(HFSTHOME)hfst-twolc
HMIN=$(HFSTHOME)hfst-minimize
HOPT=$(HFSTHOME)hfst-lookup-optimize
HIC=$(HFSTHOME)hfst-compose-intersect
HPROJ=$(HFSTHOME)hfst-project
HCOMP=$(HFSTHOME)hfst-compose

HYPH_SOURCES= \
			  hyph-bg.tex hyph-ca.tex hyph-cop.tex hyph-cs.tex hyph-cy.tex \
			  hyph-da.tex hyph-de-1901.tex hyph-de-1996.tex \
			  hyph-el-monoton.tex hyph-el-polyton.tex hyph-en-gb.tex \
			  hyph-en-us.tex hyph-es.tex hyph-et.tex hyph-eu.tex \
			  hyph-fi.tex hyph-fr.tex hyph-ga.tex hyph-gl.tex hyph-grc.tex \
			  hyph-hr.tex hyph-hu.tex \
			  hyph-hsb.tex hyph-ia.tex hyph-id.tex hyph-is.tex hyph-it.tex \
			  hyph-la.tex hyph-mn-cyrl.tex hyph-mn-cyrl-x-2a.tex hyph-nb.tex \
			  hyph-nl.tex hyph-nn.tex hyph-pl.tex hyph-pt.tex \
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
			  hyph-pl.hfst hyph-pt.hfst hyph-ro.hfst hyph-ru.hfst \
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
			  hyph-nl.hfst.ol hyph-nn.hfst.ol hyph-pl.hfst.ol \
			  hyph-pt.hfst.ol hyph-ro.hfst.ol hyph-ru.hfst.ol hyph-sa.hfst.ol \
			  hyph-sh-cyrl.hfst.ol hyph-sh-latn.hfst.ol hyph-sk.hfst.ol \
			  hyph-sl.hfst.ol hyph-sr-cyrl.hfst.ol hyph-sv.hfst.ol \
			  hyph-tr.hfst.ol hyph-uk.hfst.ol hyph-zh-latn.hfst.ol 

HUNSPELL_DICS=\
af-ZA/af-ZA.dic am/am.dic az/az.dic bn-BD/bn-BD.dic ca-ES/ca-ES.dic csb/csb.dic\
cs-CZ/cs-CZ.dic de-AT/de-AT.dic de-BE/de-BE.dic de-CH/de-CH.dic de-DE/de-DE.dic\
de-DE/de-DE.dic de-LU/de-LU.dic el-GR/el-GR.dic en-GB/en-GB.dic en-US/en-US.dic\
en-US/en-US.dic en-ZA/en-ZA.dic eo-l3/eo-l3.dic es-ES/es-ES.dic es-MX/es-MX.dic\
et-EE/et-EE.dic fa/fa.dic fo-FO/fo-FO.dic fr-BE/fr-BE.dic fr-CA/fr-CA.dic\
fr-CH/fr-CH.dic fr-FR/fr-FR.dic fr-LU/fr-LU.dic fr-MC/fr-MC.dic\
fur-IT/fur-IT.dic fy-NL/fy-NL.dic ga-IE/ga-IE.dic gd-GB/gd-GB.dic\
gsc-FR/gsc-FR.dic gu-IN/gu-IN.dic he-IL/he-IL.dic hi-IN/hi-IN.dic\
hr-HR/hr-HR.dic hu-HU/hu-HU.dic ia/ia.dic id-ID/id-ID.dic is/is.dic\
is-IS/is-IS.dic it-IT/it-IT.dic ku-TR/ku-TR.dic la/la.dic ln-CD/ln-CD.dic\
lt-LT/lt-LT.dic mg-MG/mg-MG.dic ms-MY/ms-MY.dic ml-IN/ml-IN.dic nr-ZA/nr-ZA.dic\
ne-NP/ne-NP.dic nl-NL/nl-NL.dic ns-ZA/ns-ZA.dic ny-MW/ny-MW.dic oc-FR/oc-FR.dic\
or/or.dic pa/pa.dic pl-PL/pl-PL.dic pt-BR/pt-BR.dic pt-PT/pt-PT.dic\
ro-RO/ro-RO.dic ru-RU/ru-RU.dic rw-RW/rw-RW.dic se/se.dic se-FI/se-FI.dic\
se-NO/se-NO.dic se-SE/se-SE.dic sk-SK/sk-SK.dic sl-SI/sl-SI.dic sv-SE/sv-SE.dic\
sw-KE/sw-KE.dic ta/ta.dic th-TH/th-TH.dic tl-PH/tl-PH.dic tn-ZA/tn-ZA.dic\
ts-ZA/ts-ZA.dic uk-UA/uk-UA.dic uz/uz.dic vi-VN/vi-VN.dic

HUNSPELL_AFFS=\
af-ZA/af-ZA.aff am/am.aff az/az.aff bn-BD/bn-BD.aff ca-ES/ca-ES.aff csb/csb.aff\
cs-CZ/cs-CZ.aff de-AT/de-AT.aff de-BE/de-BE.aff de-CH/de-CH.aff de-DE/de-DE.aff\
de-DE/de-DE.aff de-LU/de-LU.aff el-GR/el-GR.aff en-GB/en-GB.aff en-US/en-US.aff\
en-US/en-US.aff en-ZA/en-ZA.aff eo-l3/eo-l3.aff es-ES/es-ES.aff es-MX/es-MX.aff\
et-EE/et-EE.aff fa/fa.aff fo-FO/fo-FO.aff fr-BE/fr-BE.aff fr-CA/fr-CA.aff\
fr-CH/fr-CH.aff fr-FR/fr-FR.aff fr-LU/fr-LU.aff fr-MC/fr-MC.aff\
fur-IT/fur-IT.aff fy-NL/fy-NL.aff ga-IE/ga-IE.aff gd-GB/gd-GB.aff\
gsc-FR/gsc-FR.aff gu-IN/gu-IN.aff he-IL/he-IL.aff hi-IN/hi-IN.aff\
hr-HR/hr-HR.aff hu-HU/hu-HU.aff ia/ia.aff id-ID/id-ID.aff is/is.aff\
is-IS/is-IS.aff it-IT/it-IT.aff ku-TR/ku-TR.aff la/la.aff ln-CD/ln-CD.aff\
lt-LT/lt-LT.aff mg-MG/mg-MG.aff ms-MY/ms-MY.aff ml-IN/ml-IN.aff nr-ZA/nr-ZA.aff\
ne-NP/ne-NP.aff nl-NL/nl-NL.aff ns-ZA/ns-ZA.aff ny-MW/ny-MW.aff oc-FR/oc-FR.aff\
or/or.aff pa/pa.aff pl-PL/pl-PL.aff pt-BR/pt-BR.aff pt-PT/pt-PT.aff\
ro-RO/ro-RO.aff ru-RU/ru-RU.aff rw-RW/rw-RW.aff se/se.aff se-FI/se-FI.aff\
se-NO/se-NO.aff se-SE/se-SE.aff sk-SK/sk-SK.aff sl-SI/sl-SI.aff sv-SE/sv-SE.aff\
sw-KE/sw-KE.aff ta/ta.aff th-TH/th-TH.aff tl-PH/tl-PH.aff tn-ZA/tn-ZA.aff\
ts-ZA/ts-ZA.aff uk-UA/uk-UA.aff uz/uz.aff vi-VN/vi-VN.aff
HUNSPELL_TARGETS=\
af-ZA/af-ZA.hfst am/am.hfst az/az.hfst bn-BD/bn-BD.hfst ca-ES/ca-ES.hfst csb/csb.hfst\
cs-CZ/cs-CZ.hfst de-AT/de-AT.hfst de-BE/de-BE.hfst de-CH/de-CH.hfst de-DE/de-DE.hfst\
de-DE/de-DE.hfst de-LU/de-LU.hfst el-GR/el-GR.hfst en-GB/en-GB.hfst en-US/en-US.hfst\
en-US/en-US.hfst en-ZA/en-ZA.hfst eo-l3/eo-l3.hfst es-ES/es-ES.hfst es-MX/es-MX.hfst\
et-EE/et-EE.hfst fa/fa.hfst fo-FO/fo-FO.hfst fr-BE/fr-BE.hfst fr-CA/fr-CA.hfst\
fr-CH/fr-CH.hfst fr-FR/fr-FR.hfst fr-LU/fr-LU.hfst fr-MC/fr-MC.hfst\
fur-IT/fur-IT.hfst fy-NL/fy-NL.hfst ga-IE/ga-IE.hfst gd-GB/gd-GB.hfst\
gsc-FR/gsc-FR.hfst gu-IN/gu-IN.hfst he-IL/he-IL.hfst hi-IN/hi-IN.hfst\
hr-HR/hr-HR.hfst hu-HU/hu-HU.hfst ia/ia.hfst id-ID/id-ID.hfst is/is.hfst\
is-IS/is-IS.hfst it-IT/it-IT.hfst ku-TR/ku-TR.hfst la/la.hfst ln-CD/ln-CD.hfst\
lt-LT/lt-LT.hfst mg-MG/mg-MG.hfst ms-MY/ms-MY.hfst ml-IN/ml-IN.hfst nr-ZA/nr-ZA.hfst\
ne-NP/ne-NP.hfst nl-NL/nl-NL.hfst ns-ZA/ns-ZA.hfst ny-MW/ny-MW.hfst oc-FR/oc-FR.hfst\
or/or.hfst pa/pa.hfst pl-PL/pl-PL.hfst pt-BR/pt-BR.hfst pt-PT/pt-PT.hfst\
ro-RO/ro-RO.hfst ru-RU/ru-RU.hfst rw-RW/rw-RW.hfst se/se.hfst se-FI/se-FI.hfst\
se-NO/se-NO.hfst se-SE/se-SE.hfst sk-SK/sk-SK.hfst sl-SI/sl-SI.hfst sv-SE/sv-SE.hfst\
sw-KE/sw-KE.hfst ta/ta.hfst th-TH/th-TH.hfst tl-PH/tl-PH.hfst tn-ZA/tn-ZA.hfst\
ts-ZA/ts-ZA.hfst uk-UA/uk-UA.hfst uz/uz.hfst vi-VN/vi-VN.hfst \
af-ZA/af-ZA.hfst.ol am/am.hfst.ol az/az.hfst.ol bn-BD/bn-BD.hfst.ol ca-ES/ca-ES.hfst.ol csb/csb.hfst.ol\
cs-CZ/cs-CZ.hfst.ol de-AT/de-AT.hfst.ol de-BE/de-BE.hfst.ol de-CH/de-CH.hfst.ol de-DE/de-DE.hfst.ol\
de-DE/de-DE.hfst.ol de-LU/de-LU.hfst.ol el-GR/el-GR.hfst.ol en-GB/en-GB.hfst.ol en-US/en-US.hfst.ol\
en-US/en-US.hfst.ol en-ZA/en-ZA.hfst.ol eo-l3/eo-l3.hfst.ol es-ES/es-ES.hfst.ol es-MX/es-MX.hfst.ol\
et-EE/et-EE.hfst.ol fa/fa.hfst.ol fo-FO/fo-FO.hfst.ol fr-BE/fr-BE.hfst.ol fr-CA/fr-CA.hfst.ol\
fr-CH/fr-CH.hfst.ol fr-FR/fr-FR.hfst.ol fr-LU/fr-LU.hfst.ol fr-MC/fr-MC.hfst.ol\
fur-IT/fur-IT.hfst.ol fy-NL/fy-NL.hfst.ol ga-IE/ga-IE.hfst.ol gd-GB/gd-GB.hfst.ol\
gsc-FR/gsc-FR.hfst.ol gu-IN/gu-IN.hfst.ol he-IL/he-IL.hfst.ol hi-IN/hi-IN.hfst.ol\
hr-HR/hr-HR.hfst.ol hu-HU/hu-HU.hfst.ol ia/ia.hfst.ol id-ID/id-ID.hfst.ol is/is.hfst.ol\
is-IS/is-IS.hfst.ol it-IT/it-IT.hfst.ol ku-TR/ku-TR.hfst.ol la/la.hfst.ol ln-CD/ln-CD.hfst.ol\
lt-LT/lt-LT.hfst.ol mg-MG/mg-MG.hfst.ol ms-MY/ms-MY.hfst.ol ml-IN/ml-IN.hfst.ol nr-ZA/nr-ZA.hfst.ol\
ne-NP/ne-NP.hfst.ol nl-NL/nl-NL.hfst.ol ns-ZA/ns-ZA.hfst.ol ny-MW/ny-MW.hfst.ol oc-FR/oc-FR.hfst.ol\
or/or.hfst.ol pa/pa.hfst.ol pl-PL/pl-PL.hfst.ol pt-BR/pt-BR.hfst.ol pt-PT/pt-PT.hfst.ol\
ro-RO/ro-RO.hfst.ol ru-RU/ru-RU.hfst.ol rw-RW/rw-RW.hfst.ol se/se.hfst.ol se-FI/se-FI.hfst.ol\
se-NO/se-NO.hfst.ol se-SE/se-SE.hfst.ol sk-SK/sk-SK.hfst.ol sl-SI/sl-SI.hfst.ol sv-SE/sv-SE.hfst.ol\
sw-KE/sw-KE.hfst.ol ta/ta.hfst.ol th-TH/th-TH.hfst.ol tl-PH/tl-PH.hfst.ol tn-ZA/tn-ZA.hfst.ol\
ts-ZA/ts-ZA.hfst.ol uk-UA/uk-UA.hfst.ol uz/uz.hfst.ol vi-VN/vi-VN.hfst.ol \
af-ZA/af-ZA.1errm.hfst.ol am/am.1errm.hfst.ol az/az.1errm.hfst.ol bn-BD/bn-BD.1errm.hfst.ol ca-ES/ca-ES.1errm.hfst.ol csb/csb.1errm.hfst.ol\
cs-CZ/cs-CZ.1errm.hfst.ol de-AT/de-AT.1errm.hfst.ol de-BE/de-BE.1errm.hfst.ol de-CH/de-CH.1errm.hfst.ol de-DE/de-DE.1errm.hfst.ol\
de-DE/de-DE.1errm.hfst.ol de-LU/de-LU.1errm.hfst.ol el-GR/el-GR.1errm.hfst.ol en-GB/en-GB.1errm.hfst.ol en-US/en-US.1errm.hfst.ol\
en-US/en-US.1errm.hfst.ol en-ZA/en-ZA.1errm.hfst.ol eo-l3/eo-l3.1errm.hfst.ol es-ES/es-ES.1errm.hfst.ol es-MX/es-MX.1errm.hfst.ol\
et-EE/et-EE.1errm.hfst.ol fa/fa.1errm.hfst.ol fo-FO/fo-FO.1errm.hfst.ol fr-BE/fr-BE.1errm.hfst.ol fr-CA/fr-CA.1errm.hfst.ol\
fr-CH/fr-CH.1errm.hfst.ol fr-FR/fr-FR.1errm.hfst.ol fr-LU/fr-LU.1errm.hfst.ol fr-MC/fr-MC.1errm.hfst.ol\
fur-IT/fur-IT.1errm.hfst.ol fy-NL/fy-NL.1errm.hfst.ol ga-IE/ga-IE.1errm.hfst.ol gd-GB/gd-GB.1errm.hfst.ol\
gsc-FR/gsc-FR.1errm.hfst.ol gu-IN/gu-IN.1errm.hfst.ol he-IL/he-IL.1errm.hfst.ol hi-IN/hi-IN.1errm.hfst.ol\
hr-HR/hr-HR.1errm.hfst.ol hu-HU/hu-HU.1errm.hfst.ol ia/ia.1errm.hfst.ol id-ID/id-ID.1errm.hfst.ol is/is.1errm.hfst.ol\
is-IS/is-IS.1errm.hfst.ol it-IT/it-IT.1errm.hfst.ol ku-TR/ku-TR.1errm.hfst.ol la/la.1errm.hfst.ol ln-CD/ln-CD.1errm.hfst.ol\
lt-LT/lt-LT.1errm.hfst.ol mg-MG/mg-MG.1errm.hfst.ol ms-MY/ms-MY.1errm.hfst.ol ml-IN/ml-IN.1errm.hfst.ol nr-ZA/nr-ZA.1errm.hfst.ol\
ne-NP/ne-NP.1errm.hfst.ol nl-NL/nl-NL.1errm.hfst.ol ns-ZA/ns-ZA.1errm.hfst.ol ny-MW/ny-MW.1errm.hfst.ol oc-FR/oc-FR.1errm.hfst.ol\
or/or.1errm.hfst.ol pa/pa.1errm.hfst.ol pl-PL/pl-PL.1errm.hfst.ol pt-BR/pt-BR.1errm.hfst.ol pt-PT/pt-PT.1errm.hfst.ol\
ro-RO/ro-RO.1errm.hfst.ol ru-RU/ru-RU.1errm.hfst.ol rw-RW/rw-RW.1errm.hfst.ol se/se.1errm.hfst.ol se-FI/se-FI.1errm.hfst.ol\
se-NO/se-NO.1errm.hfst.ol se-SE/se-SE.1errm.hfst.ol sk-SK/sk-SK.1errm.hfst.ol sl-SI/sl-SI.1errm.hfst.ol sv-SE/sv-SE.1errm.hfst.ol\
sw-KE/sw-KE.1errm.hfst.ol ta/ta.1errm.hfst.ol th-TH/th-TH.1errm.hfst.ol tl-PH/tl-PH.1errm.hfst.ol tn-ZA/tn-ZA.1errm.hfst.ol\
ts-ZA/ts-ZA.1errm.hfst.ol uk-UA/uk-UA.1errm.hfst.ol uz/uz.1errm.hfst.ol vi-VN/vi-VN.1errm.hfst.ol


# couldn't compile
BROKEN_HUNSPELLS=ar.aff da_DK.dic hy_AM.aff mr_IN.dic ve_ZA.dic zu_ZA.dic
# ve_ZA.dic has lots of obviously bogus entries: XML tag parts, TeX data...
# zu_ZA.dic contains aff and ELF(!) data (=potential segfault+rootkit)
# uz_1errm.hfst would require better Unicode support

all: hunspell-dic2lexc hunspell-aff2lexc

hyph-utf8-hfst.tar.gz: $(HYPH_TARGETS)
	-mkdir -p hyph-utf8-hfst
	cp -v $(HYPH_TARGETS) README.hyph hyph-utf8-hfst
	tar zcvf $@ hyph-utf8-hfst
	rm -rf hyph-utf8-hfst

hunspell-hfst.tar.bz: $(HUNSPELL_TARGETS)
	-mkdir -p hunspell-hfst
	cp -v $(HUNSPELL_TARGETS) README.hunspell hunspell-hfst
	tar zcvf $@ hunspell-hfst
	rm -rf hunspell-hfst

texhyphs: $(HYPH_TARGETS)

hunspells: $(HUNSPELL_TARGETS)

# tex pattern translation
%.hfst: %.tex
	python texpat2twolc.py --input=$< --output=$*
	$(HTWOLC) -w -i $*.1.htwolc -o $*.1.hfst -v
	$(HTWOLC) -w -i $*.2.htwolc -o $*.2.hfst -v
	$(HTWOLC) -w -i $*.3.htwolc -o $*.3.hfst -v
	$(HTWOLC) -w -i $*.4.htwolc -o $*.4.hfst -v
	$(HTWOLC) -w -i $*.5.htwolc -o $*.5.hfst -v
	$(HTWOLC) -w -i $*.6.htwolc -o $*.6.hfst -v
	$(HTWOLC) -w -i $*.7.htwolc -o $*.7.hfst -v
	$(HTWOLC) -w -i $*.8.htwolc -o $*.8.hfst -v
	$(HTWOLC) -w -i $*.9.htwolc -o $*.9.hfst -v
	$(HCOMP) $*.1.hfst $*.2.hfst -v |\
		$(HCOMP) $*.3.hfst -v |\
		$(HCOMP) $*.4.hfst -v |\
		$(HCOMP) $*.5.hfst -v |\
		$(HCOMP) $*.6.hfst -v |\
		$(HCOMP) $*.7.hfst -v |\
		$(HCOMP) $*.8.hfst -v |\
		$(HCOMP) $*.9.hfst -v |\
		$(HMIN) -v -o $@

%.hfst.ol: %.hfst
	$(HOPT) < $< > $@

# hunspell compilation

%.hfst: %.aff %.dic
	if grep -m 1 '^AF' $*.aff > /dev/null ; then \
		./hunspell-dic2lexc AF $*.dic $*.dic.lexc $*.dic.symbols ; \
	elif grep -m 1 'FLAG.*UTF-8' $*.aff; then \
		./hunspell-dic2lexc UTF-8 $*.dic $*.dic.lexc $*.dic.symbols ; \
	elif grep -m 1 'FLAG.*long' $*.aff ; then \
		./hunspell-dic2lexc LONG $*.dic $*.dic.lexc $*.dic.symbols ; \
	elif grep -m 1 'FLAG.*num' $*.aff ; then \
		./hunspell-dic2lexc NUM $*.dic $*.dic.lexc $*.dic.symbols ; \
	else \
		./hunspell-dic2lexc ASCII $*.dic $*.dic.lexc $*.dic.symbols ; \
	fi
	./hunspell-aff2lexc $*.aff $*.aff.lexc $*.rules.twolc $*.err.lexc $*.aff.symbols $*.symbols.twolc
	cat $*.dic.symbols $*.aff.symbols > $*.symbols.lexc
	$(HLEXC) -w -v -o $*.lex.hfst $*.symbols.lexc $*.dic.lexc $*.aff.lexc
	$(HLEXC) -w -v -o $*.1errm.hfst $*.err.lexc
	cat $*.symbols.twolc $*.rules.twolc > $*.twolc
	$(HTWOLC) --resolve -w -v -o $*.twol.hfst $*.twolc
	$(HIC) -v -l $*.lex.hfst $*.twol.hfst |\
		$(HMIN) -v -o $*.dbg.hfst
	$(HPROJ) -p lower -v $*.dbg.hfst |\
		$(HMIN) -v -o $@
	$(HOPT) -v -i $@ -o $@.ol
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
