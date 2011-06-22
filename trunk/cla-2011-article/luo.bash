#!/bin/bash

# Tarkoituksena luoda L_tot = L - (Univ - (L_asp U L_omorfi))
# kaikilla L:n painoyhdistelmillä
#
# L_asp U L_omorfi on aspektileksikon ja omorfileksikon unioni eli disjunktio
# L = W(k, (L_asp U Univ)) o W(1-k, (L_omorfi U Univ))
# jossa k on skaalauskerroin,
# W skaalattu yhdistelmä leksikon ja universaalitransduktorin unionista
# L on siis kompositio kahdesta automaatista, jotka muuttavat merkkijonot
# itseikseen, eli käytännössä kahden automaatin leikkaus. 

# luodaan universaalikieli Univ = universaalikieli.txt
cat universaali.txt | hfst-regexp2fst -v -o universaalikieli.hfst

# luodaan aspektileksikko L_asp = asplex-min.hfst
# asplex.txt:ssä sana per rivi, tyhjät rivit poistettu
sort asplex.txt | uniq -c > asplex_freq.txt
python flipfreq.py "asplex_freq.txt" "asplex_flipped.txt"
hfst-strings2fst -v -i asplex_flipped.txt -o asplex.hfst -j
hfst-minimize -i asplex.hfst -o asplex-min.hfst

# omorfileksikko jo olemassa, L_omor = omorfi-weighted-min.hfst

# luodaan disjunktio eli unioni aspektileksikosta ja omorfileksikosta
hfst-disjunct -1 asplex-min.hfst -2 omorfi-weighted-min.hfst -o aspUomo.hfst
hfst-minimize -i aspUomo.hfst -o aspUomo-min.hfst

# luodaan välivaihetransduktori uni-auo-min.hfst, joka kuvaa Univ - (L_asp U L_omorfi)

hfst-subtract -1 universaalikieli.hfst -2 aspUomo-min.hfst -o uni-auo.hfst
hfst-minimize -i uni-auo.hfst -o uni-auo-min.hfst

# luodaan aspektileksikon ja universaalikielen unioni

hfst-disjunct -1 asplex-min.hfst -2 universaalikieli.hfst -o aspUuni.hfst
hfst-minimize -i aspUuni.hfst -o aspUuni-min.hfst

# luodaan omorfileksikon ja universaalikielen unioni

hfst-disjunct -1 omorfi-weighted-min.hfst -2 universaalikieli.hfst -o omoUuni.hfst
hfst-minimize -i omoUuni.hfst -o omoUuni-min.hfst

# skaalataan edelliset, eli luodaan W = (k, L_asp U Univ) ja W = (1-k, L_omorfi U Univ)

# - luodaan kaikki painoversiot aspUunista
hfst-scale-weights 0.0 aspUuni-min.hfst asp00.hfst
hfst-scale-weights 0.1 aspUuni-min.hfst asp01.hfst
hfst-scale-weights 0.2 aspUuni-min.hfst asp02.hfst
hfst-scale-weights 0.3 aspUuni-min.hfst asp03.hfst
hfst-scale-weights 0.4 aspUuni-min.hfst asp04.hfst
hfst-scale-weights 0.5 aspUuni-min.hfst asp05.hfst
hfst-scale-weights 0.6 aspUuni-min.hfst asp06.hfst
hfst-scale-weights 0.7 aspUuni-min.hfst asp07.hfst
hfst-scale-weights 0.8 aspUuni-min.hfst asp08.hfst
hfst-scale-weights 0.9 aspUuni-min.hfst asp09.hfst
hfst-scale-weights 1.0 aspUuni-min.hfst asp10.hfst

# - luodaan kaikki painoversiot omoUunista
hfst-scale-weights 0.0 omoUuni-min.hfst omo00.hfst
hfst-scale-weights 0.1 omoUuni-min.hfst omo01.hfst
hfst-scale-weights 0.2 omoUuni-min.hfst omo02.hfst
hfst-scale-weights 0.3 omoUuni-min.hfst omo03.hfst
hfst-scale-weights 0.4 omoUuni-min.hfst omo04.hfst
hfst-scale-weights 0.5 omoUuni-min.hfst omo05.hfst
hfst-scale-weights 0.6 omoUuni-min.hfst omo06.hfst
hfst-scale-weights 0.7 omoUuni-min.hfst omo07.hfst
hfst-scale-weights 0.8 omoUuni-min.hfst omo08.hfst
hfst-scale-weights 0.9 omoUuni-min.hfst omo09.hfst
hfst-scale-weights 1.0 omoUuni-min.hfst omo10.hfst

# L = W o W -kompositioiden luonti kaikille painoversioille

hfst-compose -1 asp00-min.hfst -2 omo10-min.hfst -o L_a00o10.hfst -F
hfst-minimize -i L_a00o10.hfst -o L_a00o10-min.hfst
hfst-compose -1 asp01-min.hfst -2 omo09-min.hfst -o L_a01o09.hfst -F
hfst-minimize -i L_a01o09.hfst -o L_a01o09-min.hfst
hfst-compose -1 asp02-min.hfst -2 omo08-min.hfst -o L_a02o08.hfst -F
hfst-minimize -i L_a02o08.hfst -o L_a02o08-min.hfst
hfst-compose -1 asp03-min.hfst -2 omo07-min.hfst -o L_a03o07.hfst -F
hfst-minimize -i L_a03o07.hfst -o L_a03o07-min.hfst
hfst-compose -1 asp04-min.hfst -2 omo06-min.hfst -o L_a04o06.hfst -F
hfst-minimize -i L_a04o06.hfst -o L_a04o06-min.hfst
hfst-compose -1 asp05-min.hfst -2 omo05-min.hfst -o L_a05o05.hfst -F
hfst-minimize -i L_a05o05.hfst -o L_a05o05-min.hfst
hfst-compose -1 asp06-min.hfst -2 omo04-min.hfst -o L_a06o04.hfst -F
hfst-minimize -i L_a06o04.hfst -o L_a06o04-min.hfst
hfst-compose -1 asp07-min.hfst -2 omo03-min.hfst -o L_a07o03.hfst -F
hfst-minimize -i L_a07o03.hfst -o L_a07o03-min.hfst
hfst-compose -1 asp08-min.hfst -2 omo02-min.hfst -o L_a08o02.hfst -F
hfst-minimize -i L_a08o02.hfst -o L_a08o02-min.hfst
hfst-compose -1 asp09-min.hfst -2 omo01-min.hfst -o L_a09o01.hfst -F
hfst-minimize -i L_a09o01.hfst -o L_a09o01-min.hfst
hfst-compose -1 asp10-min.hfst -2 omo00-min.hfst -o L_a10o00.hfst -F
hfst-minimize -i L_a10o00.hfst -o L_a10o00-min.hfst

# L_tot = L - Univ (L_asp U L_omorfi) -versioiden luonti

hfst-subtract -1 L_a00o10-min.hfst -2 uni-auo-min.hfst -o final_a00o10.hfst -F
hfst-subtract -1 L_a01o09-min.hfst -2 uni-auo-min.hfst -o final_a01o09.hfst -F
hfst-subtract -1 L_a02o08-min.hfst -2 uni-auo-min.hfst -o final_a02o08.hfst -F
hfst-subtract -1 L_a03o07-min.hfst -2 uni-auo-min.hfst -o final_a03o07.hfst -F
hfst-subtract -1 L_a04o06-min.hfst -2 uni-auo-min.hfst -o final_a04o06.hfst -F
hfst-subtract -1 L_a05o05-min.hfst -2 uni-auo-min.hfst -o final_a05o05.hfst -F
hfst-subtract -1 L_a06o04-min.hfst -2 uni-auo-min.hfst -o final_a06o04.hfst -F
hfst-subtract -1 L_a07o03-min.hfst -2 uni-auo-min.hfst -o final_a07o03.hfst -F
hfst-subtract -1 L_a08o02-min.hfst -2 uni-auo-min.hfst -o final_a08o02.hfst -F
hfst-subtract -1 L_a09o01-min.hfst -2 uni-auo-min.hfst -o final_a09o01.hfst -F 
hfst-subtract -1 L_a10o00-min.hfst -2 uni-auo-min.hfst -o final_a10o00.hfst -F
hfst-minimize -i final_a00o10.hfst -o final_a00o10-min.hfst
hfst-minimize -i final_a01o09.hfst -o final_a01o09-min.hfst
hfst-minimize -i final_a02o08.hfst -o final_a02o08-min.hfst
hfst-minimize -i final_a03o07.hfst -o final_a03o07-min.hfst
hfst-minimize -i final_a04o06.hfst -o final_a04o06-min.hfst
hfst-minimize -i final_a05o05.hfst -o final_a05o05-min.hfst
hfst-minimize -i final_a06o04.hfst -o final_a06o04-min.hfst
hfst-minimize -i final_a07o03.hfst -o final_a07o03-min.hfst
hfst-minimize -i final_a08o02.hfst -o final_a08o02-min.hfst
hfst-minimize -i final_a09o01.hfst -o final_a09o01-min.hfst
hfst-minimize -i final_a10o00.hfst -o final_a10o00-min.hfst






