// rel2abs.c
// By: Steve Webb ( bigwebb@gmail.com )
// License: LGPL
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <pcre.h>

#define IOBUFF_SIZE 32*1024
#define BUFF_SIZE 8192
#define TOKEN_SIZE 8192
#define VECTOR_SIZE 186 /* has to be a multiple of 3 */

static char *progname;
extern int optind;
extern char *optarg;
int aggressive = 0;
int debug = 0;

pcre *re_http;
pcre *re_path;
pcre *re_url_parts;
//pcre *re_url_domainonly;

static void usage() {
	fprintf (stderr, "Usage: %s [-h] -a -b <base url> [-f <filename>]\n", progname);
}

int main (int argc, char **argv) {
	char buffer[BUFF_SIZE];
	char *orig_url;
	const char *error;
	int erroffset,opt,rc;
	int ovector[VECTOR_SIZE];
	int base_url_has_no_path = 0;
	char *filename = NULL;
	char tmpstring[TOKEN_SIZE];
	char relurl[TOKEN_SIZE];
	char baseurl[TOKEN_SIZE];
	char basedomain[TOKEN_SIZE];
	char word1[TOKEN_SIZE];
	char word2[TOKEN_SIZE];
	char basepath_word1[TOKEN_SIZE];
	char basepath_word2[TOKEN_SIZE];
	char base_word1[TOKEN_SIZE];
	char base_word2[TOKEN_SIZE];
	char base_word3[TOKEN_SIZE];
        char inbuff[IOBUFF_SIZE];
        char outbuff[IOBUFF_SIZE];
	FILE *fp;

        setbuf(stdin, inbuff);
        setbuf(stdout, outbuff);

	progname = argv[0];
	if (argc < 2) {
		usage();
		exit(2);
	}

	re_http = pcre_compile("^(http:|https:|ftp:|file:|mailto:|irc:|news:|gopher:|nfs:|telnet:|rlogin:|tn3270:|mailserver:|finger:)",PCRE_CASELESS|PCRE_DOTALL, &error, &erroffset, NULL);
	re_path = pcre_compile("^(.*)/(.*)",PCRE_CASELESS|PCRE_DOTALL, &error, &erroffset, NULL);
//	re_url_domainonly = pcre_compile("^(http|https)://([a-zA-Z.0-9-:]+)",PCRE_CASELESS|PCRE_DOTALL, &error, &erroffset, NULL);
	re_url_parts = pcre_compile("^(http|https)://([a-zA-Z.0-9-:]+)(/*.*)",PCRE_CASELESS|PCRE_DOTALL, &error, &erroffset, NULL);

   	while ((opt = getopt(argc, argv, "ab:f:h")) != EOF) {
		switch (opt) {
			case 'a':
				aggressive = 1;
				break;
			case 'f':
				filename = optarg;
				break;
			case 'b':
				orig_url = optarg;
				strcpy(buffer,orig_url);
//				strcpy(baseurl,orig_url);
//				printf ("BUFFER: %s\n",buffer);
				rc = pcre_exec(re_url_parts, NULL, buffer, strlen(buffer), 0, 0, ovector, VECTOR_SIZE);
				if (rc > 0 ) {
					pcre_copy_substring(buffer, ovector, rc, 1, base_word1, TOKEN_SIZE);
					pcre_copy_substring(buffer, ovector, rc, 2, base_word2, TOKEN_SIZE);
					pcre_copy_substring(buffer, ovector, rc, 3, base_word3, TOKEN_SIZE);
					if (debug) {
						printf("BASE PARTS: %s|%s|%s\n",base_word1,base_word2,base_word3);
					}
					if (strlen(base_word3) > 0) {
						rc = pcre_exec(re_path, NULL, base_word3, strlen(base_word3), 0, 0, ovector, VECTOR_SIZE);
						if (rc > 0 ) {
							pcre_copy_substring(base_word3, ovector, rc, 1, basepath_word1, TOKEN_SIZE);
							pcre_copy_substring(base_word3, ovector, rc, 2, basepath_word2, TOKEN_SIZE);
							if (debug) {
								printf("BASE PATH PARTS: %s|%s\n",basepath_word1,basepath_word2);
							}
						}
					}
				} else {
					if (debug) {
						printf ("INVALID BASE URL: %s\n",buffer);
					}
				}
				break;
			case 'h':
				usage();
				exit(2);
				break;
			default:
			usage ();
			exit (2);
		}
	}

	if (!orig_url) {
		printf ("ARG: -b required - exiting\n");
		exit(1);
	}
	// construct baseurl with machine name
	strcpy(baseurl,base_word1);
	strcat(baseurl,"://");
	strcat(baseurl,base_word2);
	// if there is a leading slash in the path, then don't put a trailing slash after the domain name
	if (base_word3[0] == '/') {
		strcat(baseurl,"/");
	}
	strcpy(basedomain,baseurl);
	// for the domain, we always want a trailing slash if it doesn't already exist
	if (baseurl[strlen(baseurl)-1] != '/') {
		strcat(baseurl,"/");
	}
	strcpy(basedomain,baseurl);
	// now our domains all end in a slash
	if (strlen(basepath_word1) > 0) {
		if (baseurl[strlen(baseurl)-1] == '/') {
			// chop off the trailing slash before appending the first part of the path
			baseurl[strlen(baseurl)-1] = '\0';
		}
		strcat(baseurl,basepath_word1);
		strcat(baseurl,"/");
	}

	// check to see if we need to just append the urls to the full url, or just replace the file with the new url, or both.
/*
	if ((basepath_word1[0] == '\0') || (basepath_word2[0] == '\0')) {
		base_url_has_no_path = 1;
	} else {
		base_url_has_no_path = 0;
		if (debug) {
			printf ("NOTE: doing both url types because base url doesn't have a trailing slash\n");
		}
	}
*/

	if (debug) {
		printf ("*** base_url_has_no_path: %d\n",base_url_has_no_path);
		printf ("*** base_word1: %s\n",base_word1);
		printf ("*** base_word2: %s\n",base_word2);
		printf ("*** base_word3: %s\n",base_word3);
		printf ("*** basepath_word1: %s\n",basepath_word1);
		printf ("*** basepath_word2: %s\n",basepath_word2);
//		printf ("*** word1: %s\n",word1);
//		printf ("*** word2: %s\n",word2);
		printf ("*** baseurl: %s\n",baseurl);
		printf ("*** basedomain: %s\n",basedomain);
	}

	if (filename) {
		fp = fopen (filename,"r");
	} else {
		fp = stdin;
	}
	while(fgets(buffer, BUFF_SIZE, fp)!=NULL) { 
		// chop off the carriage return
		buffer[strlen(buffer)-1] = '\0';

		strcpy(relurl,buffer);
		rc = pcre_exec(re_http, NULL, buffer, strlen(buffer), 0, 0, ovector, VECTOR_SIZE);
		if (rc > 0 ) {
			if (debug) {
				printf ("%s\n",buffer);
			} else {
				printf ("%s\n",buffer);
			}
		} else {
			if (debug) {
				printf ("REL: %s\n",buffer);
			}
			rc = pcre_exec(re_path, NULL, buffer, strlen(buffer), 0, 0, ovector, VECTOR_SIZE);
			if (rc > 0 ) {
				pcre_copy_substring(buffer, ovector, rc, 1, word1, TOKEN_SIZE);
				pcre_copy_substring(buffer, ovector, rc, 2, word2, TOKEN_SIZE);
//				if (debug) {
//					printf("REGEXP: re_path parts: %s|%s\n",word1,word2);
//				}
			} else {
//				if (debug) {
//					printf ("\n");
//				}
			}
			/*
			if (base_url_has_no_path) {
				if (baseurl[strlen(baseurl)-1] == '/') {
					// base url has a trailing slash
					if (relurl[0] == '/') {
						// base url has a trailing and rel url has a leading slash!
						// TODO: chop off the last character of base url
						strcpy(tmpstring,baseurl);
						tmpstring[strlen(tmpstring)-1] = '\0';
						printf ("%s%s\n",tmpstring,relurl);
					} else {
						printf ("%s%s\n",basepath_word1,relurl);
					}
				} else {
					// base url doesn't have a trailing slash
					if (relurl[0] == '/') {
						printf ("%s%s\n",basepath_word1,relurl);
					} else {
						printf ("%s/%s\n",basepath_word1,relurl);
					}
				}
			} else {
			*/
				// base url has a path
//				printf ("%s%s\n",baseurl,relurl);
				if (relurl[0] == '/') {
					// if rel url has a leading slash, chop off the path from the base url
					// TODO: chop off the first character of rel url or last char of base url
					strcpy(tmpstring,basedomain);
					tmpstring[strlen(tmpstring)-1] = '\0';
					printf ("%s%s\n",tmpstring,relurl);
				} else {
					printf ("%s%s\n",baseurl,relurl);
//					printf ("%s%s\n",basepath_word1,relurl);
				}
			/*
			}
			*/
		}
	}
	exit(0);
}
