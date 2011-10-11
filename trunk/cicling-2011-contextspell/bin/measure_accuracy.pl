#! /usr/bin/perl

$total = 0;
$correct = 0;

while (<>)
{
    ++$total;

    chomp($_);

    @fields = split(/\t/,$_);

    if ($#fields != 2)
    {
	print "Syntax error on line $total";
	exit 1;
    }

    if ($fields[1] eq $fields[2])
    { ++$correct; }
}

print "Totally $total entries. Of these $correct were correct.\n";

$accuracy = $correct * 1.0 / $total;

print "Accuracy: $accuracy\n"
