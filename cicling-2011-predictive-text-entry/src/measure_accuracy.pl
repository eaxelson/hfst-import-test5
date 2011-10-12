#! /usr/bin/perl

@correct_suggestions = (0,0,0,0,0,0,0,0,0,0);

$total_words=0;

while (<stdin>)
{
    ++$total_words;
    
    chomp();

    @fields = split(/\t/,$_);
    
    $correct_word = $fields[0];

    @suggestions = @fields[1 .. 10];

    $position=0;
    $found=0;

    foreach (@suggestions)
    {
	if ($_ eq $correct_word)
	{ 
	    $found=1;
	    last;
	}
	$position = $position + 1;
	
    }

    if ($found==1)
    {
	$correct_suggestions[$position] = $correct_suggestions[$position] + 1;
    }
}

print "$ARGV[0]\t";
foreach (@correct_suggestions)
{
    printf("& %.4f\\\%",100*$_/$total_words);
    print " ";
}
print "\\\\\n"
