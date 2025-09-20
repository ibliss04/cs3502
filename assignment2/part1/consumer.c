#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <time.h>

int main(int argc, char *argv[]) {
	int max_lines = -1;
	int verbose = 0;
	struct timespec t0, t1;
	clock_gettime(CLOCK_MONOTONIC, &t0);
	int opt;
	while ((opt = getopt(argc, argv, "n:vh")) != -1){
		switch (opt) {
			case 'n': {
				long v = strtol(optarg, NULL, 10);
				if (v < 0) {
					fprintf(stderr, "Invalid -n value: %s\n", optarg);
					return 1;
				}
				max_lines = (int)v;
				break;
			}
			case 'v':
				verbose = 1;
				break;
			case 'h':
			default:
				fprintf(stderr, "Usage: %s [-n max_lines] [-v]\n", argv[0]);
				return (opt == 'h') ? 0 : 1;
		}
	}

	char *line = NULL;
	size_t cap = 0;
	ssize_t len;

	long line_count = 0;
	unsigned long long byte_count = 0;


	while ((len = getline(&line, &cap, stdin)) != -1){
		line_count++;
		byte_count += (unsigned long long)len;

		if (verbose){
			if(fwrite(line, 1, (size_t)len, stdout) < (size_t)len){
				perror("fwrite");
				break;
			}
			fflush(stdout);
		}

		if (max_lines >= 0 && line_count >= max_lines) {
			break;
		}
	}

	if (!feof(stdin) && ferror(stdin)) {
		perror("getline");
	}


	fprintf(stderr, "Lines: %ld\n", line_count);
	fprintf(stderr, "Bytes: %llu\n", byte_count);

	clock_gettime(CLOCK_MONOTONIC, &t1);
	double secs = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) /1e9;
	if (secs > 0.0) {
		fprintf(stderr, "Time: %.3f s\n", secs);
		fprintf(stderr, "Rate: %.2f lines/s, %.3f MB/s\n", (double)line_count /secs, ((double)byte_count / (1024.0 * 1024.0)) /secs);
	}

	free(line);
	return 0;
}

