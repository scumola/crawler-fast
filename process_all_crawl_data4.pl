#!/usr/bin/perl
#use Data::Dumper;

$numArgs = $#ARGV + 1;
if ($numArgs != 1) {
	die "Usage: progname <unixtime>\n";
}
$crawl_dir = $ARGV[0];

%img = (
	gif,1,
	jpg,1,
	png,1,
	pnm,1,
	tif,1,
	bmp,1,
	svg,1
	);
%doc = (
	doc,1,
	pdf,1,
	xls,1,
	ppt,1
	);
%smap = (
	xml,1
	);
%crap = (
	'000',1,
	avi,1,
	wma,1,
	dcr,1,
	dir,1,
	dmg,1,
	exe,1,
	flv,1,
	iso,1,
	mov,1,
	mp3,1,
	mp4,1,
	mid,1,
	rar,1,
	ram,1,
	rm,1,
	wav,1,
	wmv,1,
	zip,1
	);

#print Dumper(%crap);
#if (exists($img{'gif'})) {
#	print ("Yea!  GIF exists!\n");
#}

open (GOOD, ">> output/$crawl_dir/_good.txt");
open (BAD,  ">> output/$crawl_dir/_bad.txt");
open (IMG,  ">> output/$crawl_dir/_images.txt");
open (DOC,  ">> output/$crawl_dir/_docs.txt");

open (OUT,  ">> output/$crawl_dir/_urls.txt");
open (SMAP, ">> output/$crawl_dir/_sitemap.txt");
open (OTHER, ">> output/$crawl_dir/_other.txt");
open (CRAP, ">> output/$crawl_dir/_crap.txt");

open (IN, "<output/$crawl_dir/crawl.txt");
while (<IN>) {
	chop();
	$line = $_;
	($exitcode,$id,$url) = split(/\|/,$line);
	if (($exitcode > 0) and ($exitcode < 400)) {
#		print GOOD ("$id|$url\n");
		print GOOD ("$url\n");
#		print ("ripping: output/$crawl_dir/$id.txt  --  $url\n");
		#system ("./list_urls.sed < output/$id.txt | ./rel2abs -b '$url' | sed 's/\\/\\//\\//g' | grep -E '^http' | sort -u > output/abs_urls_$id.txt");
#		system ("./get_urls.php --filename=output/$crawl_dir/$id.txt | ./rel2abs -b '$url' > output/$crawl_dir/abs_urls_$id.txt");
		system ("./geturls output/$crawl_dir/$id.txt | ./rel2abs -b '$url' > output/$crawl_dir/abs_urls_$id.txt");
		open (URLS,"<output/$crawl_dir/abs_urls_$id.txt");
		while (<URLS>) {
			$line2 = $_;
			chop($line2);
			if ($line2 =~ m/^http/i) {
				if ($line2 =~ m/\.(...$)/i) {
					$ext = $1;
				} elsif ($line2 =~ m/\.(....$)/i) {
					$ext = $1;
				} elsif ($line2 =~ m/\.(..$)/i) {
					$ext = $1;
				} else {
					undef($ext);
				}
				if ($ext) {
					$ext =~ tr/A-Z/a-z/;
					if (exists($img{$ext})) {
						print IMG ("$line2\n");
					} elsif (exists($doc{$ext})) {
						print DOC ("$line2\n");
					} elsif (exists($crap{$ext})) {
						print CRAP ("$line2\n");
					} elsif (exists($smap{$ext})) {
						print SMAP ("$line2\n");
					} else {
						print OUT ("$line2\n");
					}
				}
			} else {
				print OTHER ("$line2\n");
			}
		}
		unlink ("<output/$crawl_dir/abs_urls_$id.txt");
	} else {
#		print BAD ("$id|$url\n");
		print BAD ("$url\n");
	}
}

close IN;
close OUT;
close IMG;
close GOOD;
close OTHER;
close BAD;
close SMAP;
close CRAP;

mkdir("output/$crawl_dir/extracted");
system("sort -u -S 100M output/$crawl_dir/_good.txt > output/$crawl_dir/extracted/good.txt");
system("sort -u -S 100M output/$crawl_dir/_bad.txt > output/$crawl_dir/extracted/bad.txt");
system("sort -u -S 100M output/$crawl_dir/_images.txt > output/$crawl_dir/extracted/images.txt");
system("sort -u -S 100M output/$crawl_dir/_docs.txt > output/$crawl_dir/extracted/docs.txt");
system("sort -u -S 100M output/$crawl_dir/_urls.txt > output/$crawl_dir/extracted/urls.txt");
system("sort -u -S 100M output/$crawl_dir/_sitemap.txt > output/$crawl_dir/extracted/sitemap.txt");
system("sort -u -S 100M output/$crawl_dir/_crap.txt > output/$crawl_dir/extracted/crap.txt");
system("sort -u -S 100M output/$crawl_dir/_other.txt > output/$crawl_dir/extracted/other.txt");
system("rm -f output/$crawl_dir/_*");
system("touch output/$crawl_dir/extract.done");
