BEGIN {firsts = 0; rows = 0; seconds = 0; thirds = 0; fourths = 0; laters = 0; misses = 0;}
NF < 4 {misses++; rows++;}
NF > 3 {thismiss = 1;
    rows++;
    if ($2 == $3) {firsts++; thismiss = 0;}
    else if ($2 == $4) {seconds++; thismiss = 0;}
    else if ($2 == $5) {thirds++; thismiss = 0;}
    else if ($2 == $6) {fourths++; thismiss = 0;}
    else {
        for (i = 7; i < (NF - 1); i++) {
            if ($2 = $i) {thismiss = 0;}
        }
    }
    misses = misses + thismiss;
}
END { printf("1st   & 2nd   & 3rd   & 4th   & miss  & all \\\\\n");
      printf("%d    & %d    & %d    & %d    & %d    & %d \\\\\n", firsts, seconds, thirds, fourths, misses, rows);
      printf("%f %% & %f %% & %f %% & %f %% & %f %% & %f %% \\\\\n", firsts/rows, seconds/rows, thirds/rows, fourths/rows, misses/rows, rows/rows);
      
  }
