#!/usr/bin/perl
open(TESTS,"geniustests.txt") || die "can't open the geniustests.txt file";

$errors = 0;
$tests = 0;
$options = "";

$i = 0;

while(<TESTS>) {
	if(/^OPTIONS[ 	]+(.*)$/) {
		$options = $1;
	} elsif(/^([^	]+)	+([^	]+)$/) {
		$tests++;
		$command = $1;
		$shd=$2;
	} elsif(/^([^	]+)$/) {
		$tests++;
		$command = $1;
		$shd="";
	} else {
		next;
	}

	print "$1\n";
	#something weird happens and the following modifies $1 and $2
	#as well, I guess those can only be used from the last regexp
	$command =~ s/'/'\\''/g;
	open(GENIUS,"echo '$command' | ./genius $options |") ||
		die "can't open pipe!";

	if($rep=<GENIUS>) {
		chomp $shd;
		chomp $rep;

		print " (should be)=$shd\n";
		print " (reported)=$rep\n";
		if($rep ne $shd) {
			print "ERROR!\n";
			$errors++;
		}
	} else {
		chomp $shd;
		print " (should be)=$shd\n";
		print " (reported)=\n";
		if($shd ne "") {
			print "ERROR! NO OUTPUT\n";
			$errors++;
		}
	}
	print "\n";
	close(GENIUS);
	system("mv gmon.out gmon${i}.out") if(-e "gmon.out");
	$i++;
}

print "tests: $tests, errors: $errors\n"
