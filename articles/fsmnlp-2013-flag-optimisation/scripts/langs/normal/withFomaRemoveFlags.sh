#!/bin/bash

rm langs_sizeFoma.all

#kpv myv sma sme smj
for i in kpv myv sma sme smj ; do
    echo "Testing" $i "..."
    
    P=$i/src/morphology
    FomaFile=$P/$i.lexc.foma
    
    # create foma type tr from hfst
    hfst-fst2fst -f foma -b $P/$i.lexc.hfst -o $FomaFile
    
    # print language code
    echo "$i" >> langs_sizeFoma.all
    
    # print original size
    printf "load $FomaFile \n\
        quit \n" > tmp-flags-script
        foma -f tmp-flags-script > tmpResult
    rm tmp-flags-script
    
    head -n 1 tmpResult > size.txt
    rm tmpResult
    
    cat size.txt >> langs_sizeFoma.all
    
    originalSize=$(cat size.txt | sed -n 's/\(.*\) kB\..*/\1/p')
    originalSizeDisk=$(stat -c%s $FomaFile)
    
    printFirstsize=$(echo $originalSizeDisk | sed -r ':L;s=\b([0-9]+)([0-9]{3})\b=\1,\2=g;t L')
    echo "Orig Disk: $printFirstsize" >> langs_sizeFoma.all
    
    
    # get flag names
    sh findOneFlag.sh $i # saves all flag names to flags.txt
        
 echo "--First part done --"

# for each flag load orig tr and eliminate the flag. Print new size
    while read line           
    do  
        #escape [ ]
        flag=$(echo $line | sed 's/\[/%[/g' | sed 's/\]/%]/g' )
        echo "FLAG IS : $flag"
        printf "load $FomaFile \n\
            eliminate flag $flag\n\
            size \n\
            save stack tmp.foma.gz \n\
            quit \n" > tmp-flags-script
            foma -f tmp-flags-script > tmpResult
            
        # get new size
        gunzip tmp.foma.gz
        newSizeDisk=$(stat -c%s tmp.foma)
            
        # calculate percentage, print    
        percent=$(echo "scale=2; $newSizeDisk * 100 / $originalSizeDisk" | bc)
        printsize=$(echo $newSizeDisk | sed -r ':L;s=\b([0-9]+)([0-9]{3})\b=\1,\2=g;t L')
        echo "Wo FLAG: $line, tr size is: $printsize bytes, $percent% size of the original tr" >> langs_sizeFoma.all
        
        rm tmp-flags-script
        rm  tmp.foma
       
    done < flags.txt      
    rm *.txt
    
    echo "--" >> langs_sizeFoma.all
    
       
done

echo " ----- RESULT: ------ "
cat langs_sizeFoma.all