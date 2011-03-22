#!/usr/bin/perl
use BerkeleyDB;

$argc = @ARGV;
if ($argc < 1) {
	print ("prog <dbname> < output/xxxxxxxx/crawl.txt\n");
	die;
}
$dbname = $ARGV[0];

$db = new BerkeleyDB::Btree( -Filename => $dbname, -Flags => DB_CREATE, -Cachesize => 10000000 ) or die "dead.\n";
$thetime = time();

while (<STDIN>) {
        $line = $_;
        chop($line);
	($status, $id, $url) = split(/\|/,$line);
	$db->db_put($url,"$status|$thetime");
}

undef $db;
