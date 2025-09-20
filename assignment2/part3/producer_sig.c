#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <signal.h>

unsigned long long bytes_out = 0;
volatile sig_atomic_t shutdown_flag = 0;
volatile sig_atomic_t stats_flag = 0;

void handle_sigint(int sig){
	shutdown_flag = 1;
}
void handle_sigusr1(int sig) {
	stats_flag = 1;
}

int main(int argc, char *argv[]) {

	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	sa.sa_handler = handle_sigint;
	sigaction(SIGINT, &sa, NULL);

	sa.sa_handler = handle_sigusr1;
	sigaction(SIGUSR1, &sa, NULL);
	FILE *input = stdin;
	int buffer_size = 4096;
	int opt;

	char *filename = NULL;
	while ((opt = getopt(argc, argv, "f:b:h")) != -1) {
		switch (opt){
			case 'f':
				filename = optarg;
				break;
			case 'b': {
				long v = strtol(optarg, NULL,10);
				if (v <= 0) {
					fprintf(stderr, "Invalid buffer size: %s\n", optarg);
					return 1;
				}
				buffer_size = (int)v;
				break;
			}
			case 'h':
			default:
				fprintf(stderr, "Usage: %s [-f filename] [-b buffer_size]\n", argv[0]);
				return (opt == 'h') ? 0 : 1;
		}
	}

	if (filename) {
		input =  fopen(filename, "rb");
		if(!input){
			perror("fopen");
			return 1;
		}
	}

	char *buffer = (char *)malloc((size_t)buffer_size);
	if (!buffer) {
		perror("malloc");
		if  (input != stdin) fclose(input);
		return 1;
	}

	size_t nread;
	while((nread = fread(buffer, 1, (size_t)buffer_size, input)) > 0){
		size_t off = 0;
		while(off < nread){
			ssize_t nw = write(STDOUT_FILENO, buffer + off, nread - off);
			bytes_out += nread;
			if (nw < 0){
				perror("write");
				free(buffer);
				if(input != stdin) fclose(input);
				return 1;
			}
			off += (size_t)nw;
			bytes_out += (unsigned long long)nw;
		}

		if (stats_flag){
			fprintf(stderr, "[producer bytes so far: %lli\n", bytes_out);
		}
		usleep (10000*1000);
	}

	if (ferror(input)) {
		perror("fread");
	}

	free(buffer);
	if (input != stdin){
		 fclose(input);
	}
	return 0;
}
