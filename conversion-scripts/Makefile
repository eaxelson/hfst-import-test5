
all: hunspell-dic2lexc hunspell-aff2lexc+twolc

# tex pattern translation
%.hfst: %.tex
	python texpat2twolc.py --input=$< --output=$*
	hfst-twolc -i $*.1.htwolc -o $*.1.hfst -v
	hfst-twolc -i $*.2.htwolc -o $*.2.hfst -v
	hfst-twolc -i $*.3.htwolc -o $*.3.hfst -v
	hfst-twolc -i $*.4.htwolc -o $*.4.hfst -v
	hfst-twolc -i $*.5.htwolc -o $*.5.hfst -v
	hfst-twolc -i $*.6.htwolc -o $*.6.hfst -v
	hfst-compose $*.1.hfst $*.2.hfst -v |\
		hfst-compose $*.3.hfst -v |\
		hfst-compose $*.4.hfst -v |\
		hfst-compose $*.5.hfst -v |\
		hfst-compose $*.6.hfst -v |\
		hfst-minimize -v -o $@

# hunspell tools

hunspell-dic2lexc: hunspell-dic.tab.o hunspell-dic.o

hunspell-dic.o: hunspell-dic.cc hunspell-dic.tab.hh

hunspell-dic.cc: hunspell-dic.ll
	flex -o $@ $<

hunspell-dic.tab.hh: hunspell-dic.yy
	bison -d $<
