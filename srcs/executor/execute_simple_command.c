/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_simple_command.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nam-vu <nam-vu@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/13 04:12:00 by nam-vu            #+#    #+#             */
/*   Updated: 2024/06/13 04:12:00 by nam-vu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int  setup_input(char *path, t_token_type type)
{
	int	in_fd;

	if (type == TOK_INPUT || type == TOK_HEREDOC)
	{
		in_fd = open(path, O_RDONLY);
		if (in_fd < 0)
		{
			ft_dprintf(2, "%s: no such file or directory\n", path);
			return (0);
		}
		if (dup2(in_fd, STDIN_FILENO) < 0)
		{
			perror("dup2 error");
			exit(EXIT_FAILURE);
		}
		if (in_fd != STDIN_FILENO)
			close(in_fd);
	}
	return (1);
}

static int	setup_output(char *path, t_token_type type)
{
	int	out_fd;

	if (type == TOK_OVERWRITE)
		out_fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else
		out_fd = open(path, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (out_fd < 0)
	{
	ft_dprintf(2, "%s: Permission denied\n", path);
		return (0);
	}
	if (dup2(out_fd, STDOUT_FILENO) < 0)
	{
		perror("dup2 output");
		exit(EXIT_FAILURE);
	}
	if (out_fd != STDOUT_FILENO)
		close(out_fd);
	return (1);
}

int setup_redirections(char *str, t_token_type type)
{
	struct stat buf;

	stat(str, &buf);
	if (S_ISDIR(buf.st_mode))
	{
		ft_dprintf(2, "%s: is a directory\n", str);
		return (0);
	}
	if (type == TOK_INPUT || type == TOK_HEREDOC)
		return (setup_input(str, type));
	else if (type == TOK_OVERWRITE || type == TOK_APPEND)
		return (setup_output(str, type));
	return (-1);
}

static char	*search_executable(char *program, char **path_parts)
{
//	struct stat	statbuf;
	char		*path;
	char		*executable_path;

	if (ft_strchr(program, '/'))
	{
		if (access(program, X_OK) == 0)
			return (ft_strdup(program));
		else
			return (NULL);
	}
	while (path_parts && *path_parts)
	{
		path = ft_strjoin(*path_parts, "/");
		executable_path = ft_strjoin(path, program);
//		free(path); not needed
		if (access(executable_path, X_OK) == 0)//&& stat(executable_path, &statbuf) == 0 && S_ISREG(statbuf.st_mode)
			return (executable_path);
		free(executable_path);
		path_parts++;
	}
	return (NULL);
}

int	execute_simple_command(char *program, char **argv)
{
	int		status;
	char	**path_parts;
	char	*executable_path;
	pid_t	pid;
	char	exp_prefix[2];

	exp_prefix[0] = EXP_REPLACE;
	exp_prefix[1] = 0;
	path_parts = ft_split(envp_find(ft_strjoin(exp_prefix, "PATH")), ':');
	executable_path = search_executable(program, path_parts);// todo: remove searching if theres '/' in the path
	if (!executable_path)
	{
		ft_dprintf(STDERR_FILENO, "%s: ", program);
		write(STDERR_FILENO, "Command not found\n", 18);
		return (127);
	}
	pid = fork();
	if (pid == 0)
	{
		execve(executable_path, argv, NULL); // todo: get_envp() instead of NULL
		perror("execve");
		exit(EXIT_FAILURE);
	}
	else if (pid > 0)
		waitpid(pid, &status, 0);
	else
		perror("fork");
	return (0);
}