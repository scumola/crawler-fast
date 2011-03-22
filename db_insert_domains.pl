#!/usr/bin/perl
use BerkeleyDB;

$db = new BerkeleyDB::Btree( -Filename => 'domains.db', -Flags => DB_CREATE, -Cachesize => 10000000 ) or die "dead.\n";

while (<>) {
        $line = $_;
        chop($line);
        $db->db_put($line,'1');
}

