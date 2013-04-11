#!/bin/bash

rm flagFiles

for i in est fao fin fkv izh kca kpv liv mdf mhr mrj myv ndl nio olo sjd sje sma sme smj smn tlh tuv udm vep vro yrk zul; do

    echo $i;
    cd $i/src/morphology
    
    
    FILE=tmp_flags_list.txt
    # old_allLangs/langs/sme/src/morphology
	hfst-summarize -v $i.lexc.hfst | grep -ow "\@[^_]*\@" | sed 's/\, /\n/g' | sort | uniq > $FILE
         
         
         
         
         
    if [[ -s $FILE ]] ; then
		echo $i >> ../../../flagFiles
		echo "-" >> ../../../flagFiles
	    cat $FILE | sed -e 's/^@[^.]*\.//' | sed -e 's/\.[^@]*@//' | sed -e 's/\@//' | sort | uniq >> ../../../flagFiles
		echo "----" >> ../../../flagFiles

 		# load stack <  kpv.lexc.hfst
		#  eliminate flag @U.CONJ-NX.SG@
		# ...
		 
	fi ;
       
    rm $FILE 
       
         
         # save to file, if it is empty there are no flags, ( delete that file)
         # if there are flags, remove one by one and check size
         
#an exit status of 1 indicates that the FILE is empty or the file does not exist.....
       
         
         
    cd ../../../
done

cat flagFiles

