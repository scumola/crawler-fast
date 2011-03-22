#!/bin/sh

# delete all lines that contain https
sed -i '/https/ d' crawl.dat

# delete anything without a leading http://
sed -i '/^http:\/\// !d' crawl.dat

tr A-Z a-z < crawl.dat | sort -u > sorteduniq.dat
rm -f crawl.dat
mv sorteduniq.dat crawl.dat
