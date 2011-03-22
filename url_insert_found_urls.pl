#!/usr/bin/perl
use BerkeleyDB;

$argc = @ARGV;
if ($argc < 1) {
	print ("prog <dbname> < output/xxxxxxxx/extracted/urls.txt\n");
	die;
}
$dbname = $ARGV[0];

$db = new BerkeleyDB::Btree( -Filename => $dbname, -Flags => DB_CREATE, -Cachesize => 10000000 ) or die "dead.\n";
$thetime = time();

while (<STDIN>) {
        $line = $_;
        chop($line);
	if ($db->db_get($line, $v) == 0) {
	} else {
		$db->db_put($line,"");
	}
}

undef $db;
