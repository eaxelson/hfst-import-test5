BEGIN {a=a; b=b;}
NR = 5 {$5=a*$5+b;}
NR = 2 {$2=a*$2+b;}
{print}
