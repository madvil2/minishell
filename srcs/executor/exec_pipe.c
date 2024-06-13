/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipe.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kokaimov <kokaimov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 18:02:39 by kokaimov          #+#    #+#             */
/*   Updated: 2024/05/18 15:00:54 by kokaimov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "../../includes/minishell.h"
#include <sys/stat.h>

//typedef struct {
//	char	*command;
//	char	*args[10];
//	char	*input; // Path to input file, or NULL if no redirection
//	char	*output; // Path to output file, or NULL if no redirection
//}	Command;
//
//static char	*search_executable(char *program, char **path_parts)
//{
//	struct stat	statbuf;
//	char		*path;
//	char		*executable_path;
//
//	if (ft_strchr(program, '/'))
//	{
//		if (access(program, X_OK) == 0)
//			return (ft_strdup(program));
//		else
//			return (NULL);
//	}
//	while (path_parts && *path_parts)
//	{
//		path = ft_strjoin(*path_parts, "/");
//		executable_path = ft_strjoin(path, program);
//		free(path);
//		if (access(executable_path, X_OK) == 0 && stat(executable_path,
//				&statbuf) == 0 && S_ISREG(statbuf.st_mode))
//			return (executable_path);
//		free(executable_path);
//		path_parts++;
//	}
//	return (NULL);
//}
//
//static void	setup_input(char *path)
//{
//	int	in_fd;
//
//	in_fd = open(path, O_RDONLY);
//	if (in_fd < 0)
//	{
//		perror("open input");
//		exit(EXIT_FAILURE);
//	}
//	if (dup2(in_fd, STDIN_FILENO) < 0)
//	{
//		perror("dup2 input");
//		exit(EXIT_FAILURE);
//	}
//	close(in_fd);
//}
//
//static void	setup_output(char *path)
//{
//	int	out_fd;
//
//	out_fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
//	if (out_fd < 0)
//	{
//		perror("open output");
//		exit(EXIT_FAILURE);
//	}
//	if (dup2(out_fd, STDOUT_FILENO) < 0)
//	{
//		perror("dup2 output");
//		exit(EXIT_FAILURE);
//	}
//	close(out_fd);
//}
//
//static void	setup_redirections(Command cmd)
//{
//	if (cmd.input)
//		setup_input(cmd.input);
//	if (cmd.output)
//		setup_output(cmd.output);
//}
//
//static void	execute_command(Command cmd)
//{
//	int		status;
//	char	**path_parts;
//	char	*executable_path;
//	pid_t	pid;
//
//	path_parts = ft_split(getenv("PATH"), ':');
//	executable_path = search_executable(cmd.command, path_parts);
//	if (!executable_path)
//	{
//		write(STDERR_FILENO, "Command not found\n", 18);
//		free_matrix(path_parts);
//		return ;
//	}
//	pid = fork();
//	if (pid == 0)
//	{
//		setup_redirections(cmd); // Set up redirections before executing
//		execve(executable_path, cmd.args, NULL); // todo: get_envp() instead of NULL
//		perror("execve");
//		exit(EXIT_FAILURE);
//	}
//	else if (pid > 0)
//		waitpid(pid, &status, 0);
//	else
//		perror("fork");
//	free(executable_path);
//	free_matrix(path_parts);
//}
//
//static void	execute_pipeline(Command *cmds, int n) {
//	int	i;
//	int	in_fd;
//	int	fd[2];
//
//	i = 0;
//	in_fd = 0;
//	while (i < n)
//	{
//		if (i < n - 1)
//		{
//			if (pipe(fd) < 0)
//			{
//				perror("pipe");
//				exit(EXIT_FAILURE);
//			}
//		}
//		pid_t pid = fork();
//		if (pid == 0)
//		{
//			if (in_fd != 0)
//			{
//				dup2(in_fd, 0);  // Use previous pipe's read end as stdin
//				close(in_fd);
//			}
//			if (i < n - 1)
//			{
//				close(fd[0]);  // Close the read end of the next pipe
//				dup2(fd[1], 1);  // Use the write end of the next pipe as stdout
//				close(fd[1]);
//			}
//			execute_command(cmds[i]);
//			exit(EXIT_FAILURE);  // Should never reach here unless execution fails
//		}
//		else if (pid < 0)
//		{
//			perror("fork");
//			exit(EXIT_FAILURE);
//		}
//		else
//		{
//			if (in_fd != 0)
//				close(in_fd);  // Close the previous pipe's read end
//			if (i < n - 1)
//			{
//				close(fd[1]);  // Close the current pipe's write end
//				in_fd = fd[0];  // Keep the read end to pass as stdin to next command
//			}
//		}
//		i++;
//	}
//	while (wait(NULL) > 0);
//}

//int	main()
//{
//	Command	cmds[3] = {
//			{"ls", {"/bin/ls", "-l", "-a", NULL}, NULL, NULL},
//			{"grep", {"grep", "minishell", NULL}, NULL, NULL},
//			{"sort", {"sort", NULL}, NULL, "test.txt"}
//	};
//
//	execute_pipeline(cmds, 3);
//	return (0);
//}
