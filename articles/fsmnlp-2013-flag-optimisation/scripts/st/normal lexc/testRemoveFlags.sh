#!/bin/bash


# ipk has flag: @D.[IT]V.ON@ how to eliminate it?


# rgrep -w "\@[PURDCN].*\..*\@" .

# no flags: bxr iku ces cor eng eus kal nob non nno nso rum rus som tgl 
# don't work: bul zul epo fao hin(commented out)
# no src: kin tir tuk

rm st_sizeXfst.all
XFST=/home/drobac/xfst/bin/xfst



#amh ara ciw deu grn ipk kmr
for i in  deu grn kmr; do
    echo "Testing" $i "..."
    
    # print language code
    echo "$i" >> st_sizeXfst.all
    
    # print original size
    printf "load $i/bin/$i.save \n\
        size > size.txt \n\
        sigma > sigma_orig.txt \n\
        quit \n" > tmp-flags-script
        $XFST < tmp-flags-script
    rm tmp-flags-script
    
    cat size.txt >> st_sizeXfst.all
    
    originalSize=$(cat size.txt | sed -n 's/\(.*\) Kb\..*/\1/p')

	# get flag names
    cat sigma_orig.txt | sed -n 's/.*feature '\''\(.*\)'\'' with .*/\1/p' | uniq > flags.txt
    
    
    # for each flag load orig tr and eliminate the flag. Print new size
    while read line           
    do  
    	#escape [ ]
       flag=$(echo $line | sed 's/\[/%[/g' | sed 's/\]/%]/g' )
       echo "FLAG IS : $flag"
        printf "load $i/bin/$i.save \n\
            eliminate flag $flag \n\
            size > tmp_size.txt \n\
            quit \n" > tmp-flags-script
            $XFST < tmp-flags-script
            
            
        woFlagSize=$(cat tmp_size.txt | sed -n 's/\(.*\) Kb\..*/\1/p')
    	percent=$(echo "scale=2; $woFlagSize * 100 / $originalSize" | bc)
    
   		echo "Wo FLAG: $line, tr size is: $woFlagSize Kb., $percent% size of the original tr" >> st_sizeXfst.all
   		
   		# prints full size wo flag
   		# cat tmp_size.txt >> st_sizeXfst.all
        rm tmp-flags-script
        rm tmp_size.txt
       
    done < flags.txt      
    rm *.txt
    
    echo "--" >> st_sizeXfst.all
done

echo " ----- RESULT: ------ "
cat st_sizeXfst.all

    
    