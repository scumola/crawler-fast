#!/usr/bin/perl

$start_hour = 9;
#$start_hour = 21;
$end_hour = 8;


while (1) {
	($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);
	if ((($hour >= $start_hour) or ($hour < $end_hour))) {
		system ("./doit.sh");
	} else {
		$thetime = localtime();
		print ("CRAWL: $thetime (start: $start_hour:00, end $end_hour:00)\n");
		sleep(300);
	}

}
