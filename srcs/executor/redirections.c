/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_status.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nam-vu <nam-vu@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/27 00:48:29 by nam-vu            #+#    #+#             */
/*   Updated: 2024/07/27 00:48:29 by nam-vu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	is_dir(char *path)
{
	struct stat	buf;

	stat(path, &buf);
	if (S_ISDIR(buf.st_mode))
		return (TRUE);
	return (FALSE);
}

int	redir_flag(int flag)
{
	static int	in_flag;
	static int	out_flag;

	if (flag & SET_IN_FLAG)
		in_flag = 1;
	else if (flag & SET_OUT_FLAG)
		out_flag = 1;
	else if (flag & RESET_IN_FLAG)
		in_flag = 0;
	else if (flag & RESET_OUT_FLAG)
		out_flag = 0;
	else if (flag & GET_IN_FLAG)
		return (in_flag);
	else if (flag & GET_OUT_FLAG)
		return (out_flag);
	return (-1);
}

int	setup_redir(int fd, int flag)
{
	static int	cur_in;
	static int	cur_out;
	static int	stdin_save;
	static int	stdout_save;

//	ft_dprintf(2, "cur_in %i\n cur_out %i\n stdin_save %i\n stdout %i\n", cur_in, cur_out, stdin_save, stdout_save);
//	ft_dprintf(2, "first %i, second %i\n", flag & GET_REDIR, flag & CUR_OUT);
	if ((flag & SET_REDIR) && (flag & CUR_IN))
		cur_in = fd;
	else if ((flag & SET_REDIR) && (flag & CUR_OUT))
		cur_out = fd;
	else if ((flag & SET_REDIR) && (flag & STDIN_SAVE))
		stdin_save = fd;
	else if ((flag & SET_REDIR) && (flag & STDOUT_SAVE))
		stdout_save = fd;
	else if ((flag & GET_REDIR) && (flag & CUR_IN))
		return (cur_in);
	else if ((flag & GET_REDIR) && (flag & CUR_OUT))
		return (cur_out);
	else if ((flag & GET_REDIR) && (flag & STDIN_SAVE))
		return (stdin_save);
	else if ((flag & GET_REDIR) && (flag & STDOUT_SAVE))
		return (stdout_save);
	return (-1);
}

int	change_input(char *path, t_token_type type)
{
	int	temp;

	if (type == TOK_INPUT || type == TOK_HEREDOC)
	{
		temp = open(path, O_RDONLY);
//		ft_dprintf(2, "open infile: %s fd: %i\n", path, temp);
		if (temp < 0)
		{
			ft_dprintf(2, "%s: no such file or directory\n", path);
			return (EXIT_FAILURE);
		}
		if (is_dir(path))
		{
			ft_dprintf(2, "%s: is a directory\n", path);
			return (EXIT_FAILURE);
		}
		if (redir_flag(GET_IN_FLAG))
		{
//			ft_dprintf(2, "close infile fd: %i\n", setup_redir(-1, GET_REDIR | CUR_IN));
			close(setup_redir(-1, GET_REDIR | CUR_IN));
		}
		setup_redir(temp, SET_REDIR | CUR_IN);
	}
	redir_flag(SET_IN_FLAG);
	return (EXIT_SUCCESS);
}

int	change_output(char *path, t_token_type type)
{
	int	temp;

	if (type == TOK_OVERWRITE)
		temp = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else
		temp = open(path, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (temp < 0)
	{
		ft_dprintf(2, "%s: Permission denied\n", path);
		return (EXIT_FAILURE);
	}
//	ft_dprintf(2, "open outfile: %s fd: %i\n", path, temp);
	if (is_dir(path))
	{
		ft_dprintf(2, "%s: is a directory\n", path);
		return (EXIT_FAILURE);
	}
	if (redir_flag(GET_OUT_FLAG))
	{
//		ft_dprintf(2, "close outfile fd: %i\n", setup_redir(-1, GET_REDIR | CUR_OUT));
		close(setup_redir(-1, GET_REDIR | CUR_OUT));
	}
	setup_redir(temp, SET_REDIR | CUR_OUT);
	redir_flag(SET_OUT_FLAG);
	return (EXIT_SUCCESS);
}

int	change_redir(char *path, t_token_type type)
{
	if (type == TOK_APPEND || type == TOK_OVERWRITE)
		return (change_output(path, type));
	if (type == TOK_INPUT || type == TOK_HEREDOC)
		return (change_input(path, type));
	return (EXIT_FAILURE);
}

int	set_input()
{
	if (redir_flag(GET_IN_FLAG))
	{
		if (dup2(setup_redir(-1, GET_REDIR | CUR_IN), STDIN_FILENO) < 0)
		{
			ft_dprintf(STDERR_FILENO, "dup2 set input error fd: %i\n", setup_redir(-1, GET_REDIR | CUR_IN));
			exit(EXIT_FAILURE);
		}
	}
	else
		ft_dprintf(2, "no input redirections\n");
	return (EXIT_SUCCESS);
}

int	set_output()
{
	if (redir_flag(GET_OUT_FLAG))
	{
		if (dup2(setup_redir(-1, GET_REDIR | CUR_OUT), STDOUT_FILENO) < 0)
		{
			ft_dprintf(STDERR_FILENO, "dup2 set output error fd: %i\n", setup_redir(-1, GET_REDIR | CUR_OUT));
			exit(EXIT_FAILURE);
		}
	}
	else
		ft_dprintf(2, "no output redirections\n");
	return (EXIT_SUCCESS);
}

int	save_stdin()
{
	int temp;

	if (redir_flag(GET_IN_FLAG))
	{
		temp = dup(STDIN_FILENO);
		if (temp < 0)
		{
			ft_dprintf(STDERR_FILENO, "dup save stdin error\n");
			exit(EXIT_FAILURE);
		}
		setup_redir(temp, SET_REDIR | STDIN_SAVE);
	}
	return (EXIT_SUCCESS);
}

int	save_stdout()
{
	int temp;

	if (redir_flag(GET_OUT_FLAG))
	{
		temp = dup(STDOUT_FILENO);
		if (temp < 0)
		{
			ft_dprintf(STDERR_FILENO, "dup save stdout error\n");
			exit(EXIT_FAILURE);
		}
		setup_redir(temp, SET_REDIR | STDOUT_SAVE);
	}
	return (EXIT_SUCCESS);
}

int restore_stdin()
{
	if (redir_flag(GET_IN_FLAG))
	{
		close(STDIN_FILENO);
		if (dup2(setup_redir(-1, GET_REDIR | STDIN_SAVE), STDIN_FILENO) < 0)
		{
			ft_dprintf(STDERR_FILENO, "dup2 restore stdin error\n");
			exit(EXIT_FAILURE);
		}
	}
	return (EXIT_SUCCESS);
}

int restore_stdout()
{
	if (redir_flag(GET_OUT_FLAG))
	{
		close(STDOUT_FILENO);
		if (dup2(setup_redir(-1, GET_REDIR | STDOUT_SAVE), STDOUT_FILENO) < 0)
		{
			ft_dprintf(STDERR_FILENO, "dup2 restore stdout error\n");
			exit(EXIT_FAILURE);
		}
	}
	return (EXIT_SUCCESS);
}
