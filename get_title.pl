#!/usr/bin/perl

while (<STDIN>) {
	$line = $_;
	chop($line);
	if ($line =~ m/<title>(.*)<\/title>/i) {
		$title = $1;
		print ("$title\n");
	}
}
