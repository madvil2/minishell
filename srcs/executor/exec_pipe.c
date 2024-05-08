#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "../../includes/minishell.h"
#include <sys/stat.h>

typedef struct {
	char *command;
	char *args[10];
	char *input;  // Path to input file, or NULL if no redirection
	char *output; // Path to output file, or NULL if no redirection
} Command;


static char	*search_executable(char *program, char **path_parts) {
	struct stat	statbuf;

	if (ft_strchr(program, '/'))
		return (access(program, X_OK) == 0) ? ft_strdup(program) : NULL;

	while (path_parts && *path_parts)
	{
		char *path = ft_strjoin(*path_parts, "/");
		char *executable_path = ft_strjoin(path, program);
		free(path);
		if (access(executable_path, X_OK) == 0 && stat(executable_path, &statbuf) == 0 && S_ISREG(statbuf.st_mode))
			return (executable_path);
		free(executable_path);
		path_parts++;
	}
	return (NULL);
}

static void	setup_redirections(Command cmd) {
	if (cmd.input) {
		int in_fd = open(cmd.input, O_RDONLY);
		if (in_fd < 0) {
			perror("open input");
			exit(EXIT_FAILURE);
		}
		if (dup2(in_fd, STDIN_FILENO) < 0) {
			perror("dup2 input");
			exit(EXIT_FAILURE);
		}
		close(in_fd);
	}

	if (cmd.output) {
		int out_fd = open(cmd.output, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (out_fd < 0) {
			perror("open output");
			exit(EXIT_FAILURE);
		}
		if (dup2(out_fd, STDOUT_FILENO) < 0) {
			perror("dup2 output");
			exit(EXIT_FAILURE);
		}
		close(out_fd);
	}
}

static void execute_command(Command cmd) {
	int	status;
	char	**path_parts = ft_split(getenv("PATH"), ':');
	char	*executable_path = search_executable(cmd.command, path_parts);
	if (!executable_path) {
		write(STDERR_FILENO, "Command not found\n", 18);
		free_matrix(path_parts);
		return;
	}

	pid_t pid = fork();
	if (pid == 0) {
		setup_redirections(cmd);  // Set up redirections before executing
		execve(executable_path, cmd.args, NULL);
		perror("execve");
		exit(EXIT_FAILURE);
	} else if (pid > 0) {
		waitpid(pid, &status, 0);
	} else {
		perror("fork");
	}

	free(executable_path);
	free_matrix(path_parts);
}


void	execute_pipeline(Command *cmds, int n) {
	int	i = 0, in_fd = 0, fd[2];

	while (i < n)
	{
		if (i < n - 1)
		{
			if (pipe(fd) < 0)
			{
				perror("pipe");
				exit(EXIT_FAILURE);
			}
		}

		pid_t pid = fork();
		if (pid == 0)
		{
			if (in_fd != 0)
			{
				dup2(in_fd, 0);  // Use previous pipe's read end as stdin
				close(in_fd);
			}
			if (i < n - 1)
			{
				close(fd[0]);  // Close the read end of the next pipe
				dup2(fd[1], 1);  // Use the write end of the next pipe as stdout
				close(fd[1]);
			}

			execute_command(cmds[i]);
			exit(EXIT_FAILURE);  // Should never reach here unless execution fails
		}
		else if (pid < 0)
		{
			perror("fork");
			exit(EXIT_FAILURE);
		}
		else
		{
			if (in_fd != 0)
				close(in_fd);  // Close the previous pipe's read end
			if (i < n - 1)
			{
				close(fd[1]);  // Close the current pipe's write end
				in_fd = fd[0];  // Keep the read end to pass as stdin to next command
			}
		}
		i++;
	}

	while (wait(NULL) > 0);
}

int	main()
{
	Command	cmds[3] = {
			{"ls", {"ls", "-l", "-a", NULL}, NULL, NULL},
			{"grep", {"grep", "minishell", NULL}, NULL, NULL},
			{"sort", {"sort", NULL}, NULL, NULL}
	};

	execute_pipeline(cmds, 3);
	return (0);
}
