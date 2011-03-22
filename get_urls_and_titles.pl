#!/usr/bin/perl

while (<STDIN>) {
	$line = $_;
	chop($line);
	if ($line =~ m/<a\s*.*?\s*href\s*=\s*['"](.*?)['"].*>(.*)<\/a>/i) {
		$url = $1;
		$title = $2;
		print ("$url|$title\n");
	}
}
