/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_simple_command.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kokaimov <kokaimov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/23 21:42:41 by kokaimov          #+#    #+#             */
/*   Updated: 2024/06/23 21:54:18 by kokaimov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int  setup_input(char *path, t_token_type type, int flag)
{
	static int	in_fd;
	int			temp;
	struct stat	buf;

	if (flag == SET_REDIR)
	{
		if (dup2(in_fd, STDIN_FILENO) < 0)
		{
			perror("dup2 error");
			exit(EXIT_FAILURE);
		}
		if (in_fd != STDIN_FILENO && in_fd != STDOUT_FILENO)
			close(in_fd);
		return (0);
	}
	stat(path, &buf);
	if (S_ISDIR(buf.st_mode))
	{
		ft_dprintf(2, "%s: is a directory\n", path);
		return (0);
	}
	if (type == TOK_INPUT || type == TOK_HEREDOC)
	{
		temp = in_fd;
		in_fd = open(path, O_RDONLY);
		if (in_fd < 0)
		{
			ft_dprintf(2, "%s: no such file or directory\n", path);
			return (0);
		}
		if (temp != STDIN_FILENO && temp != STDOUT_FILENO)
			close(temp);
	}
	return (1);
}

static int	setup_output(char *path, t_token_type type, int flag)
{
	static int	out_fd;
	struct stat	buf;
	int			temp;

	if (flag == SET_REDIR)
	{
		if (dup2(out_fd, STDOUT_FILENO) < 0)
		{
			perror("dup2 output");
			exit(EXIT_FAILURE);
		}
		if (out_fd != STDOUT_FILENO && out_fd != STDIN_FILENO)
			close(out_fd);
		return (0);
	}
	temp = out_fd;
	if (type == TOK_OVERWRITE)
		out_fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else
		out_fd = open(path, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (out_fd < 0)
	{
		ft_dprintf(2, "%s: Permission denied\n", path);
		return (0);
	}
	if (temp != STDIN_FILENO && temp != STDOUT_FILENO)
		close(temp);
	stat(path, &buf);
	if (S_ISDIR(buf.st_mode))
	{
		ft_dprintf(2, "%s: is a directory\n", path);
		return (0);
	}
	return (1);
}

int setup_redirections(char *str, t_token_type type, int flag)
{
	if (flag == SET_REDIR)
	{
		setup_input(str, type, SET_REDIR);
		setup_output(str, type, SET_REDIR);
		return (0);
	}
	if (type == TOK_INPUT || type == TOK_HEREDOC)
		return (setup_input(str, type, flag));
	else if (type == TOK_OVERWRITE || type == TOK_APPEND)
		return (setup_output(str, type, flag));
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
		if (access(executable_path, X_OK) == 0)//&& stat(executable_path, &statbuf) == 0 && S_ISREG(statbuf.st_mode)
			return (executable_path);
		path_parts++;
	}
	return (NULL);
}

char	**ht_to_envp(t_ht *ht)
{
	char	**envp_array;
	int		i, j;

	envp_array = ft_calloc(ht->nb_entry + 1, sizeof(char *));
	if (!envp_array)
	{
		perror("ft_calloc");
		return (NULL);
	}
	j = 0;
	i = 0;
	while (i < ht->size)
	{
		if (ht->key[i])
		{
			envp_array[j] = ft_strjoin(ht->key[i], "=");
			envp_array[j] = ft_strjoin(envp_array[j], ht->value[i]);
			j++;
		}
		i++;
	}
	envp_array[j] = NULL;
	return (envp_array);
}

int	execute_simple_command(char *program, char **argv)
{
	char	**path_parts;
	char	*executable_path;
	char	**envp;
	char	exp_prefix[2];

	if (!program)
	{
		gc_free(TEMP);
		gc_free(PERM);
		rl_clear_history();
		return (0);
	}
	child_signals_hook();
	exp_prefix[0] = EXP_REPLACE;
	exp_prefix[1] = 0;
	path_parts = ft_split(envp_find(ft_strjoin(exp_prefix, "PATH")), ':');
	executable_path = search_executable(program, path_parts);
	if (!executable_path)
	{
		ft_dprintf(STDERR_FILENO, "%s: Command not found\n", program);
		return (127);
	}
	set_allocator(PERM);
	envp = ht_to_envp(get_envp(NULL));
	set_allocator(TEMP);
	execve(executable_path, argv, envp);
	perror("execve");
	exit(EXIT_FAILURE);
	return (0);
}
