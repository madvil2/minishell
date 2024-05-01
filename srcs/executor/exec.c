#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_ARGS 10

typedef struct {
	char *command;
	char *args[MAX_ARGS];
} Command;

void execute_command(Command cmd) {
	if (fork() == 0) { // Child process
		execvp(cmd.command, cmd.args);
		perror("execvp"); // Only returns on error
		exit(EXIT_FAILURE);
	} else { // Parent process
		wait(NULL); // Wait for the child to finish
	}
}

void execute_pipeline(Command *cmds, int n) {
	int i;
	int pipe_fds[2];
	int in_fd = 0; // Initially set to standard input

	for (i = 0; i < n; i++) {
		pipe(pipe_fds); // Create a pipe

		if (fork() == 0) { // Child process
			if (in_fd != 0) { // If not the first command
				dup2(in_fd, 0); // Replace standard input with the input part of the pipe
				close(in_fd);
			}
			if (i < n - 1) { // If not the last command
				dup2(pipe_fds[1], 1); // Replace standard output with the output part of the pipe
				close(pipe_fds[1]);
			}
			execvp(cmds[i].command, cmds[i].args);
			perror("execvp");
			exit(EXIT_FAILURE);
		} else { // Parent process
			close(pipe_fds[1]); // Close the unused write end
			if (in_fd != 0) {
				close(in_fd); // Close the previous read end
			}
			in_fd = pipe_fds[0]; // Save the read end of the current pipe to be the input for the next command
		}
	}

	// The parent process waits for all child processes
	for (i = 0; i < n; i++) {
		wait(NULL);
	}
}

int main() {
	Command cmds[3] = {
			{"ls", {"ls", "-l", NULL}},
			{"grep", {"grep", "minishell", NULL}},
			{"sort", {"sort", NULL}}
	};

	execute_pipeline(cmds, 3);
	return 0;
}
