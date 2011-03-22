./get_urls.pl > crawl.dat
./crawl
./process_all_crawl_data.pl
./put_data_back.pl
rm -f good.txt bad.txt urls.txt swf.txt
cd output
find . -print | xargs rm
