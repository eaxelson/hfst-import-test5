#! /usr/bin/perl

$total = 0;
$correct = 0;

while (<>)
{
    chomp();

    if ($_ eq "")
    { next; }

    @fields = split('\t');

    $correct_word = $fields[0];

    @suggestions = @fields[1 .. 10];

    $found = 0;

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

