#!/usr/bin/perl
use BerkeleyDB;

$argc = @ARGV;
if ($argc < 1) {
	print ("prog <dbname>\n");
	die;
}
$dbname = $ARGV[0];

$db = new BerkeleyDB::Btree( -Filename => $dbname, -Flags => DB_CREATE, -Cachesize => 10000000 ) or die "dead.\n";

my ($k, $v) = ("", "") ;
my $cursor = $db->db_cursor() ;
while ($cursor->c_get($k, $v, DB_NEXT) == 0) {
	($status, $thetime) = split(/\|/,$v);
	if ($v eq "") {
		print "$k\n";
	}
}

undef $cursor;
undef $db
