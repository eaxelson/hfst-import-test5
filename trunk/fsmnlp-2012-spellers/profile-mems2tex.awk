BEGIN {FS="\t"; rss_max = 0;  results = 0}
NF > 5 {rss_max += $5; utimes += $3; stimes += $4; results += 1;}
END {
    printf("%.1f\n", rss_max / results);
    if (NR > 10) {
        print("DANGER TERROR HORROR !!!!!!");
    }
}

