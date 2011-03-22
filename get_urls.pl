#!/usr/bin/perl

while (<STDIN>) {
	$line = $_;
	chop($line);
	if ($line =~ m/<a\s*.*?\s*href\s*=\s*['"](.*?)['"].*>/i) {
		$url = $1;
		print ("$url\n");
	}
}
