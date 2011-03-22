#!/usr/bin/perl

use Mysql;

require "config.txt";
$ENV{'MYSQL_TCP_PORT'} = $dbport;
$dbh = Mysql->connect($host,$dbname,$user,$passwd);

print ("Inserting SWF urls\n");
open (IN, "<swf.txt");
while (<IN>) {
	chop();
	$line = $_;
	$sth = $dbh->query("insert ignore into flashurl values ('','$line','2001-01-01 00:00:00','2001-01-01 00:00:00','','','')");
}
close IN;

print ("Updating bad crawl urls\n");
open (IN, "<bad.txt");
while (<IN>) {
	chop();
	$line = $_;
	($id,$url) = split(/\|/,$line);
	print ("BAD: $url\n");
	$sth = $dbh->query("update crawlurl set state = 999, last_modified = now() where id = '$id'");
}
close IN;

print ("Updating good crawl urls\n");
open (IN, "<good.txt");
while (<IN>) {
	chop();
	$line = $_;
	($id,$url) = split(/\|/,$line);
	print ("GOOD: $url\n");
	$sth = $dbh->query("update crawlurl set state = 1, last_modified = now() where id = '$id'");
}
close IN;

print ("Inserting NEW crawl urls\n");
open (IN, "<urls.txt");
while (<IN>) {
	chop();
	$line = $_;
	($url,$depth) = split(/\|/,$line);
	$depth++;
	$sth = $dbh->query("insert ignore into crawlurl values ('','$url','$depth','2001-01-01 00:00:00','','','')");
}
close IN;
