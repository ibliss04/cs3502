#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(void) {
	int pipe1[2];
	int pipe2[2];
	pid_t pid;

	if (pipe(pipe1) == -1) { perror("pipe1"); return 1; }
	if (pipe(pipe2) == -1) { perror("pipe2"); return 1; }

	pid = fork();
	if (pid < 0) { perror("fork"); return 1; }

 	if (pid == 0) {
		close(pipe1[1]);
		close(pipe2[0]);

		FILE *in  = fdopen(pipe1[0], "r");
		FILE *out = fdopen(pipe2[1], "w");
		if (!in || !out) { perror("fdopen(child)"); return 1; }

		char *line = NULL; size_t cap = 0; ssize_t n;
		while ((n = getline(&line, &cap, in)) != -1) {
			int is_exit = (strcmp(line, "exit\n") == 0) || (strcmp(line, "exit") == 0);
			if (is_exit) {
				fputs("bye\n", out);
				fflush(out);
				break;
			}
			fputs("ACK: ", out);
			fputs(line, out);
			if (n > 0 && line[n-1] != '\n') fputc('\n', out);
			fflush(out);
		}
		free(line);
		fclose(in);
		fclose(out);
		_exit(0);
	}
	else {
		close(pipe1[0]);
		close(pipe2[1]);

		FILE *to_child   = fdopen(pipe1[1], "w");
		FILE *from_child = fdopen(pipe2[0], "r");
		if (!to_child || !from_child) { perror("fdopen(parent)"); return 1; }

		char *line = NULL; size_t cap = 0;
		char *resp = NULL; size_t rcap = 0;

		printf("Type messages (type 'exit' to quit):\n");
		fflush(stdout);

		while (1) {
			printf("parent> ");
			fflush(stdout);
			ssize_t n = getline(&line, &cap, stdin);
			if (n == -1) {
				fputs("exit\n", to_child);
				fflush(to_child);
				break;
			}
			fputs(line, to_child);
			fflush(to_child);

			ssize_t rn = getline(&resp, &rcap, from_child);
			if (rn == -1) break;
			fputs("child : ", stdout);
			fputs(resp, stdout);

			if (strcmp(line, "exit\n") == 0 || strcmp(line, "exit") == 0) break;
		}

 		free(line);
		free(resp);
		fclose(to_child);
		fclose(from_child);

		int status = 0;
		waitpid(pid, &status, 0);
	}
	return 0;
}

