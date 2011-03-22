// crawl.c
// By: Steve Webb ( bigwebb@gmail.com )
// License: LGPL

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <getopt.h>
#include <curl/curl.h>

#define IOBUFF_SIZE 32*1024
#define BUFF_SIZE 30000
#define FILENAME "crawl.dat"
//#define FILENAMEOUT "output/crawl.out"
#define DEFAULT_NUM_THREADS 30
#define DELAY_SECONDS 10

#define debug(x) printf("%s:%d:%s(): %s\n", __FILE__, __LINE__, __FUNCTION__, x)

typedef enum { STOPPED, STARTED, FINISHED
} status;

typedef enum { OK, HAMMERED
} domain_status;

typedef struct domains {
	char *domain;
	time_t *last_crawl_time;
	struct domains *next;
} domains;

typedef struct my_url {
	char *url;
	int retcode;
	double old_size;
	double new_size;
	status status;
	struct my_url *next;
	char *filename;
	int url_id;
	int depth;
} my_url;

extern int optind;
extern char *optarg;
static char *progname;
int num_threads;
int max_id = 0;
FILE *status_out;
long unixtime;
char tmpstring[2000];
char useragent[2000];

my_url *root;
my_url *next_url;
domains *my_domains;

/*
char *get_domain(char *url) {
	char *new_url,*new_domain;
	domains *curr_domain,*prev_domain;

	// returns a pointer to the domain name in a newly malloc'ed string
	// parent needs to free() the pointer after using.

	new_url = (char *)malloc(sizeof(url));
	strcpy (new_url,url);	// make a local copy to mangle

	curr_domain = my_domains;
	while (curr_domain) {
		// scan domain list and find (and reset) one
		prev_domain = curr_domain;
		curr_domain = curr_domain->next;
	}
	if (curr_domain == NULL) {
		// not found, make one and set it to now()
		prev_domain->next = (domains *)malloc(sizeof(domains));
		prev_domain->next->domain = new_domain;
		time(prev_domain->next->last_crawl_time);
		prev_domain->next->next = NULL;
	}
}
*/

/*
int is_hammered(my_url *my_url) {
	char *domain;
	domain = get_domain(my_url->url);
}
*/

size_t my_write_func(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	char outbuff[IOBUFF_SIZE];
	setbuf(stream, outbuff);
	return fwrite(ptr, size, nmemb, stream);
}

size_t my_read_func(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	return fread(ptr, size, nmemb, stream);
}

void read_file(char *filename) {
	my_url *tmp_node;
	FILE *in;
	char buffer[BUFF_SIZE];
	int url_id;
	int depth;
	char *url;
	char *token;
	int token_num;
	in = fopen(filename,"r");
	if (!in) {
		printf("Error: can't open file.\n");
		exit(1);
	} else {
		while(fgets(buffer, BUFF_SIZE, in)!=NULL) { 
			token_num = 0;
			url_id=max_id + 1;
			depth=0;
			token = strtok(buffer,"|\n");
			while (token) {
				if (token_num == 0) {
					url = (char *)malloc(sizeof(char) * (strlen(token)+1));
					strcpy(url,token);
				}
				if (token_num == 1) {
					url_id = atoi(token);
				}
				if (token_num == 2) {
					depth = atoi(token);
				}
				token_num++;
				token = strtok(NULL,"|\n");
			}
			tmp_node = (my_url *)malloc(sizeof(my_url));
			tmp_node->old_size = 0;
			tmp_node->url = url;
			tmp_node->depth = depth;
			tmp_node->url_id = url_id;
			tmp_node->status = STOPPED;
			tmp_node->next = root;
			root = tmp_node;		// inserts in reverse-order
			if (url_id > max_id)
				max_id = url_id;
		}
	}
	fclose(in);
}

my_url *get_next_url(void) {
	my_url *ret;
	if (next_url) {
		if ((next_url == root) && (next_url->next)){
			next_url = next_url->next;
			ret = root;
		} else if (next_url->next) {
			ret = next_url;
			next_url = next_url->next;
		} else {
			ret = next_url;
			if (next_url) {
				next_url = next_url->next;
			}
		}
	} else {
		ret = NULL;
	}
	return (ret);
}

void *pull_one_url(void *thread_num) {
	CURL *curl;
	FILE *outfile;
	FILE *headerfile;
//	FILE *out;
	char outfilename[100];
	char headerfilename[100];
	my_url *tmp_url;
	double size;
	long retcode;
	CURLcode ret;
	static int counter;

	tmp_url = get_next_url();
	while (tmp_url) {
		counter++;
		sprintf(outfilename,"output/%ld/%d.txt",unixtime,tmp_url->url_id);
		outfile = fopen(outfilename, "w");
		sprintf(headerfilename,"output/%ld/%d.headers.txt",unixtime,tmp_url->url_id);
		headerfile = fopen(headerfilename, "w");
		curl = curl_easy_init();
		tmp_url->status = STARTED;
		curl_easy_setopt(curl, CURLOPT_URL, tmp_url->url);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, outfile);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_write_func);
		curl_easy_setopt(curl, CURLOPT_WRITEHEADER, headerfile);
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, my_write_func);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_ENCODING, "deflate");
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 20);
		curl_easy_setopt(curl, CURLOPT_MAXFILESIZE, 2048000);
//		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, DELAY_SECONDS);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, useragent);
		curl_easy_perform(curl);
		ret = curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD,&size);
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE,&retcode);
		tmp_url->status = FINISHED;
		tmp_url->retcode = retcode;
		tmp_url->filename = strdup(outfilename);
		curl_easy_cleanup(curl);
		fclose(outfile);
		fclose(headerfile);
		if ((retcode == 200) && (size > 0)) {
			tmp_url->new_size = size;
			fprintf (stderr,"%03d %7g [%3d] %s\n",(int)thread_num,size,(int)retcode,tmp_url->url);
		} else {
			tmp_url->new_size = tmp_url->old_size;
			fprintf (stderr,"%03d ------- [%3d] %s\n",(int)thread_num,(int)retcode,tmp_url->url);
			unlink(outfilename);
			unlink(headerfilename);
		}
		fprintf(status_out,"%d|%d|%s\n",tmp_url->retcode,tmp_url->url_id,tmp_url->url);
//		fflush(status_out);
		tmp_url = get_next_url();
	}
	return NULL;
}

/*
void write_file (char *filename) {
	FILE *out;
	my_url *tmp_url;

	printf("writing output file\n");
	tmp_url = root;
	out = fopen(filename,"w");
	while (tmp_url) {
		fprintf(out,"%d|%d|%s|%d\n",tmp_url->retcode,tmp_url->url_id,tmp_url->url,tmp_url->depth);
		tmp_url = tmp_url->next;
	}
	fclose(out);
}
*/

static void usage() {
  fprintf (stderr, "%s: usage: %s [-u \"useragent string\"] [-t num_threads] [-h]\n", progname, progname);
}

int main(int argc, char **argv) {
	int i;
	int opt;
	int error;

	num_threads = DEFAULT_NUM_THREADS;

	unixtime = (long)time(NULL);
	system("mkdir output");
	sprintf(tmpstring,"mkdir output/%ld",unixtime);
	system(tmpstring);
	sprintf(tmpstring,"output/%ld/crawl.txt",unixtime);
	status_out = fopen(tmpstring,"w");

	strcpy(useragent,"Steve Webb's Crawler (bigwebb@gmail.com)");

	progname = strrchr(argv[0], '/');
   	while ((opt = getopt(argc, argv, "t:u:h")) != EOF) {
		switch (opt) {
			case 'u':
				strcpy(useragent,optarg);
				break;
			case 't':
				num_threads = atoi(optarg);
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

	pthread_t tid[num_threads];

	root = NULL;
	my_domains = NULL;

	read_file(FILENAME);
	next_url = root;

	for(i=0; i< num_threads; i++) {
		error = pthread_create(&tid[i],
				   NULL, /* default attributes please */
				   pull_one_url,
				   (void *)i);
		if (0 != error) {
			fprintf(stderr, "Couldn't run thread number %d, errno %d\n", i, error);
		}
	}
	fprintf(stderr,"Started %d threads.\n",num_threads);

	for(i=0; i< num_threads; i++) {
		error = pthread_join(tid[i], NULL);
	}

//	unlink(FILENAME);
//	write_file(FILENAMEOUT);

	fclose(status_out);
	sprintf(tmpstring,"touch output/%ld/crawl.done",unixtime);
	system(tmpstring);
	return 0;
}
