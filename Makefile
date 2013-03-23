#CFLAGS = -g -pg
#CFLAGS = -g -gstabs
#CFLAGS = -g
#CFLAGS = -g -Wall
CFLAGS = -g -O2
I =
CRAWLL = -lpthread -lcurl
RELABSL = -lpcre
DOMAINSL = -lpcre
CC = gcc

.c.o:
	gcc `xml2-config --cflags --libs` $(I) $(CFLAGS) -c $<

CRAWLO = crawl.o
RELABSO = rel2abs.o
DOMAINSO = domains.o
GETURLSO = geturls.o

all: domains rel2abs crawl geturls tags

geturls: $(GETURLSO)
	gcc -o geturls  $(CFLAGS) $(I) $(GETURLSO) `xml2-config --cflags --libs`

domains: $(DOMAINSO)
	gcc -o domains $(CFLAGS) $(I) $(DOMAINSO) $(DOMAINSL)

rel2abs: $(RELABSO)
	gcc -o rel2abs $(CFLAGS) $(I) $(RELABSO) $(RELABSL)

crawl: $(CRAWLO)
	gcc -o crawl $(CFLAGS) $(I) $(CRAWLO) $(CRAWLL)

tags:
	ctags *.c

clean:
	rm -fv *.o crawl rel2abs domains tags geturls

install:
	mkdir /usr/local/bin
	chmod 755 crawl domains rel2abs geturls
	cp crawl domains rel2abs geturls /usr/local/bin
