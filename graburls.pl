#!/usr/bin/perl
#use strict;
use Getopt::Std;
use LWP::Simple;
use HTML::Parser;

sub catch_zap {
#        print "Caught a ctrl-c will quit after next fetching.\n";
#	$done = 1;
}
$SIG{INT} = \&catch_zap;

sub validateUrl {
  my($strUrl) = shift;
  return
  $strUrl =~ m!(http:|https:|ftp:)//([A-z\d]+)\:([A-z\d]+)\@([A-z\d\-\.]+\.)+[A-z]!i ||
  $strUrl =~ m!^(http:|https:|ftp:)//([A-z\d\-\.]+\.)+[A-z]!i ||
  $strUrl=~ m!^(http:|https:|ftp:)//(\d){1,3}\.(\d){1,3}\.(\d){1,3}\.(\d){1,3}!i ? 1 : 0;
}

#
# Grab all links from local or remote html file
# perl html munging
#
# option -a (/ -r) grabs only absolute (/ relative) urls

# get options and argument
#
my %opts;
getopts('ard:', \%opts);
my $arg = shift;
die "Usage: $0 [-d depth] [-a | -r] filename [| URL]\n"
	if (not defined $arg or $opts{a} && $opts{r}); # allow either -a or -r

# get the page either from file or url
#
my $page;
if ($arg =~ m!^http://!) {
	$page = get($arg)
		or die "Couldn't get $arg: $!\n";
} 
else {
	open FH, "<", $arg
		or die "Couldn't open $arg: $!\n";
	$page = do { local $/; <FH> };
	close FH;
}

# set the parser and parse
#
my $parser = HTML::Parser->new( api_version => 3, start_h => [\&start,"tagname, attr"],);
my @links;
sub start {
	my ($tag, $attr) = @_;
	if ($tag =~ /^a$/ and defined $attr->{href}) {
			return
				if ($attr->{href} =~ m!^http://! and $opts{r}); # exclude absolute url when -r
			return
				if ($attr->{href} !~ m!http://! and $opts{a});	# exclude relative url when -a
			push @links, $attr->{href};
	}
	if ($tag =~ /^embed$/ and defined $attr->{src}) {
			return
				if ($attr->{src} =~ m!^http://! and $opts{r}); # exclude absolute url when -r
			return
				if ($attr->{src} !~ m!http://! and $opts{a});	# exclude relative url when -a
			push @links, $attr->{src};
	}
}
$parser->parse($page);
$parser->eof;	

#print "Option -d was set to:".$opts{d}."\n";
$depth = $opts{d};

open (OUT,">>urls.txt");

# output
#
#$dbh = Mysql->connect($host,$user,$passwd,$dbname);
#map {$sth = $dbh->query("insert into crawlurl values ('','$_','$depth','2001-01-01 00:00:00','','','')")} @links;
foreach (@links) {
	$mylink = $_;
	$ret = validateUrl($mylink);
	if ($ret) {
		if ($mylink =~ m/#$/i) {
			chop($mylink);
		}
		if ($mylink =~ m/\/$/i) {
			chop($mylink);
		}
		if ($mylink =~ m/^(.*)\#(.*)$/i) {
			$line = $1;
			$mylink = $line;
		}
		if ($mylink =~ m/\.swf$/i) {
			open (SWF,">>swf.txt");
			print SWF ("$mylink\n");
			close SWF;
	#		print OUT "$mylink\n";
	#	} elsif ($mylink =~ m/download/i) {
	#		$sth = $dbh->query("insert into applog values (now(),'skipped $mylink')");
		} elsif ($mylink =~ m/^javascript/i) {
		} elsif ($mylink =~ m/download\.jsp/i) {
		} elsif (!($mylink =~ m/^http/i)) {
		} elsif ($mylink =~ m/download\.php/i) {
		} elsif ($mylink =~ m/\.collegeclown\.com/i) {
		} elsif ($mylink =~ m/\.zip$/i) {
		} elsif ($mylink =~ m/\.wmv$/i) {
		} elsif ($mylink =~ m/\.mov$/i) {
		} elsif ($mylink =~ m/\.pdf$/i) {
		} elsif ($mylink =~ m/\.dmg$/i) {
		} elsif ($mylink =~ m/\.mp3$/i) {
		} elsif ($mylink =~ m/\.mp4$/i) {
		} elsif ($mylink =~ m/\.flv$/i) {
		} elsif ($mylink =~ m/\.rar$/i) {
		} elsif ($mylink =~ m/\.000$/i) {
		} elsif ($mylink =~ m/\.xml$/i) {
		} elsif ($mylink =~ m/\.svg$/i) {
		} elsif ($mylink =~ m/\.wav$/i) {
		} elsif ($mylink =~ m/\.iso$/i) {
		} elsif ($mylink =~ m/\.exe$/i) {
		} elsif ($mylink =~ m/\.dir$/i) {
		} elsif ($mylink =~ m/\.dcr$/i) {
		} elsif ($mylink =~ m/\.jpg$/i) {
		} elsif ($mylink =~ m/\.gif$/i) {
		} elsif ($mylink =~ m/\.png$/i) {
		} elsif ($mylink =~ m/\.avi$/i) {
		} elsif ($mylink =~ m/^mailto/i) {
		} elsif ($mylink =~ m/\/1.htm$/i) {
		} else {
			print OUT "$mylink|$depth\n";
		}
	}
}
close OUT;

