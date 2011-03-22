#!/usr/bin/perl

$argc = @ARGV;
if ($argc < 2) {
	print ("prog <dbname> <timestamp>\n");
	die;
}
$dbname = $ARGV[0];
$timestamp = $ARGV[1];

print ("post-processing all crawl output\n");
system ("./process_all_crawl_data4.pl $timestamp");
print ("pulling in crawl report data\n");
system ("./url_insert_urls_from_crawl_report.pl '$dbname.db' < output/$timestamp/crawl.txt");
print ("pulling in all new found urls\n");
system ("./url_insert_found_urls.pl '$dbname.db' < output/$timestamp/extracted/urls.txt");
system ("./url_insert_found_urls.pl '$dbname-other.db' < output/$timestamp/extracted/images.txt");
system ("./url_insert_found_urls.pl '$dbname-other.db' < output/$timestamp/extracted/other.txt");
system ("./url_insert_found_urls.pl '$dbname-other.db' < output/$timestamp/extracted/crap.txt");
system ("./url_insert_found_urls.pl '$dbname-other.db' < output/$timestamp/extracted/docs.txt");
