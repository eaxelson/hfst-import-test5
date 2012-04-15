BEGIN {FS="\t"; clocks = 0; utimes=0; stimes=0; results = 0}
NF > 5 {clocks += $2; utimes += $3; stimes += $4; results += 1;}
END {printf("%.2f\n", utimes / results);}

