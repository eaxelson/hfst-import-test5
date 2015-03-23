#! /bin/bash

for i in 3 2 1; do                                                                                 \
 for l in fin sme kal; do                                                                          \
  (time "$l"/compile_"$l" bin/hfst-lexc"$i" "$i") &> compile_"$l"_"$i";                            \
  echo "LOAD TIME:" > lookup_"$l"_"$i";                                                            \
  echo ""                                                                                         |\
  (time bin/hfst-optimized-lookup "$l"/hfst_bin/"$l".hfst."$i" > /dev/null) 2>> lookup_"$l"_"$i";  \
  echo "LOOKUP TIME:" >> lookup_"$l"_"$i";                                                         \
  cat data/"$l"_data.txt                                                                          |\
  (time bin/hfst-optimized-lookup "$l"/hfst_bin/"$l".hfst."$i" > /dev/null) 2>> lookup_"$l"_"$i";  \
 done;                                                                                             \
done