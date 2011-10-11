#! /usr/bin/perl

while (<>)
{
    chomp();

    if (/^#/)
    { next; }

    @fields = split(/ /,$_);
    
    $multiplicity = $fields[0];

    foreach $i (1 .. $multiplicity)
    {
	print "$fields[1]";

	foreach $j (2 .. $#fields)
	{ print " $fields[$j]"; } 
	print "\n";
    }
}
