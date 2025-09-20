#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

int main(int argc, char *argv[]) {
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
			if (nw < 0){
				perror("write");
				free(buffer);
				if(input != stdin) fclose(input);
				return 1;
			}
			off += (size_t)nw;
		}
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
