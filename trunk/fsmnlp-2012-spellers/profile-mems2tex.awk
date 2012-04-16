BEGIN {FS="\t"; rss_max = 0;  results = 0}
NF > 5 {rss_max += $5; utimes += $3; stimes += $4; results += 1;}
END {
    printf("%.1f kiB\n", (rss_max / 1024) / results);
}

