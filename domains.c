// domains.c
// By: Steve Webb ( bigwebb@gmail.com )
// License: LGPL
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include <pcre.h>

#define IOBUFF_SIZE 32*1024
#define BUFF_SIZE 8192
#define TOKEN_SIZE 8192
#define VECTOR_SIZE 186 /* has to be a multiple of 3 */

static char *progname;
extern int optind;
extern char *optarg;

pcre *re_domains;

static void usage() {
	fprintf (stderr, "Usage: %s [-h]]\n", progname);
}

int main (int argc, char **argv) {
	char buffer[BUFF_SIZE];
	char inbuff[IOBUFF_SIZE];
	char outbuff[IOBUFF_SIZE];
        setbuf(stdin, inbuff);
        setbuf(stdout, outbuff);

	const char *error;
	int erroffset,opt,rc,a;
	int ovector[VECTOR_SIZE];
	char word1[TOKEN_SIZE];
	FILE *fp;

	progname = argv[0];
	re_domains = pcre_compile("^http://([-a-z0-9.]*)",PCRE_CASELESS|PCRE_DOTALL, &error, &erroffset, NULL);
   	while ((opt = getopt(argc, argv, "ab:f:h")) != EOF) {
		switch (opt) {
			case 'h':
				usage();
				exit(2);
				break;
			default:
			usage ();
			exit (2);
		}
	}

	fp = stdin;
	while(fgets(buffer, BUFF_SIZE, fp)!=NULL) { 
		buffer[strlen(buffer)-1] = '\0';
		rc = pcre_exec(re_domains, NULL, buffer, strlen(buffer), 0, 0, ovector, VECTOR_SIZE);
		if (rc > 0 ) {
			pcre_copy_substring(buffer, ovector, rc, 1, word1, TOKEN_SIZE);
			// convert all to lower-case (domains are not case-sensitive)
			for (a=0;a<strlen(word1);a++) {
				word1[a] = tolower(word1[a]);
			}
			printf ("%s\n",word1);
		}
	}
	exit(0);
}
