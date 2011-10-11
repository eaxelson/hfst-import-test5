#! /usr/bin/perl

@correct_suggestions = (0,0,0,0,0,0,0,0,0,0);

$total_words=0;

while (<>)
{
    ++$total_words;
    
    chomp();

    @fields = split(/\t/,$_);
    
    $correct_word = $fields[0];

    @suggestions = @fields[1,10];

    $position=0;
    $found=0;

    foreach (@suggestions)
    {
	if ($_ eq $correct_word)
	{ 
	    $found=1;
	    last;
	}
	++$position;
    }

    if ($found==1)
    {
	++$correct_suggestions[$position];
    }
}

foreach (@correct_suggestions)
{
    print $_/$total_words;
    print " ";
}
print "\n"
