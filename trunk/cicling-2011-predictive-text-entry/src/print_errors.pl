#! /usr/bin/perl

$total = 0;
$correct = 0;

while (<>)
{
    chomp();

    if ($_ eq "")
    { next; }

    @fields = split('\t');
    
    @suggestions = @fields[1 .. 10];

    foreach (@suggestions)
    {
	if ($_ eq $correct_word)
	{ 
	    $found=1;
	    last;
	}
    }

    if ($found == 0)
    { print "$_\n"; }
}

