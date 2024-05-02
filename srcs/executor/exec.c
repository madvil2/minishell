/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kokaimov <kokaimov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/02 02:47:33 by kokaimov          #+#    #+#             */
/*   Updated: 2024/05/02 03:36:08 by kokaimov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define MAX_ARGS 10

typedef struct {
	char *command;
	char *args[MAX_ARGS];
} Command;

void	execute_command(Command cmd)
{
	int	status;
	pid_t	pid = fork();
	if (pid == 0)
	{
		if (execve(cmd.command, cmd.args, NULL) == -1)
		{
			write(STDERR_FILENO, "Error executing command\n", 24);
			exit(EXIT_FAILURE);
		}
	}
	else if (pid > 0)
		waitpid(pid, &status, 0);
	else
		write(STDERR_FILENO, "Fork failed\n", 12);
}

void execute_pipeline(Command *cmds, int n) {
	int i = 0, in_fd = 0, fd[2];

	while (i < n)
	{
		if (i < n - 1)
		{
			if (pipe(fd) < 0)
			{
				write(STDERR_FILENO, "Pipe failed\n", 12);
				exit(EXIT_FAILURE);
			}
		}

		pid_t pid = fork();
		if (pid == 0)
		{
			if (in_fd != 0)
			{
				dup2(in_fd, 0); // Set stdin to the input part of the pipe
				close(in_fd);
			}
			if (i < n - 1)
			{
				close(fd[0]); // Close the read end of the pipe in the child
				dup2(fd[1], 1); // Set stdout to the output part of the pipe
				close(fd[1]);
			}
			execve(cmds[i].command, cmds[i].args, NULL);
			write(STDERR_FILENO, "Execve failed\n", 14);
			exit(EXIT_FAILURE);
		}
		else if (pid < 0)
			return ((void)write(STDERR_FILENO, "Fork failed\n", 12));
		else
		{
			if (in_fd != 0)
				close(in_fd);
			if (i < n - 1)
			{
				close(fd[1]); // Close the write end of the pipe in the parent
				in_fd = fd[0]; // Keep the read end of the pipe
			}
		}
		i++;
	}
	while (wait(NULL) > 0);
}

int	main()
{
	Command cmds[3] = {
			{"/bin/ls", {"/bin/ls", "-l", NULL}},
			{"/usr/bin/grep", {"/usr/bin/grep", "minishell", NULL}},
			{"/usr/bin/sort", {"/usr/bin/sort", NULL}}
	};

	execute_pipeline(cmds, 3);
	return 0;
}
