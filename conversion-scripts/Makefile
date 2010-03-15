
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
