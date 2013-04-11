#!/bin/bash

rm langs_sizeXfst.all
XFST=/home/drobac/xfst/bin/xfst

# myv sma sme smj
for i in kpv myv sma sme smj; do
    echo "Testing" $i "..."
    
    # print language code
    echo "$i" >> langs_sizeXfst.all
    
    P=$i/src/morphology
    XfstFile=$P/$i.lexc.xfst
    
    # print original size
    printf "load $XfstFile \n\
        size > size.txt \n\
        sigma > sigma_orig.txt \n\
        quit \n" > tmp-flags-script
        $XFST < tmp-flags-script
    rm tmp-flags-script
    
    cat size.txt >> langs_sizeXfst.all
    
    originalSizeKb=$(cat size.txt | sed -n 's/\(.*\) Kb\..*/\1/p')
    originalSizeMb=$(cat size.txt | sed -n 's/\(.*\) Mb\..*/\1/p')

#    if [ -z "$originalSizeKb" ]; then
#        originalSize=$(echo "scale=2; $originalSizeMb * 1024" | bc)
#        echo "--- originalSizeKb is empty --- $originalSize"
#    else
#        originalSize=$originalSizeKb
#    fi

    
	# get flag names
    #cat sigma_orig.txt | sed -n 's/.*feature '\''\(.*\)'\'' with .*/\1/p' | sort | uniq | sort > flags.txt
    sh findOneFlag.sh $i
    
    # for each flag load orig tr and eliminate the flag. Print new size
    while read line           
    do  
    	#escape [ ]
       flag=$(echo $line | sed 's/\[/%[/g' | sed 's/\]/%]/g' )
       echo "FLAG IS : $flag"
        printf "load $XfstFile \n\
            eliminate flag $flag \n\
            size > tmp_size.txt \n\
            quit \n" > tmp-flags-script
            $XFST < tmp-flags-script
            
        
        woFlagSizeKb=$(cat tmp_size.txt | sed -n 's/\(.*\) Kb\..*/\1/p')
        woFlagSizeMb=$(cat tmp_size.txt | sed -n 's/\(.*\) Mb\..*/\1/p')
        

#        if [ -z "$woFlagSizeKb" ]; then
#            woFlagSize=$(echo "scale=2; $woFlagSizeMb * 1024" | bc)
#            echo "--- woFlagSizeKb is empty --- $woFlagSize"
#        else
#            woFlagSize=$woFlagSizeKb
#        fi
    
        echo "woflagsize $woFlagSize -"
    	percent=$(echo "scale=2; $woFlagSizeMb * 100 / $originalSizeMb" | bc)
    
   		echo "Wo FLAG: $line, tr size is: $woFlagSizeMb Mb., $percent% size of the original tr" >> langs_sizeXfst.all
   		
   		# prints full size wo flag
   		# cat tmp_size.txt >> langs_sizeXfst.all
        rm tmp-flags-script
        rm tmp_size.txt
       
    done < flags.txt      
    rm *.txt
    
    echo "--" >> langs_sizeXfst.all
done

echo " ----- RESULT: ------ "
cat langs_sizeXfst.all

    
    