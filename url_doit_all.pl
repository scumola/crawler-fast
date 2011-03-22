#!/usr/bin/perl

$argc = @ARGV;
if ($argc < 1) {
	print ("prog <sitename>\n");
	die;
}
$dbname = $ARGV[0];

$some_dir = "output";
opendir(DH, $some_dir) || die "can't opendir $some_dir: $!";
while ($file=readdir(DH)) {
	if ($file =~ /^\./) {
	} else {
		unless ( -e "$some_dir/$file/extract.done") {
			print ("POST-PROCESSING: $some_dir/$file\n");
			system("./url_doit_one_dir.pl '$dbname' $file");
		}
	}
}
closedir DH;

print ("pulling new work out of DB\n");
mkdir ("new_work");
system ("cd new_work ; rm -f *");
system ("cd new_work ; ../url_get_unfetched.pl '../$dbname.db' | /usr/bin/split -a 10 -l 100000");
