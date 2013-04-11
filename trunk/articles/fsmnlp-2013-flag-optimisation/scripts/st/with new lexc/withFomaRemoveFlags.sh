#!/bin/bash

rm st_sizeFoma.all
#grn kmr 
for i in deu grn kmr; do
    echo "Testing" $i "..."
    
    # print language code
    echo "$i" >> st_sizeFoma.all
    
    # print original size
    printf "load $i/bin/$i.foma \n\
        sigma \n\
        quit \n" > tmp-flags-script
        foma -f tmp-flags-script > tmpResult
    rm tmp-flags-script
    
    head -n 1 tmpResult > size.txt
    tail -n +2 tmpResult > sigma_orig.txt
    rm tmpResult
    
    cat size.txt >> st_sizeFoma.all
    
    originalSize=$(cat size.txt | sed -n 's/\(.*\) kB\..*/\1/p')
    originalSizeDisk=$(stat -c%s $i/bin/$i.foma)
    
    echo "Orig Disk: $originalSizeDisk" >> st_sizeFoma.all
    # get flag names
    cat sigma_orig.txt | grep -ow "\@[^_@]*\@" | sed -e 's/^@[^.]*\.//' | sed -e 's/\.[^@]*@//' | sed -e 's/\@//' | sort | uniq > flags.txt

 echo "--First part done --"

# for each flag load orig tr and eliminate the flag. Print new size
    while read line           
    do  
        #escape [ ]
       flag=$(echo $line | sed 's/\[/%[/g' | sed 's/\]/%]/g' )
       echo "FLAG IS : $flag"
        printf "load $i/bin/$i.foma \n\
            eliminate flag $flag\n\
            size \n\
            save stack tmp.foma.gz \n\
            quit \n" > tmp-flags-script
            foma -f tmp-flags-script > tmpResult
            
        echo "tmp result:::"
        cat     tmpResult
            
        gunzip tmp.foma.gz
        newSizeDisk=$(stat -c%s tmp.foma)
       
        percent=$(echo "scale=2; $newSizeDisk * 100 / $originalSizeDisk" | bc)
        echo "Wo FLAG: $line, tr size is: $newSizeDisk, $percent% size of the original tr" >> st_sizeFoma.all
        
        rm  tmp.foma
        rm tmp-flags-script

       
    done < flags.txt      
    rm *.txt
    

    
    echo "--" >> st_sizeFoma.all
    
       
done

echo " ----- RESULT: ------ "
cat st_sizeFoma.all